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

#include "o3dgcSC3DMCDecoder.h"
#include "o3dgcArithmeticCodec.h"

//#define DEBUG_VERBOSE

namespace o3dgc
{
#ifdef DEBUG_VERBOSE
        FILE* g_fileDebugSC3DMCDec = NULL;
#endif //DEBUG_VERBOSE

    O3DGCErrorCode SC3DMCDecoder::DecodeHeader(IndexedFaceSet & ifs, 
                                               const BinaryStream & bstream)
    {
        unsigned long iterator0 = m_iterator;
        unsigned long start_code = bstream.ReadUInt32(m_iterator, O3DGC_SC3DMC_STREAM_TYPE_BINARY);
        if (start_code != O3DGC_SC3DMC_START_CODE)
        {
            m_iterator = iterator0;
            start_code = bstream.ReadUInt32(m_iterator, O3DGC_SC3DMC_STREAM_TYPE_ASCII);
            if (start_code != O3DGC_SC3DMC_START_CODE)
            {
                return O3DGC_ERROR_CORRUPTED_STREAM;
            }
            else
            {
                m_streamType = O3DGC_SC3DMC_STREAM_TYPE_ASCII;
            }
        }
        else
        {
            m_streamType = O3DGC_SC3DMC_STREAM_TYPE_BINARY;
        }
            
        m_streamSize = bstream.ReadUInt32(m_iterator, m_streamType); // to be filled later
        m_params.SetEncodeMode( (O3DGCSC3DMCEncodingMode) bstream.ReadUChar(m_iterator, m_streamType));

        ifs.SetCreaseAngle((Real) bstream.ReadFloat32(m_iterator, m_streamType));
          
        unsigned char mask = bstream.ReadUChar(m_iterator, m_streamType);

        ifs.SetCCW             ((mask & 1) == 1);
        ifs.SetSolid           ((mask & 2) == 1);
        ifs.SetConvex          ((mask & 4) == 1);
        ifs.SetIsTriangularMesh((mask & 8) == 1);
        //bool markerBit0 = (mask & 16 ) == 1;
        //bool markerBit1 = (mask & 32 ) == 1;
        //bool markerBit2 = (mask & 64 ) == 1;
        //bool markerBit3 = (mask & 128) == 1;
       
        ifs.SetNCoord         (bstream.ReadUInt32(m_iterator, m_streamType));
        ifs.SetNNormal        (bstream.ReadUInt32(m_iterator, m_streamType));
        ifs.SetNColor         (bstream.ReadUInt32(m_iterator, m_streamType));
        ifs.SetNTexCoord      (bstream.ReadUInt32(m_iterator, m_streamType));


        ifs.SetNumFloatAttributes(bstream.ReadUInt32(m_iterator, m_streamType));
        ifs.SetNumIntAttributes  (bstream.ReadUInt32(m_iterator, m_streamType));
                              
        if (ifs.GetNCoord() > 0)
        {
            ifs.SetNCoordIndex(bstream.ReadUInt32(m_iterator, m_streamType));
            for(int j=0 ; j<3 ; ++j)
            {
                ifs.SetCoordMin(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
                ifs.SetCoordMax(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
            }
            m_params.SetCoordQuantBits( bstream.ReadUChar(m_iterator, m_streamType) );
        }
        if (ifs.GetNNormal() > 0)
        {
            ifs.SetNNormalIndex(bstream.ReadUInt32(m_iterator, m_streamType));
            for(int j=0 ; j<3 ; ++j)
            {
                ifs.SetNormalMin(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
                ifs.SetNormalMax(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
            }
            ifs.SetNormalPerVertex(bstream.ReadUChar(m_iterator, m_streamType) == 1);
            m_params.SetNormalQuantBits(bstream.ReadUChar(m_iterator, m_streamType));
        }
        if (ifs.GetNColor() > 0)
        {
            ifs.SetNColorIndex(bstream.ReadUInt32(m_iterator, m_streamType));
            for(int j=0 ; j<3 ; ++j)
            {
                ifs.SetColorMin(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
                ifs.SetColorMax(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
            }
            ifs.SetColorPerVertex(bstream.ReadUChar(m_iterator, m_streamType)==1);
            m_params.SetColorQuantBits(bstream.ReadUChar(m_iterator, m_streamType));
        }
        if (ifs.GetNTexCoord() > 0)
        {
            ifs.SetNTexCoordIndex(bstream.ReadUInt32(m_iterator, m_streamType));
            for(int j=0 ; j<2 ; ++j)
            {
                ifs.SetTexCoordMin(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
                ifs.SetTexCoordMax(j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
            }
            m_params.SetTexCoordQuantBits(bstream.ReadUChar(m_iterator, m_streamType));
        }

        for(unsigned long a = 0; a < ifs.GetNumFloatAttributes(); ++a)
        {
            ifs.SetNFloatAttribute(a, bstream.ReadUInt32(m_iterator, m_streamType));    
            if (ifs.GetNFloatAttribute(a) > 0)
            {
                ifs.SetNFloatAttributeIndex(a, bstream.ReadUInt32(m_iterator, m_streamType));
                unsigned char d = bstream.ReadUChar(m_iterator, m_streamType);
                ifs.SetFloatAttributeDim(a, d);
                for(unsigned char j = 0 ; j < d ; ++j)
                {
                    ifs.SetFloatAttributeMin(a, j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
                    ifs.SetFloatAttributeMax(a, j, (Real) bstream.ReadFloat32(m_iterator, m_streamType));
                }
                ifs.SetFloatAttributePerVertex(a, bstream.ReadUChar(m_iterator, m_streamType) == 1);
                m_params.SetFloatAttributeQuantBits(a, bstream.ReadUChar(m_iterator, m_streamType));
            }
        }
        for(unsigned long a = 0; a < ifs.GetNumIntAttributes(); ++a)
        {
            ifs.SetNIntAttribute(a, bstream.ReadUInt32(m_iterator, m_streamType));
            if (ifs.GetNIntAttribute(a) > 0)
            {
                ifs.SetNIntAttributeIndex(a, bstream.ReadUInt32(m_iterator, m_streamType));
                ifs.SetIntAttributeDim(a, bstream.ReadUChar(m_iterator, m_streamType));
                ifs.SetIntAttributePerVertex(a, bstream.ReadUChar(m_iterator, m_streamType) == 1);
            }
        }    
        return O3DGC_OK;
    }

    O3DGCErrorCode SC3DMCDecoder::DecodePlayload(IndexedFaceSet & ifs,
                                                 const BinaryStream & bstream)
    {
#ifdef DEBUG_VERBOSE
        g_fileDebugSC3DMCDec = fopen("tfans_dec_main.txt", "w");
#endif //DEBUG_VERBOSE

        m_triangleListDecoder.SetStreamType(m_streamType);
        m_triangleListDecoder.Decode(ifs.GetCoordIndex(), ifs.GetNCoordIndex(), ifs.GetNCoord(), bstream, m_iterator);

        // Decode coord
        if (ifs.GetNCoord() > 0)
        {
            DecodeFloatArray(ifs.GetCoord(), ifs.GetNCoord(), 3, ifs.GetCoordMin(), ifs.GetCoordMax(),
                             m_params.GetCoordQuantBits(), ifs, m_params.GetCoordPredMode(), bstream);
        }

        // encode Normal
        if (ifs.GetNNormal() > 0)
        {
            DecodeFloatArray(ifs.GetNormal(), ifs.GetNNormal(), 3, ifs.GetNormalMin(), ifs.GetNormalMax(),
                                m_params.GetNormalQuantBits(), ifs, m_params.GetNormalPredMode(), bstream);
        }
        // encode Color
        if (ifs.GetNColor() > 0)
        {
            DecodeFloatArray(ifs.GetColor(), ifs.GetNColor(), 3, ifs.GetColorMin(), ifs.GetColorMax(),
                                m_params.GetColorQuantBits(), ifs, m_params.GetColorPredMode(), bstream);
        }
        // encode TexCoord
        if (ifs.GetNTexCoord() > 0)
        {
            DecodeFloatArray(ifs.GetTexCoord(), ifs.GetNTexCoord(), 2, ifs.GetTexCoordMin(), ifs.GetTexCoordMax(), 
                                m_params.GetTexCoordQuantBits(), ifs, m_params.GetTexCoordPredMode(), bstream);
        }

        for(unsigned long a = 0; a < ifs.GetNumFloatAttributes(); ++a)
        {
            DecodeFloatArray(ifs.GetFloatAttribute(a), ifs.GetNFloatAttribute(a), ifs.GetFloatAttributeDim(a), ifs.GetFloatAttributeMin(a), ifs.GetFloatAttributeMax(a), 
                                m_params.GetFloatAttributeQuantBits(a), ifs, m_params.GetFloatAttributePredMode(a), bstream);
        }

        for(unsigned long a = 0; a < ifs.GetNumIntAttributes(); ++a)
        {
            DecodeIntArray(ifs.GetIntAttribute(a), ifs.GetNIntAttribute(a), ifs.GetIntAttributeDim(a), bstream);
        }
#ifdef DEBUG_VERBOSE
        fclose(g_fileDebugSC3DMCDec);
#endif //DEBUG_VERBOSE
        return O3DGC_OK;
    }
    inline long DecodeIntACEGC(Arithmetic_Codec & acd,
                               Adaptive_Data_Model & mModelValues,
                               Static_Bit_Model & bModel0,
                               Adaptive_Bit_Model & bModel1,
                               const unsigned long exp_k,
                               const unsigned long M)
    {
        unsigned long uiValue = acd.decode(mModelValues);
        if (uiValue == M) 
        {
            uiValue += acd.ExpGolombDecode(exp_k, bModel0, bModel1);
        }

        if (uiValue & 1)
        {
            return - ((long)(uiValue >> 1));
        }
        else
        {
            return  (long)(uiValue >> 1);
        }
    }
    inline unsigned long DecodeUIntACEGC(Arithmetic_Codec & acd,
                                         Adaptive_Data_Model & mModelValues,
                                         Static_Bit_Model & bModel0,
                                         Adaptive_Bit_Model & bModel1,
                                         const unsigned long exp_k,
                                         const unsigned long M)
    {
        unsigned long uiValue = acd.decode(mModelValues);
        if (uiValue == M) 
        {
            uiValue += acd.ExpGolombDecode(exp_k, bModel0, bModel1);
        }
        return uiValue;
    }
    O3DGCErrorCode SC3DMCDecoder::DecodeIntArray(long * const intArray, 
                                                 unsigned long numIntArray,
                                                 unsigned long dimIntArray,
                                                 const BinaryStream & bstream)
    {        
        const long nvert = (long) numIntArray;

        unsigned char * buffer = 0;
        Arithmetic_Codec acd;
        Static_Bit_Model bModel0;
        Adaptive_Bit_Model bModel1;
        unsigned long sizeSize = bstream.ReadUInt32(m_iterator, m_streamType) - 9;        // bitsream size
        unsigned char mask = bstream.ReadUChar(m_iterator, m_streamType);
        unsigned int exp_k;
        unsigned int M = 0;
        long minValue = bstream.ReadUInt32(m_iterator, m_streamType) - O3DGC_MAX_LONG;
        
        if (m_streamType != O3DGC_SC3DMC_STREAM_TYPE_ASCII)
        {
            bstream.GetBuffer(m_iterator, buffer);
            m_iterator += sizeSize;
            acd.set_buffer(sizeSize, buffer);
            acd.start_decoder();
            exp_k = acd.ExpGolombDecode(0, bModel0, bModel1);
            M     = acd.ExpGolombDecode(0, bModel0, bModel1);
        }
        Adaptive_Data_Model mModelValues(M+2);

#ifdef DEBUG_VERBOSE
        printf("IntArray (%i, %i)\n", numIntArray, dimIntArray);
        fprintf(g_fileDebugSC3DMCDec, "IntArray (%i, %i)\n", numIntArray, dimIntArray);
#endif //DEBUG_VERBOSE
        if (m_streamType == O3DGC_SC3DMC_STREAM_TYPE_ASCII)
        {
            for (long v=0; v < nvert; ++v) 
            {
                for (unsigned long i = 0; i < dimIntArray; i++) 
                {
                    intArray[v*dimIntArray+i] = bstream.ReadUIntASCII(m_iterator) + minValue;
#ifdef DEBUG_VERBOSE
                    printf("%i\n", intArray[v*dimIntArray+i]);
                    fprintf(g_fileDebugSC3DMCDec, "%i\n", intArray[v*dimIntArray+i]);
#endif //DEBUG_VERBOSE
                }
            }
        }
        else
        {
            for (long v=0; v < nvert; ++v) 
            {
                for (unsigned long i = 0; i < dimIntArray; i++) 
                {
                    intArray[v*dimIntArray+i] = DecodeUIntACEGC(acd, mModelValues, bModel0, bModel1, exp_k, M) + minValue;
#ifdef DEBUG_VERBOSE
                    printf("%i\n", intArray[v*dimIntArray+i]);
                    fprintf(g_fileDebugSC3DMCDec, "%i\n", intArray[v*dimIntArray+i]);
#endif //DEBUG_VERBOSE
                }
            }
        }
#ifdef DEBUG_VERBOSE
        fflush(g_fileDebugSC3DMCDec);
#endif //DEBUG_VERBOSE
        return O3DGC_OK;
    }
    O3DGCErrorCode SC3DMCDecoder::DecodeFloatArray(Real * const floatArray, 
                                                   unsigned long numFloatArray,
                                                   unsigned long dimFloatArray,
                                                   const Real * const minFloatArray,
                                                   const Real * const maxFloatArray,
                                                   unsigned long nQBits,
                                                   const IndexedFaceSet & ifs,
                                                   O3DGCSC3DMCPredictionMode predMode,
                                                   const BinaryStream & bstream)
    {
        assert(dimFloatArray <  O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES);
        const AdjacencyInfo & v2T    = m_triangleListDecoder.GetVertexToTriangle();
        const long * const triangles = ifs.GetCoordIndex();
        long vpred[O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        long tpred[O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        long nv, nt;
        long predResidual;
        const long nvert = (long) numFloatArray;
        const unsigned long size = numFloatArray * dimFloatArray;
        unsigned char * buffer = 0;
        Arithmetic_Codec acd;
        Static_Bit_Model bModel0;
        Adaptive_Bit_Model bModel1;
        unsigned long sizeSize = bstream.ReadUInt32(m_iterator, m_streamType) - 5;        // bitsream size
        unsigned char mask = bstream.ReadUChar(m_iterator, m_streamType);
        unsigned int exp_k;
        unsigned int M = 0;
        if (m_streamType != O3DGC_SC3DMC_STREAM_TYPE_ASCII)
        {
            bstream.GetBuffer(m_iterator, buffer);
            m_iterator += sizeSize;
            acd.set_buffer(sizeSize, buffer);
            acd.start_decoder();
            exp_k = acd.ExpGolombDecode(0, bModel0, bModel1);
            M     = acd.ExpGolombDecode(0, bModel0, bModel1);
        }
        Adaptive_Data_Model mModelValues(M+2);

#ifdef DEBUG_VERBOSE
        printf("FloatArray (%i, %i)\n", numFloatArray, dimFloatArray);
        fprintf(g_fileDebugSC3DMCDec, "FloatArray (%i, %i)\n", numFloatArray, dimFloatArray);
#endif //DEBUG_VERBOSE

        if (m_quantFloatArraySize < size)
        {
            delete [] m_quantFloatArray;
            m_quantFloatArraySize = size;
            m_quantFloatArray     = new long [size];
        }
        for (long v=0; v < nvert; ++v) 
        {
            nv = 0;
            nt = 0;
            if ( v2T.GetNumNeighbors(v) > 0 && 
                 predMode != O3DGC_SC3DMC_NO_PREDICTION)
            {
                for (unsigned long i = 0; i < dimFloatArray; i++) 
                {
                    vpred[i] = 0;
                    tpred[i] = 0;
                }
                int u0 = v2T.Begin(v);
                int u1 = v2T.End(v);
                for (long u = u0; u < u1; u++) 
                {
                    long ta = v2T.GetNeighbor(u);
                    if (ta < 0)
                    {
                        break;
                    }
                    if (predMode == O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION)
                    {
                        long a,b;
                        if (triangles[ta*3] == v)
                        {
                            a = triangles[ta*3 + 1];
                            b = triangles[ta*3 + 2];
                        }
                        else if (triangles[ta*3 + 1] == v)
                        {
                            a = triangles[ta*3 + 0];
                            b = triangles[ta*3 + 2];
                        }
                        else
                        {
                            a = triangles[ta*3 + 0];
                            b = triangles[ta*3 + 1];
                        }
                        if ( a < v && b < v)
                        {
                            int u0 = v2T.Begin(a);
                            int u1 = v2T.End(a);
                            for (long u = u0; u < u1; u++) 
                            {
                                long tb = v2T.GetNeighbor(u);
                                if (tb < 0)
                                {
                                    break;
                                }
                                long c = -1;
                                bool foundB = false;
                                for(long k = 0; k < 3; ++k)
                                {
                                    long x = triangles[tb*3 + k];
                                    if (x == b)
                                    {
                                        foundB = true;
                                    }
                                    if (x < v && x != a && x != b)
                                    {
                                        c = x;
                                    }
                                }
                                if (c != -1 && foundB)
                                {
                                    ++nt;
                                    for (unsigned long i = 0; i < dimFloatArray; i++) 
                                    {
                                        tpred[i] += m_quantFloatArray[a*dimFloatArray+i] + 
                                                    m_quantFloatArray[b*dimFloatArray+i] - 
                                                    m_quantFloatArray[c*dimFloatArray+i];
                                    }
                                }
                            }
                        }
                    }
                    if ( nt==0 &&
                        (predMode == O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION ||
                         predMode == O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION))
                    {                
                        for(long k = 0; k < 3; ++k)
                        {
                            long w = triangles[ta*3 + k];
                            if ( w < v )
                            {
                                ++nv;
                                for (unsigned long i = 0; i < dimFloatArray; i++) 
                                {
                                    vpred[i] += m_quantFloatArray[w*dimFloatArray+i];
                                }
                            }
                        }
                    }
                }
            }
            if (nt > 0)
            {
                for (unsigned long i = 0; i < dimFloatArray; i++) 
                {
                    if (m_streamType == O3DGC_SC3DMC_STREAM_TYPE_ASCII)
                    {
                        predResidual = bstream.ReadIntASCII(m_iterator);
                    }
                    else
                    {
                        predResidual = DecodeIntACEGC(acd, mModelValues, bModel0, bModel1, exp_k, M);
                    }
                    m_quantFloatArray[v*dimFloatArray+i] = predResidual + (tpred[i] + nt/2) / nt;
#ifdef DEBUG_VERBOSE
                    printf("%i \t %i\n", v*dimFloatArray+i, predResidual);
                    fprintf(g_fileDebugSC3DMCDec, "%i \t %i\n", v*dimFloatArray+i, predResidual);
#endif //DEBUG_VERBOSE
                }
            }
            else if (nv > 0)
            {
                for (unsigned long i = 0; i < dimFloatArray; i++) 
                {
                    if (m_streamType == O3DGC_SC3DMC_STREAM_TYPE_ASCII)
                    {
                        predResidual = bstream.ReadIntASCII(m_iterator);
                    }
                    else
                    {
                        predResidual = DecodeIntACEGC(acd, mModelValues, bModel0, bModel1, exp_k, M);
                    }
                    m_quantFloatArray[v*dimFloatArray+i] = predResidual + (vpred[i] + nv/2) / nv;
#ifdef DEBUG_VERBOSE
                    printf("%i \t %i\n", v*dimFloatArray+i, predResidual);
                    fprintf(g_fileDebugSC3DMCDec, "%i \t %i\n", v*dimFloatArray+i, predResidual);
#endif //DEBUG_VERBOSE
                }
            }
            else if (v > 0)
            {
                for (unsigned long i = 0; i < dimFloatArray; i++) 
                {
                    if (m_streamType == O3DGC_SC3DMC_STREAM_TYPE_ASCII)
                    {
                        predResidual = bstream.ReadIntASCII(m_iterator);
                    }
                    else
                    {
                        predResidual = DecodeIntACEGC(acd, mModelValues, bModel0, bModel1, exp_k, M);
                    }
                    m_quantFloatArray[v*dimFloatArray+i] = predResidual + m_quantFloatArray[(v-1)*dimFloatArray+i];
#ifdef DEBUG_VERBOSE
                    printf("%i \t %i\n", v*dimFloatArray+i, predResidual);
                    fprintf(g_fileDebugSC3DMCDec, "%i \t %i\n", v*dimFloatArray+i, predResidual);
#endif //DEBUG_VERBOSE
                }
            }
            else
            {
                for (unsigned long i = 0; i < dimFloatArray; i++) 
                {
                    if (m_streamType == O3DGC_SC3DMC_STREAM_TYPE_ASCII)
                    {
                        predResidual = bstream.ReadUIntASCII(m_iterator);
                    }
                    else
                    {
                        predResidual = DecodeUIntACEGC(acd, mModelValues, bModel0, bModel1, exp_k, M);
                    }
                    m_quantFloatArray[v*dimFloatArray+i] = predResidual;
#ifdef DEBUG_VERBOSE
                    printf("%i \t %i\n", v*dimFloatArray+i, predResidual);
                    fprintf(g_fileDebugSC3DMCDec, "%i \t %i\n", v*dimFloatArray+i, predResidual);
#endif //DEBUG_VERBOSE
                }
            }
        }
        IQuantizeFloatArray(floatArray, numFloatArray, dimFloatArray, minFloatArray, maxFloatArray, nQBits);
#ifdef DEBUG_VERBOSE
        fflush(g_fileDebugSC3DMCDec);
#endif //DEBUG_VERBOSE
        return O3DGC_OK;
    }
    O3DGCErrorCode SC3DMCDecoder::IQuantizeFloatArray(Real * const floatArray, 
                                                      unsigned long numFloatArray,
                                                      unsigned long dimFloatArray,
                                                      const Real * const minFloatArray,
                                                      const Real * const maxFloatArray,
                                                      unsigned long nQBits)
    {
        
        Real idelta[O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        Real r;
        for(unsigned long d = 0; d < dimFloatArray; d++)
        {
            r = maxFloatArray[d] - minFloatArray[d];
            if (r > 0.0f)
            {
                idelta[d] = r/(float)((1 << nQBits) - 1);
            }
            else 
            {
                idelta[d] = 1.0f;
            }
        }        
        for(unsigned long v = 0; v < numFloatArray; ++v)
        {
            for(unsigned long d = 0; d < dimFloatArray; ++d)
            {
                floatArray[v * dimFloatArray + d] = m_quantFloatArray[v * dimFloatArray + d] * idelta[d] + minFloatArray[d];
            }
        }
        return O3DGC_OK;
    }
}


