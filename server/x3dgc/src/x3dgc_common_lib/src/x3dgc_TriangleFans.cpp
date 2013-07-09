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

//#define DEBUG_VERBOSE

#ifdef DEBUG_VERBOSE
#include <map>
#endif //DEBUG_VERBOSE

namespace x3dgc
{
#ifdef DEBUG_VERBOSE
        void Entropy(const std::map<long, long> &freq, long numSymbols)
        {
            double optimalSize = 0;
            double pi=0.0;
            double  x = 0.0;
            double n = 0.0;
            double entropy = 0.0;        
//            printf("\t --------------------------------\n");
            for(std::map<long, long>::const_iterator it = freq.begin(); it != freq.end(); it++)
            {
                n = it->second;
                pi = n/numSymbols;
                if (pi > 0.0)
                {
                    entropy += pi * (-log(pi)/log(2.0));
                }
//                printf("\t %i -> %i\n", it->first, it->second);
            }
            optimalSize = numSymbols * entropy / 8.0;
            printf("\t Stream size = %f\n", optimalSize);
//            printf("\t --------------------------------\n");
        }
#endif //DEBUG_VERBOSE
    X3DGCErrorCode    SaveData(const Vector<long> & data,
                             const unsigned long symbolSize, 
                             BinaryStream & bstream) 
    {
        assert(symbolSize <= 8);
        
        unsigned long start = bstream.GetSize();
        bstream.WriteUInt32(0);
        const size_t        size            = data.GetSize();
        const unsigned long symbolsPerGroup = 8/symbolSize;
        long symbol;

#ifdef DEBUG_VERBOSE
        std::map<long, long> freq;
        std::map<long, long> freqGrouped;
        long numSymbols = 0;
        long numGroupedSymbols = 0;
#endif //DEBUG_VERBOSE
        for(size_t i = 0; i < size; )
        {
            symbol = 0;
            for(unsigned long h = 0; h < symbolsPerGroup && i < size; ++h)
            {
                symbol += (data[i] << (symbolSize*h));
#ifdef DEBUG_VERBOSE
                freq[data[i]]++;
                ++numSymbols;
#endif //DEBUG_VERBOSE
                ++i;
            }
            bstream.WriteUChar8((unsigned char) symbol);
#ifdef DEBUG_VERBOSE
            freqGrouped[symbol]++;
            ++numGroupedSymbols;
#endif //DEBUG_VERBOSE
        }

#ifdef DEBUG_VERBOSE
        printf("Individual symbols\n");
        Entropy(freq, numSymbols);
//        printf("Grouped symbols\n");
//        Entropy(freqGrouped, numGroupedSymbols);
#endif //DEBUG_VERBOSE
        bstream.WriteUInt32(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode    CompressedTriangleFans::Save(BinaryStream & bstream) const
    {
        SaveData(m_numTFANs,   X3DGC_NUM_TFANS_SIZE_BITS, bstream);
        SaveData(m_degrees,    X3DGC_TFAN_DEGREE_BITS   , bstream);
        SaveData(m_configs,    4                        , bstream);
        SaveData(m_operations, 1                        , bstream);
        SaveData(m_indices,    8                        , bstream);
        return X3DGC_OK;
    }
    X3DGCErrorCode    LoadData(Vector<long> & data,
                             const unsigned long symbolSize, 
                             const BinaryStream & bstream,
                             unsigned long & iterator) 
    {
        assert(symbolSize <= 8);
        const unsigned long size = bstream.ReadUInt32(iterator) - 4;
        const unsigned long symbolsPerGroup = 8/symbolSize;
        const unsigned long mask = (1 << symbolSize) - 1;
        long symbol;
        data.Allocate(size * symbolsPerGroup);
        data.Clear();
        for(size_t i = 0; i < size; ++i)
        {
            symbol = bstream.ReadUChar8(iterator);
            for(unsigned long h = 0; h < symbolsPerGroup; ++h)
            {
                data.PushBack(symbol & mask);
                symbol >>= symbolSize;
            }
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode    CompressedTriangleFans::Load(const BinaryStream & bstream, unsigned long & iterator) 
    {
        LoadData(m_numTFANs,   X3DGC_NUM_TFANS_SIZE_BITS, bstream, iterator);
        LoadData(m_degrees,    X3DGC_TFAN_DEGREE_BITS   , bstream, iterator);
        LoadData(m_configs,    4                        , bstream, iterator);
        LoadData(m_operations, 1                        , bstream, iterator);
        LoadData(m_indices,    8                        , bstream, iterator);
        return X3DGC_OK;
    }
}

