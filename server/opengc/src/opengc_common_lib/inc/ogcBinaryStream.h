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
#ifndef OGC_BINARY_STREAM_H
#define OGC_BINARY_STREAM_H

#include "ogcCommon.h"
#include "ogcVector.h"

namespace ogc
{
    const unsigned long OGC_BINARY_STREAM_DEFAULT_SIZE       = 4096;
    const unsigned long OGC_BINARY_STREAM_BITS_PER_SYMBOL0   = 7;
    const unsigned long OGC_BINARY_STREAM_MAX_SYMBOL0        = (1 << OGC_BINARY_STREAM_BITS_PER_SYMBOL0) - 1;
    const unsigned long OGC_BINARY_STREAM_BITS_PER_SYMBOL1   = 6;
    const unsigned long OGC_BINARY_STREAM_MAX_SYMBOL1        = (1 << OGC_BINARY_STREAM_BITS_PER_SYMBOL1) - 1;
    const unsigned long OGC_BINARY_STREAM_NUM_SYMBOLS_UINT32 = (32+OGC_BINARY_STREAM_BITS_PER_SYMBOL0-1) / 
                                                                 OGC_BINARY_STREAM_BITS_PER_SYMBOL0;

    //! 
    class BinaryStream
    {
    public:    
        //! Constructor.
                                BinaryStream(size_t size = OGC_BINARY_STREAM_DEFAULT_SIZE)
                                {
                                    m_endianness   = SystemEndianness();
                                    m_stream.Allocate(size);
                                };
        //! Destructor.
                                ~BinaryStream(void){};

        void                    WriteFloat32(float value, OGCSC3DMCStreamType streamType)
                                {
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        WriteFloat32ASCII(value);
                                    }
                                    else
                                    {
                                        WriteFloat32Bin(value);
                                    }
                                }
        void                    WriteUInt32(unsigned long position, unsigned long value, OGCSC3DMCStreamType streamType)
                                {
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        WriteUInt32ASCII(position, value);
                                    }
                                    else
                                    {
                                        WriteUInt32Bin(position, value);
                                    }
                                }
        void                    WriteUInt32(unsigned long value, OGCSC3DMCStreamType streamType)
                                {
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        WriteUInt32ASCII(value);
                                    }
                                    else
                                    {
                                        WriteUInt32Bin(value);
                                    }
                                }
        void                    WriteUChar(unsigned int position, unsigned char value, OGCSC3DMCStreamType streamType)
                                {
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        WriteUInt32ASCII(position, value);
                                    }
                                    else
                                    {
                                        WriteUInt32Bin(position, value);
                                    }
                                }
        void                    WriteUChar(unsigned char value, OGCSC3DMCStreamType streamType)
                                {
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        WriteUCharASCII(value);
                                    }
                                    else
                                    {
                                        WriteUChar8Bin(value);
                                    }
                                }
        float                   ReadFloat32(unsigned long & position, OGCSC3DMCStreamType streamType) const
                                {
                                    float value;
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        value = ReadFloat32ASCII(position);
                                    }
                                    else
                                    {
                                        value = ReadFloat32Bin(position);
                                    }
                                    return value;
                                }
        unsigned long           ReadUInt32(unsigned long & position, OGCSC3DMCStreamType streamType) const
                                {
                                    unsigned long value;
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        value = ReadUInt32ASCII(position);
                                    }
                                    else
                                    {
                                        value = ReadUInt32Bin(position);
                                    }
                                    return value;
                                }
        unsigned char           ReadUChar(unsigned long & position, OGCSC3DMCStreamType streamType) const
                                {
                                    unsigned char value;
                                    if (streamType == OGC_SC3DMC_STREAM_TYPE_ASCII)
                                    {
                                        value = ReadUCharASCII(position);
                                    }
                                    else
                                    {
                                        value = ReadUChar8Bin(position);
                                    }
                                    return value;
                                }

        void                    WriteFloat32Bin(unsigned long position, float value) 
                                {
                                    assert(position < m_stream.GetSize() - 4);
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == OGC_BIG_ENDIAN)
                                    {
                                        m_stream[position++] = ptr[3];
                                        m_stream[position++] = ptr[2];
                                        m_stream[position++] = ptr[1];
                                        m_stream[position  ] = ptr[0];
                                    }
                                    else
                                    {
                                        m_stream[position++] = ptr[0];
                                        m_stream[position++] = ptr[1];
                                        m_stream[position++] = ptr[2];
                                        m_stream[position  ] = ptr[3];
                                    }
                                }
        void                    WriteFloat32Bin(float value) 
                                {
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == OGC_BIG_ENDIAN)
                                    {
                                        m_stream.PushBack(ptr[3]);
                                        m_stream.PushBack(ptr[2]);
                                        m_stream.PushBack(ptr[1]);
                                        m_stream.PushBack(ptr[0]);
                                    }
                                    else
                                    {
                                        m_stream.PushBack(ptr[0]);
                                        m_stream.PushBack(ptr[1]);
                                        m_stream.PushBack(ptr[2]);
                                        m_stream.PushBack(ptr[3]);
                                    }
                                }
        void                    WriteUInt32Bin(unsigned long position, unsigned long value) 
                                {
                                    assert(position < m_stream.GetSize() - 4);
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == OGC_BIG_ENDIAN)
                                    {
                                        m_stream[position++] = ptr[3];
                                        m_stream[position++] = ptr[2];
                                        m_stream[position++] = ptr[1];
                                        m_stream[position  ] = ptr[0];
                                    }
                                    else
                                    {
                                        m_stream[position++] = ptr[0];
                                        m_stream[position++] = ptr[1];
                                        m_stream[position++] = ptr[2];
                                        m_stream[position  ] = ptr[3];
                                    }
                                }
        void                    WriteUInt32Bin(unsigned long value) 
                                {
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == OGC_BIG_ENDIAN)
                                    {
                                        m_stream.PushBack(ptr[3]);
                                        m_stream.PushBack(ptr[2]);
                                        m_stream.PushBack(ptr[1]);
                                        m_stream.PushBack(ptr[0]);
                                    }
                                    else
                                    {
                                        m_stream.PushBack(ptr[0]);
                                        m_stream.PushBack(ptr[1]);
                                        m_stream.PushBack(ptr[2]);
                                        m_stream.PushBack(ptr[3]);
                                    }
                                }
        void                    WriteUChar8Bin(unsigned int position, unsigned char value) 
                                {
                                    m_stream[position] = value;
                                }
        void                    WriteUChar8Bin(unsigned char value) 
                                {
                                    m_stream.PushBack(value);
                                }
        float                   ReadFloat32Bin(unsigned long & position) const
                                {
                                    unsigned long value = ReadUInt32Bin(position);
                                    float fvalue = *((float *)(&value));
                                    return fvalue;
                                }
        unsigned long           ReadUInt32Bin(unsigned long & position)  const
                                {
                                    assert(position < m_stream.GetSize() - 4);
                                    unsigned long value = 0;
                                    if (m_endianness == OGC_BIG_ENDIAN)
                                    {
                                        value += (m_stream[position++]<<24);
                                        value += (m_stream[position++]<<16);
                                        value += (m_stream[position++]<<8);
                                        value += (m_stream[position++]);
                                    }
                                    else
                                    {
                                        value += (m_stream[position++]);
                                        value += (m_stream[position++]<<8);
                                        value += (m_stream[position++]<<16);
                                        value += (m_stream[position++]<<24);
                                    }
                                    return value;
                                }
        unsigned char           ReadUChar8Bin(unsigned long & position) const
                                {
                                    return m_stream[position++];
                                }


        void                    WriteFloat32ASCII(float value) 
                                {
                                    unsigned long uiValue = *((unsigned long *)(&value));
                                    WriteUInt32ASCII(uiValue);
                                }
        void                    WriteUInt32ASCII(unsigned long position, unsigned long value) 
                                {
                                    assert(position < m_stream.GetSize() - OGC_BINARY_STREAM_NUM_SYMBOLS_UINT32);
                                    for(unsigned long i = 0; i < OGC_BINARY_STREAM_NUM_SYMBOLS_UINT32; ++i)
                                    {
                                        m_stream[position++] = value & OGC_BINARY_STREAM_MAX_SYMBOL0;
                                        value >>= OGC_BINARY_STREAM_BITS_PER_SYMBOL0;
                                    }
                                }
        void                    WriteUInt32ASCII(unsigned long value) 
                                {
                                    for(long i = 0; i < OGC_BINARY_STREAM_NUM_SYMBOLS_UINT32; ++i)
                                    {
                                        m_stream.PushBack(value & OGC_BINARY_STREAM_MAX_SYMBOL0);
                                        value >>= OGC_BINARY_STREAM_BITS_PER_SYMBOL0;
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
                                    if (value >= OGC_BINARY_STREAM_MAX_SYMBOL0)
                                    {
                                        m_stream.PushBack(OGC_BINARY_STREAM_MAX_SYMBOL0);
                                        value -= OGC_BINARY_STREAM_MAX_SYMBOL0;
                                        unsigned char a, b;
                                        do
                                        {
                                            a  = ((value & OGC_BINARY_STREAM_MAX_SYMBOL1) << 1);
                                            b  = ( (value >>= OGC_BINARY_STREAM_BITS_PER_SYMBOL1) > 0);
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
                                    assert(value <= OGC_BINARY_STREAM_MAX_SYMBOL0);
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
                                    assert(position < m_stream.GetSize() - OGC_BINARY_STREAM_NUM_SYMBOLS_UINT32);
                                    unsigned long value = 0;
                                    unsigned long shift = 0;
                                    for(long i = 0; i < OGC_BINARY_STREAM_NUM_SYMBOLS_UINT32; ++i)
                                    {
                                        value  += (m_stream[position++] << shift);
                                        shift  += OGC_BINARY_STREAM_BITS_PER_SYMBOL0;
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
                                    if (value == OGC_BINARY_STREAM_MAX_SYMBOL0)
                                    {
                                        long x;
                                        unsigned long i = 0;
                                        do
                                        {
                                            x = m_stream[position++];
                                            value += ( (x>>1) << i);
                                            i += OGC_BINARY_STREAM_BITS_PER_SYMBOL1;
                                        } while (x & 1);
                                    }
                                    return value;
                                }
        unsigned char           ReadUCharASCII(unsigned long & position) const
                                {
                                    return m_stream[position++];
                                }
        OGCErrorCode            Save(const char * const fileName) 
                                {
                                    FILE * fout = fopen(fileName, "wb");
                                    if (!fout)
                                    {
                                        return OGC_ERROR_CREATE_FILE;
                                    }
                                    fwrite(m_stream.GetBuffer(), 1, m_stream.GetSize(), fout);
                                    fclose(fout);
                                    return OGC_OK;
                                }
        OGCErrorCode            Load(const char * const fileName) 
                                {
                                    FILE * fin = fopen(fileName, "rb");
                                    if (!fin)
                                    {
                                        return OGC_ERROR_OPEN_FILE;
                                    }
                                    fseek(fin, 0, SEEK_END);
                                    unsigned long size = ftell(fin);
                                    m_stream.Allocate(size);
                                    rewind(fin);
                                    unsigned int nread = fread((void *) m_stream.GetBuffer(), 1, size, fin);
                                    m_stream.SetSize(size);
                                    if (nread != size)
                                    {
                                        return OGC_ERROR_READ_FILE;
                                    }
                                    fclose(fin);
                                    return OGC_OK;
                                }
        size_t                  GetSize() const
                                {
                                    return m_stream.GetSize();
                                }
    const unsigned char * const GetBuffer(unsigned long position) const
                                {
                                    return m_stream.GetBuffer() + position;
                                }
                                
    void                        GetBuffer(unsigned long position, unsigned char * & buffer) const
                                {
                                    buffer = (unsigned char *) (m_stream.GetBuffer() + position); // fix me: ugly!
                                }

    private:
        Vector<unsigned char>   m_stream;
        OGCEndianness           m_endianness;
    };

}
#endif // OGC_BINARY_STREAM_H

