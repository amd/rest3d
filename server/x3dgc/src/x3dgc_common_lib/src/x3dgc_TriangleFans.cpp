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

#include "x3dgc_TriangleFans.h"

namespace x3dgc
{
    X3DGCErrorCode    SaveUIntData(const Vector<long> & data,
                             BinaryStream & bstream) 
    {
        unsigned long start = bstream.GetSize();
        bstream.WriteUInt32ASCII(0);
        const size_t size       = data.GetSize();
        bstream.WriteUInt32ASCII(size);
        for(size_t i = 0; i < size; ++i)
        {
            bstream.WriteUIntASCII(data[i]);
        }
        bstream.WriteUInt32ASCII(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode    SaveIntData(const Vector<long> & data,
                             BinaryStream & bstream) 
    {
        unsigned long start = bstream.GetSize();
        bstream.WriteUInt32ASCII(0);
        const size_t size       = data.GetSize();
        bstream.WriteUInt32ASCII(size);
        for(size_t i = 0; i < size; ++i)
        {
            bstream.WriteIntASCII(data[i]);
        }
        bstream.WriteUInt32ASCII(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode    SaveBinData(const Vector<long> & data,
                                  BinaryStream & bstream) 
    {
        unsigned long start = bstream.GetSize();
        bstream.WriteUInt32ASCII(0);
        const size_t size = data.GetSize();
        long symbol;
        bstream.WriteUInt32ASCII(size);
        for(size_t i = 0; i < size; )
        {
            symbol = 0;
            for(unsigned long h = 0; h < X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0 && i < size; ++h)
            {
                symbol += (data[i] << h);
                ++i;
            }
            bstream.WriteUCharASCII((unsigned char) symbol);
        }
        bstream.WriteUInt32ASCII(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode    CompressedTriangleFans::Save(BinaryStream & bstream) const
    {
        SaveUIntData(m_numTFANs,   bstream);
        SaveUIntData(m_degrees,    bstream);
        SaveUIntData(m_configs,    bstream);
        SaveBinData(m_operations,  bstream);
        SaveIntData(m_indices,     bstream);
        return X3DGC_OK;
    }

    X3DGCErrorCode    LoadUIntData(Vector<long> & data,
                                  const BinaryStream & bstream,
                                  unsigned long & iterator) 
    {
        bstream.ReadUInt32ASCII(iterator);
        const unsigned long size = bstream.ReadUInt32ASCII(iterator);
        data.Allocate(size);
        data.Clear();
        for(size_t i = 0; i < size; ++i)
        {
            data.PushBack(bstream.ReadUIntASCII(iterator));
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode    LoadIntData(Vector<long> & data,
                                  const BinaryStream & bstream,
                                  unsigned long & iterator) 
    {
        bstream.ReadUInt32ASCII(iterator);
        const unsigned long size = bstream.ReadUInt32ASCII(iterator);
        data.Allocate(size);
        data.Clear();
        for(size_t i = 0; i < size; ++i)
        {
            data.PushBack(bstream.ReadIntASCII(iterator));
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode    LoadBinData(Vector<long> & data,
                                  const BinaryStream & bstream,
                                  unsigned long & iterator) 
    {
        bstream.ReadUInt32ASCII(iterator);
        const unsigned long size = bstream.ReadUInt32ASCII(iterator);
        long symbol;
        data.Allocate(size * X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0);
        data.Clear();
        for(size_t i = 0; i < size;)
        {
            symbol = bstream.ReadUCharASCII(iterator);
            for(unsigned long h = 0; h < X3DGC_BINARY_STREAM_BITS_PER_SYMBOL0; ++h)
            {
                data.PushBack(symbol & 1);
                symbol >>= 1;
                ++i;
            }
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode    CompressedTriangleFans::Load(const BinaryStream & bstream, unsigned long & iterator) 
    {
        LoadUIntData(m_numTFANs,   bstream, iterator);
        LoadUIntData(m_degrees,    bstream, iterator);
        LoadUIntData(m_configs,    bstream, iterator);
        LoadBinData(m_operations, bstream, iterator);
        LoadIntData(m_indices,    bstream, iterator);
        return X3DGC_OK;
    }
}

