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
#include "x3dgc_ArithmeticCodec.h"

//#define DEBUG_VERBOSE

namespace x3dgc
{
#ifdef DEBUG_VERBOSE
        FILE* g_fileDebugTF = NULL;
#endif //DEBUG_VERBOSE

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
    X3DGCErrorCode    CompressedTriangleFans::SaveUIntAC(const Vector<long> & data,
                                                         const unsigned long M,
                                                         BinaryStream & bstream) 
    {
        unsigned long start = bstream.GetSize();     
        const unsigned int NMAX = data.GetSize() * 8 + 100;
        const size_t size       = data.GetSize();
        long minValue = X3DGC_MAX_LONG;
        bstream.WriteUInt32Bin(0);
        bstream.WriteUInt32Bin(size);
        if (size > 0)
        {
    #ifdef DEBUG_VERBOSE
            printf("-----------\nsize %i, start %i\n", size, start);
            fprintf(g_fileDebugTF, "-----------\nsize %i, start %i\n", size, start);
    #endif //DEBUG_VERBOSE

            for(size_t i = 0; i < size; ++i)
            {
                if (minValue > data[i]) 
                {
                    minValue = data[i];
                }
    #ifdef DEBUG_VERBOSE
                printf("%i\t%i\n", i, data[i]);
                fprintf(g_fileDebugTF, "%i\t%i\n", i, data[i]);
    #endif //DEBUG_VERBOSE
            }
            bstream.WriteUInt32Bin(minValue);
            if ( m_sizeBufferAC < NMAX )
            {
                delete [] m_bufferAC;
                m_sizeBufferAC = NMAX;
                m_bufferAC     = new unsigned char [m_sizeBufferAC];
            }
            Arithmetic_Codec ace;
            ace.set_buffer(NMAX, m_bufferAC);
            ace.start_encoder();
            Adaptive_Data_Model mModelValues(M+1);
            for(size_t i = 0; i < size; ++i)
            {
                ace.encode(data[i]-minValue, mModelValues);
            }
            unsigned long encodedBytes = ace.stop_encoder();
            for(size_t i = 0; i < encodedBytes; ++i)
            {
                bstream.WriteUChar8Bin(m_bufferAC[i]);
            }
        }
        bstream.WriteUInt32Bin(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode    CompressedTriangleFans::SaveBinAC(const Vector<long> & data,
                                                         BinaryStream & bstream) 
    {
        unsigned long start = bstream.GetSize();     
        const unsigned int NMAX = data.GetSize() * 8 + 100;
        const size_t size       = data.GetSize();
        bstream.WriteUInt32Bin(0);
        bstream.WriteUInt32Bin(size);
        if (size > 0)
        {
            if ( m_sizeBufferAC < NMAX )
            {
                delete [] m_bufferAC;
                m_sizeBufferAC = NMAX;
                m_bufferAC     = new unsigned char [m_sizeBufferAC];
            }
            Arithmetic_Codec ace;
            ace.set_buffer(NMAX, m_bufferAC);
            ace.start_encoder();
            Adaptive_Bit_Model bModel;
    #ifdef DEBUG_VERBOSE
            printf("-----------\nsize %i, start %i\n", size, start);
            fprintf(g_fileDebugTF, "-----------\nsize %i, start %i\n", size, start);
    #endif //DEBUG_VERBOSE
            for(size_t i = 0; i < size; ++i)
            {
                ace.encode(data[i], bModel);
    #ifdef DEBUG_VERBOSE
                printf("%i\t%i\n", i, data[i]);
                fprintf(g_fileDebugTF, "%i\t%i\n", i, data[i]);
    #endif //DEBUG_VERBOSE
            }
            unsigned long encodedBytes = ace.stop_encoder();
            for(size_t i = 0; i < encodedBytes; ++i)
            {
                bstream.WriteUChar8Bin(m_bufferAC[i]);
            }
        }
        bstream.WriteUInt32Bin(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }

    X3DGCErrorCode    CompressedTriangleFans::SaveIntACEGC(const Vector<long> & data,
                                                            const unsigned long M,
                                                            BinaryStream & bstream) 
    {
        unsigned long start = bstream.GetSize();     
        const unsigned int NMAX = data.GetSize() * 8 + 100;
        const size_t size       = data.GetSize();
        long minValue = 0;
        bstream.WriteUInt32Bin(0);
        bstream.WriteUInt32Bin(size);
        if (size > 0)
        {
#ifdef DEBUG_VERBOSE
            printf("-----------\nsize %i, start %i\n", size, start);
            fprintf(g_fileDebugTF, "-----------\nsize %i, start %i\n", size, start);
#endif //DEBUG_VERBOSE
            for(size_t i = 0; i < size; ++i)
            {
                if (minValue > data[i]) 
                {
                    minValue = data[i];
                }
#ifdef DEBUG_VERBOSE
                printf("%i\t%i\n", i, data[i]);
                fprintf(g_fileDebugTF, "%i\t%i\n", i, data[i]);
#endif //DEBUG_VERBOSE
            }
            bstream.WriteUInt32Bin(minValue + X3DGC_MAX_LONG);
            if ( m_sizeBufferAC < NMAX )
            {
                delete [] m_bufferAC;
                m_sizeBufferAC = NMAX;
                m_bufferAC     = new unsigned char [m_sizeBufferAC];
            }
            Arithmetic_Codec ace;
            ace.set_buffer(NMAX, m_bufferAC);
            ace.start_encoder();
            Adaptive_Data_Model mModelValues(M+2);
            Static_Bit_Model bModel0;
            Adaptive_Bit_Model bModel1;
            unsigned long value;
            for(size_t i = 0; i < size; ++i)
            {
                value = data[i]-minValue;
                if (value < M) 
                {
                    ace.encode(value, mModelValues);
                }
                else 
                {
                    ace.encode(M, mModelValues);
                    ace.ExpGolombEncode(value-M, 0, bModel0, bModel1);
                }
            }
            unsigned long encodedBytes = ace.stop_encoder();
            for(size_t i = 0; i < encodedBytes; ++i)
            {
                bstream.WriteUChar8Bin(m_bufferAC[i]);
            }
        }
        bstream.WriteUInt32Bin(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode    CompressedTriangleFans::Save(BinaryStream & bstream, X3DGCSC3DMCStreamType streamType) 
    {
#ifdef DEBUG_VERBOSE
        g_fileDebugTF = fopen("SaveIntACEGC_new.txt", "w");
#endif //DEBUG_VERBOSE

        if (streamType == X3DGC_SC3DMC_STREAM_TYPE_ASCII)
        {
            SaveUIntData(m_numTFANs  , bstream);
            SaveUIntData(m_degrees   , bstream);
            SaveUIntData(m_configs   , bstream);
            SaveBinData (m_operations, bstream);
            SaveIntData (m_indices   , bstream);
        }
        else
        {
            SaveIntACEGC(m_numTFANs  , 4 , bstream);
            SaveIntACEGC(m_degrees   , 16, bstream);
            SaveUIntAC  (m_configs   , 10, bstream);
            SaveBinAC   (m_operations,     bstream);
            SaveIntACEGC(m_indices   , 8 , bstream);
        }
#ifdef DEBUG_VERBOSE
        fclose(g_fileDebugTF);
#endif //DEBUG_VERBOSE
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
    X3DGCErrorCode    LoadUIntAC(Vector<long> & data,
                                 const unsigned long M,
                                 const BinaryStream & bstream,
                                 unsigned long & iterator) 
    {
        size_t sizeSize = bstream.ReadUInt32Bin(iterator) - 12;
        size_t size     = bstream.ReadUInt32Bin(iterator);
        if (size == 0)
        {
            return X3DGC_OK;
        }
        long minValue   = bstream.ReadUInt32Bin(iterator);
        unsigned char * buffer = 0;
        bstream.GetBuffer(iterator, buffer);
        iterator += sizeSize;
        data.Allocate(size);
        Arithmetic_Codec acd;
        acd.set_buffer(sizeSize, buffer);
        acd.start_decoder();
        Adaptive_Data_Model mModelValues(M+1);
#ifdef DEBUG_VERBOSE
        printf("-----------\nsize %i\n", size);
        fprintf(g_fileDebugTF, "size %i\n", size);
#endif //DEBUG_VERBOSE
        for(size_t i = 0; i < size; ++i)
        {
            data.PushBack(acd.decode(mModelValues)+minValue);
#ifdef DEBUG_VERBOSE
            printf("%i\t%i\n", i, data[i]);
            fprintf(g_fileDebugTF, "%i\t%i\n", i, data[i]);
#endif //DEBUG_VERBOSE
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode    LoadIntACEGC(Vector<long> & data,
                                   const unsigned long M,
                                   const BinaryStream & bstream,
                                   unsigned long & iterator) 
    {
        size_t sizeSize = bstream.ReadUInt32Bin(iterator) - 12;
        size_t size     = bstream.ReadUInt32Bin(iterator);
        if (size == 0)
        {
            return X3DGC_OK;
        }
        long minValue   = bstream.ReadUInt32Bin(iterator) - X3DGC_MAX_LONG;
        unsigned char * buffer = 0;
        bstream.GetBuffer(iterator, buffer);
        iterator += sizeSize;
        data.Allocate(size);
        Arithmetic_Codec acd;
        acd.set_buffer(sizeSize, buffer);
        acd.start_decoder();
        Adaptive_Data_Model mModelValues(M+2);
        Static_Bit_Model bModel0;
        Adaptive_Bit_Model bModel1;
        unsigned long value;

#ifdef DEBUG_VERBOSE
        printf("-----------\nsize %i\n", size);
        fprintf(g_fileDebugTF, "size %i\n", size);
#endif //DEBUG_VERBOSE
        for(size_t i = 0; i < size; ++i)
        {
            value = acd.decode(mModelValues);
            if ( value == M)
            {
                value += acd.ExpGolombDecode(0, bModel0, bModel1);
            }
            data.PushBack(value + minValue);
#ifdef DEBUG_VERBOSE
            printf("%i\t%i\n", i, data[i]);
            fprintf(g_fileDebugTF, "%i\t%i\n", i, data[i]);
#endif //DEBUG_VERBOSE
        }
#ifdef DEBUG_VERBOSE
        fflush(g_fileDebugTF);
#endif //DEBUG_VERBOSE
        return X3DGC_OK;
    }
    X3DGCErrorCode    LoadBinAC(Vector<long> & data,
                                const BinaryStream & bstream,
                                unsigned long & iterator) 
    {
        size_t sizeSize = bstream.ReadUInt32Bin(iterator) - 8;
        size_t size     = bstream.ReadUInt32Bin(iterator);
        if (size == 0)
        {
            return X3DGC_OK;
        }
        unsigned char * buffer = 0;
        bstream.GetBuffer(iterator, buffer);
        iterator += sizeSize;
        data.Allocate(size);
        Arithmetic_Codec acd;
        acd.set_buffer(sizeSize, buffer);
        acd.start_decoder();
        Adaptive_Bit_Model bModel;
#ifdef DEBUG_VERBOSE
        printf("-----------\nsize %i\n", size);
        fprintf(g_fileDebugTF, "size %i\n", size);
#endif //DEBUG_VERBOSE
        for(size_t i = 0; i < size; ++i)
        {
            data.PushBack(acd.decode(bModel));
#ifdef DEBUG_VERBOSE
            printf("%i\t%i\n", i, data[i]);
            fprintf(g_fileDebugTF, "%i\t%i\n", i, data[i]);
#endif //DEBUG_VERBOSE
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode    CompressedTriangleFans::Load(const BinaryStream & bstream, 
                                                   unsigned long & iterator, 
                                                   X3DGCSC3DMCStreamType streamType) 
    {
#ifdef DEBUG_VERBOSE
        g_fileDebugTF = fopen("Load_new.txt", "w");
#endif //DEBUG_VERBOSE
        if (streamType == X3DGC_SC3DMC_STREAM_TYPE_ASCII)
        {
            LoadUIntData(m_numTFANs  , bstream, iterator);
            LoadUIntData(m_degrees   , bstream, iterator);
            LoadUIntData(m_configs   , bstream, iterator);
            LoadBinData (m_operations, bstream, iterator);
            LoadIntData (m_indices   , bstream, iterator);
        }
        else
        {
            LoadIntACEGC(m_numTFANs  , 4 , bstream, iterator);
            LoadIntACEGC(m_degrees   , 16, bstream, iterator);
            LoadUIntAC  (m_configs   , 10, bstream, iterator);
            LoadBinAC   (m_operations,     bstream, iterator);
            LoadIntACEGC(m_indices   , 8 , bstream, iterator);
        }

#ifdef DEBUG_VERBOSE
        fclose(g_fileDebugTF);
#endif //DEBUG_VERBOSE
        return X3DGC_OK;
    }
}

