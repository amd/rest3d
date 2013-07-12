/*
Copyright (c) 2013 Khaled Mammou - Advanced Micro Devices, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#pragma once
#ifndef X3DGC_BINARY_STREAM_H
#define X3DGC_BINARY_STREAM_H

#include "x3dgc_Common.h"
#include "x3dgc_Vector.h"

namespace x3dgc
{
    const unsigned long X3DGC_BINARY_STREAM_DEFAULT_SIZE       = 4096;
    const unsigned long X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0   = 7;
    const unsigned long X3DGC_BINARY_STREAM_MAX_SYMBOL0        = (1 << X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0) - 1;
    const unsigned long X3DGC_BINARY_STREAM_BITS_PER_SYMBOL1   = 6;
    const unsigned long X3DGC_BINARY_STREAM_MAX_SYMBOL1        = (1 << X3DGC_BINARY_STREAM_BITS_PER_SYMBOL1) - 1;
    const unsigned long X3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32 = (32+X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0-1) / 
                                                                 X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0;

    //! 
    class BinaryStream
    {
    public:    
        //! Constructor.
                                BinaryStream(size_t size = X3DGC_BINARY_STREAM_DEFAULT_SIZE)
                                {
                                    m_endianness = SystemEndianness();
                                    m_stream.Allocate(size);
                                };
        //! Destructor.
                                ~BinaryStream(void){};

        void                    WriteFloat32ASCII(float value) 
                                {
                                    unsigned long uiValue = *((unsigned long *)(&value));
                                    WriteUInt32ASCII(uiValue);
                                }
        void                    WriteUInt32ASCII(unsigned long position, unsigned long value) 
                                {
                                    assert(position < m_stream.GetSize() - X3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32);
                                    for(unsigned long i = 0; i < X3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32; ++i)
                                    {
                                        m_stream[position++] = value & X3DGC_BINARY_STREAM_MAX_SYMBOL0;
                                        value >>= X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0;
                                    }
                                }
        void                    WriteUInt32ASCII(unsigned long value) 
                                {
                                    for(long i = 0; i < X3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32; ++i)
                                    {
                                        m_stream.PushBack(value & X3DGC_BINARY_STREAM_MAX_SYMBOL0);
                                        value >>= X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0;
                                    }
                                }
        void                    WriteIntASCII(long value) 
                                {
                                    unsigned int uiValue;
                                    if (value < 0)
                                    {
                                        uiValue = (unsigned long) (1 - (2 * value));
                                    }
                                    else
                                    {
                                        uiValue = (unsigned long) (2 * value);
                                    }
                                    WriteUIntASCII(uiValue);
                                }
        void                    WriteUIntASCII(unsigned long value) 
                                {
                                    if (value >= X3DGC_BINARY_STREAM_MAX_SYMBOL0)
                                    {
                                        m_stream.PushBack(X3DGC_BINARY_STREAM_MAX_SYMBOL0);
                                        value -= X3DGC_BINARY_STREAM_MAX_SYMBOL0;
                                        unsigned char a, b;
                                        do
                                        {
                                            a  = ((value & X3DGC_BINARY_STREAM_MAX_SYMBOL1) << 1);
                                            b  = ( (value >>= X3DGC_BINARY_STREAM_BITS_PER_SYMBOL1) > 0);
                                            a += b;
                                            m_stream.PushBack(a);
                                        } while (b);
                                    }
                                    else
                                    {
                                        m_stream.PushBack((unsigned char) value);
                                    }
                                }
        void                    WriteUCharASCII(unsigned char value) 
                                {
                                    assert(value <= X3DGC_BINARY_STREAM_MAX_SYMBOL0);
                                    m_stream.PushBack(value);
                                }
        float                   ReadFloat32ASCII(unsigned long & position) const
                                {
                                    unsigned long value = ReadUInt32ASCII(position);
                                    float fvalue = *((float *)(&value));
                                    return fvalue;
                                }
        unsigned long           ReadUInt32ASCII(unsigned long & position)  const
                                {
                                    assert(position < m_stream.GetSize() - X3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32);
                                    unsigned long value = 0;
                                    unsigned long shift = 0;
                                    for(long i = 0; i < X3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32; ++i)
                                    {
                                        value  += (m_stream[position++] << shift);
                                        shift  += X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0;
                                    }                                    
                                    return value;
                                }
        long                    ReadIntASCII(unsigned long & position) const
                                {
                                    unsigned long uiValue = ReadUIntASCII(position);
                                    if (uiValue & 1)
                                    {
                                        return -((long) (uiValue >> 1));
                                    }
                                    else
                                    {
                                        return ((long) (uiValue >> 1));
                                    }
                                }
        unsigned long           ReadUIntASCII(unsigned long & position) const
                                {
                                    unsigned long value = m_stream[position++];
                                    if (value == X3DGC_BINARY_STREAM_MAX_SYMBOL0)
                                    {
                                        long x;
                                        unsigned long i = 0;
                                        do
                                        {
                                            x = m_stream[position++];
                                            value += ( (x>>1) << i);
                                            i += X3DGC_BINARY_STREAM_BITS_PER_SYMBOL1;
                                        } while (x & 1);
                                    }
                                    return value;
                                }
        unsigned char           ReadUCharASCII(unsigned long & position) const
                                {
                                    return m_stream[position++];
                                }
        X3DGCErrorCode          Save(const char * const fileName) 
                                {
                                    FILE * fout = fopen(fileName, "wb");
                                    if (!fout)
                                    {
                                        return X3DGC_ERROR_CREATE_FILE;
                                    }
                                    fwrite(m_stream.GetBuffer(), 1, m_stream.GetSize(), fout);
                                    fclose(fout);
                                    return X3DGC_OK;
                                }
        X3DGCErrorCode          Load(const char * const fileName) 
                                {
                                    FILE * fin = fopen(fileName, "rb");
                                    if (!fin)
                                    {
                                        return X3DGC_ERROR_OPEN_FILE;
                                    }
                                    fseek(fin, 0, SEEK_END);
                                    unsigned long size = ftell(fin);
                                    m_stream.Allocate(size);
                                    rewind(fin);
                                    unsigned int nread = fread((void *) m_stream.GetBuffer(), 1, size, fin);
                                    m_stream.SetSize(size);
                                    if (nread != size)
                                    {
                                        return X3DGC_ERROR_READ_FILE;
                                    }
                                    fclose(fin);
                                    return X3DGC_OK;
                                }
        size_t                  GetSize() const
                                {
                                    return m_stream.GetSize();
                                }
    private:
        Vector<unsigned char>   m_stream;
        X3DGCEndianness         m_endianness;
    };

}
#endif // X3DGC_BINARY_STREAM_H

