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
    const unsigned long X3DGC_BINARY_STREAM_DEFAULT_SIZE = 4096;
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
        void                    WriteFloat32(unsigned long position, float value) 
                                {
                                    assert(position < m_stream.GetSize() - 4);
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == X3DGC_BIG_ENDIAN)
                                    {
                                        m_stream[position++] = ptr[3];
                                        m_stream[position++] = ptr[2];
                                        m_stream[position++] = ptr[1];
                                        m_stream[position  ] = ptr[0];
                                    }
                                    else
                                    {
                                        m_stream[position++] = (*(ptr+0));
                                        m_stream[position++] = (*(ptr+1));
                                        m_stream[position++] = (*(ptr+2));
                                        m_stream[position  ] = (*(ptr+3));
                                    }
                                }

        void                    WriteFloat32(float value) 
                                {
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == X3DGC_BIG_ENDIAN)
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
        void                    WriteUInt32(unsigned long position, unsigned long value) 
                                {
                                    assert(position < m_stream.GetSize() - 4);
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == X3DGC_BIG_ENDIAN)
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

        void                    WriteUInt32(unsigned long value) 
                                {
                                    unsigned char * ptr = (unsigned char *) (&value);
                                    if (m_endianness == X3DGC_BIG_ENDIAN)
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
        void                    WriteUChar8(unsigned int position, unsigned char value) 
                                {
                                    m_stream[position] = value;
                                }
        void                    WriteUChar8(unsigned char value) 
                                {
                                    m_stream.PushBack(value);
                                }
        float                   ReadFloat32(unsigned long & position) const
                                {
                                    unsigned long value = ReadUInt32(position);
                                    float fvalue = *((float *)(&value));
                                    return fvalue;
                                }
        unsigned long           ReadUInt32(unsigned long & position)  const
                                {
                                    assert(position < m_stream.GetSize() - 4);
                                    unsigned long value = 0;
                                    if (m_endianness == X3DGC_BIG_ENDIAN)
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
        unsigned char           ReadUChar8(unsigned long & position) const
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
        X3DGCErrorCode            Load(const char * const fileName) 
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

