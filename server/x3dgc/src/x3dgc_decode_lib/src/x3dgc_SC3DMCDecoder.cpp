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

#include "x3dgc_SC3DMCDecoder.h"

//#define DEBUG_VERBOSE

namespace x3dgc
{
    X3DGCErrorCode SC3DMCDecoder::DecodeHeader(IndexedFaceSet & ifs, 
                                               const BinaryStream & bstream)
    {

        unsigned long start_code = bstream.ReadUInt32(m_iterator);

        if (start_code != X3DGC_SC3DMC_START_CODE)
        {
            return X3DGC_ERROR_CORRUPTED_STREAM;
        }
            
        m_streamSize = bstream.ReadUInt32(m_iterator); // to be filled later
        m_params.SetEncodeMode( (X3DGCSC3DMCEncodingMode) bstream.ReadUChar8(m_iterator));

        ifs.SetCreaseAngle((Real) bstream.ReadFloat32(m_iterator));
          
        unsigned char mask = bstream.ReadUChar8(m_iterator);;

        ifs.SetCCW             ((mask & 1) == 1);
        ifs.SetSolid           ((mask & 2) == 1);
        ifs.SetConvex          ((mask & 4) == 1);
        ifs.SetIsTriangularMesh((mask & 8) == 1);
        //bool markerBit0 = (mask & 16 ) == 1;
        //bool markerBit1 = (mask & 32 ) == 1;
        //bool markerBit2 = (mask & 64 ) == 1;
        //bool markerBit3 = (mask & 128) == 1;
       
        ifs.SetNCoord         (bstream.ReadUInt32(m_iterator));
        ifs.SetNNormal        (bstream.ReadUInt32(m_iterator));
        ifs.SetNColor         (bstream.ReadUInt32(m_iterator));
        ifs.SetNTexCoord      (bstream.ReadUInt32(m_iterator));


        ifs.SetNumFloatAttributes(bstream.ReadUInt32(m_iterator));
        ifs.SetNumIntAttributes  (bstream.ReadUInt32(m_iterator));
                              
        if (ifs.GetNCoord() > 0)
        {
            ifs.SetNCoordIndex(bstream.ReadUInt32(m_iterator));
             for(int j=0 ; j<3 ; ++j)
            {
                ifs.SetCoordMin(j, (Real) bstream.ReadFloat32(m_iterator));
                ifs.SetCoordMax(j, (Real) bstream.ReadFloat32(m_iterator));
            }
            m_params.SetCoordQuantBits( bstream.ReadUChar8(m_iterator) );
        }
        if (ifs.GetNNormal() > 0)
        {
            ifs.SetNNormalIndex(bstream.ReadUInt32(m_iterator));
             for(int j=0 ; j<3 ; ++j)
            {
                ifs.SetNormalMin(j, (Real) bstream.ReadFloat32(m_iterator));
                ifs.SetNormalMax(j, (Real) bstream.ReadFloat32(m_iterator));
            }
            ifs.SetNormalPerVertex(bstream.ReadUChar8(m_iterator) == 1);
            m_params.SetNormalQuantBits(bstream.ReadUChar8(m_iterator));
        }
        if (ifs.GetNColor() > 0)
        {
            ifs.SetNColorIndex(bstream.ReadUInt32(m_iterator));
             for(int j=0 ; j<3 ; ++j)
            {
                ifs.SetColorMin(j, (Real) bstream.ReadFloat32(m_iterator));
                ifs.SetColorMax(j, (Real) bstream.ReadFloat32(m_iterator));
            }
            ifs.SetColorPerVertex(bstream.ReadUChar8(m_iterator)==1);
            m_params.SetColorQuantBits(bstream.ReadUChar8(m_iterator));
        }
        if (ifs.GetNTexCoord() > 0)
        {
            ifs.SetNTexCoordIndex(bstream.ReadUInt32(m_iterator));
             for(int j=0 ; j<2 ; ++j)
            {
                ifs.SetTexCoordMin(j, (Real) bstream.ReadFloat32(m_iterator));
                ifs.SetTexCoordMax(j, (Real) bstream.ReadFloat32(m_iterator));
            }
            m_params.SetTexCoordQuantBits(bstream.ReadUChar8(m_iterator));
        }

        for(unsigned long a = 0; a < ifs.GetNumFloatAttributes(); ++a)
        {
            ifs.SetNFloatAttribute(a, bstream.ReadUInt32(m_iterator));    
            if (ifs.GetNFloatAttribute(a) > 0)
            {
                ifs.SetNFloatAttributeIndex(a, bstream.ReadUInt32(m_iterator));
                unsigned char d = bstream.ReadUChar8(m_iterator);
                ifs.SetFloatAttributeDim(a, d);
                for(unsigned char j = 0 ; j < d ; ++j)
                {
                    ifs.SetFloatAttributeMin(a, j, (Real) bstream.ReadFloat32(m_iterator));
                    ifs.SetFloatAttributeMax(a, j, (Real) bstream.ReadFloat32(m_iterator));
                }
                ifs.SetFloatAttributePerVertex(a, bstream.ReadUChar8(m_iterator) == 1);
                m_params.SetFloatAttributeQuantBits(a, bstream.ReadUChar8(m_iterator));
            }
        }
        for(unsigned long a = 0; a < ifs.GetNumIntAttributes(); ++a)
        {
            ifs.SetNIntAttribute(a, bstream.ReadUInt32(m_iterator));
            if (ifs.GetNIntAttribute(a) > 0)
            {
                ifs.SetNIntAttributeIndex(a, bstream.ReadUInt32(m_iterator));
                ifs.SetIntAttributeDim(a, bstream.ReadUChar8(m_iterator));
                ifs.SetIntAttributePerVertex(a, bstream.ReadUChar8(m_iterator) == 1);
            }
        }    
        return X3DGC_OK;
    }

    X3DGCErrorCode SC3DMCDecoder::DecodePlayload(IndexedFaceSet & ifs,
                                                 const BinaryStream & bstream)
    {
        m_binarization = m_params.GetBinarization();
        m_triangleListDecoder.SetBinarization(m_binarization);
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

        return X3DGC_OK;
    }
    X3DGCErrorCode SC3DMCDecoder::DecodeIntArray(long * const intArray, 
                                                 unsigned long numIntArraySize,
                                                 unsigned long dimIntArraySize,
                                                 const BinaryStream & bstream)
    {        
        const long nvert = (long) numIntArraySize;
        bstream.ReadUInt32(m_iterator); // bistream size
        for (long v=0; v < nvert; ++v) 
        {
            for (unsigned long i = 0; i < dimIntArraySize; i++) 
            {
                DecodePredicionResidual(intArray[v*dimIntArraySize+i], bstream, false);
            }
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode SC3DMCDecoder::DecodeFloatArray(Real * const floatArray, 
                                                   unsigned long numfloatArraySize,
                                                   unsigned long dimfloatArraySize,
                                                   const Real * const minfloatArray,
                                                   const Real * const maxfloatArray,
                                                   unsigned long nQBits,
                                                   const IndexedFaceSet & ifs,
                                                   X3DGCSC3DMCPredictionMode predMode,
                                                   const BinaryStream & bstream)
    {
        assert(dimfloatArraySize <  X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES);
        const AdjacencyInfo & v2T    = m_triangleListDecoder.GetVertexToTriangle();
        const long * const triangles = ifs.GetCoordIndex();
        long vpred[X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        long tpred[X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        long nv, nt;
        long predResidual;        
        const long nvert = (long) numfloatArraySize;
        const unsigned long size = numfloatArraySize * dimfloatArraySize;
        bstream.ReadUInt32(m_iterator);        // bitsream size
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
                 predMode != X3DGC_SC3DMC_NO_PREDICTION)
            {
                for (unsigned long i = 0; i < dimfloatArraySize; i++) 
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
                    if (predMode == X3DGC_SC3DMC_PARALLELOGRAM_PREDICTION)
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
                        else if (triangles[ta*3 + 2] == v)
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
                                    for (unsigned long i = 0; i < dimfloatArraySize; i++) 
                                    {
                                        tpred[i] += m_quantFloatArray[a*dimfloatArraySize+i] + 
                                                    m_quantFloatArray[b*dimfloatArraySize+i] - 
                                                    m_quantFloatArray[c*dimfloatArraySize+i];
                                    }
                                }
                            }
                        }
                    }
                    if ( nt==0 &&
                        (predMode == X3DGC_SC3DMC_PARALLELOGRAM_PREDICTION ||
                         predMode == X3DGC_SC3DMC_DIFFERENTIAL_PREDICTION))
                    {                
                        for(long k = 0; k < 3; ++k)
                        {
                            long w = triangles[ta*3 + k];
                            if ( w < v )
                            {
                                ++nv;
                                for (unsigned long i = 0; i < dimfloatArraySize; i++) 
                                {
                                    vpred[i] += m_quantFloatArray[w*dimfloatArraySize+i];
                                }
                            }
                        }
                    }
                }
            }
            if (nt > 0)
            {
                for (unsigned long i = 0; i < dimfloatArraySize; i++) 
                {
                    DecodePredicionResidual(predResidual, bstream, true);
                    m_quantFloatArray[v*dimfloatArraySize+i] = predResidual + (tpred[i] + nt/2) / nt;
                }
            }
            else if (nv > 0)
            {
                for (unsigned long i = 0; i < dimfloatArraySize; i++) 
                {
                    DecodePredicionResidual(predResidual, bstream, true);
                    m_quantFloatArray[v*dimfloatArraySize+i] = predResidual + (vpred[i] + nv/2) / nv;
                }
            }
            else
            {
                for (unsigned long i = 0; i < dimfloatArraySize; i++) 
                {
                    DecodePredicionResidual(predResidual, bstream, false);
                    m_quantFloatArray[v*dimfloatArraySize+i] = predResidual;
                }
            }
        }
        IQuantizeFloatArray(floatArray, numfloatArraySize, dimfloatArraySize, minfloatArray, maxfloatArray, nQBits);
        return X3DGC_OK;
    }
    X3DGCErrorCode SC3DMCDecoder::IQuantizeFloatArray(Real * const floatArray, 
                                                      unsigned long numfloatArraySize,
                                                      unsigned long dimfloatArraySize,
                                                      const Real * const minfloatArray,
                                                      const Real * const maxfloatArray,
                                                      unsigned long nQBits)
    {
        
        Real idelta[X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        Real r;
        for(unsigned long d = 0; d < dimfloatArraySize; d++)
        {
            r = maxfloatArray[d] - minfloatArray[d];
            if (r > 0.0f)
            {
                idelta[d] = r/(float)((1 << nQBits) - 1);
            }
            else 
            {
                idelta[d] = 1.0f;
            }
        }        
        for(unsigned long v = 0; v < numfloatArraySize; ++v)
        {
            for(unsigned long d = 0; d < dimfloatArraySize; ++d)
            {
                floatArray[v * dimfloatArraySize + d] = m_quantFloatArray[v * dimfloatArraySize + d] * idelta[d] + minfloatArray[d];
            }
        }
        return X3DGC_OK;
    }
    inline X3DGCErrorCode SC3DMCDecoder::DecodePredicionResidual(long & predResidual, 
                                                                 const BinaryStream & bstream, 
                                                                 bool predicted)
    {
        predResidual = bstream.ReadUChar8(m_iterator);
        if (m_binarization == X3DGC_SC3DMC_GZIP &&
            predResidual == X3DGC_RESIDUAL_THRESHOLD)
        {
            for(long h = 0; h < X3DGC_RESIDUAL_MAX_BITS; h+=X3DGC_RESIDUAL_BITS)
            {
                predResidual += (bstream.ReadUChar8(m_iterator) << h);
            }
        }
        if (predicted)
        {
            if (predResidual & 1)
            {
                predResidual = - (predResidual>>1);
            }
            else
            {
                predResidual >>= 1;
            }
        }
        return X3DGC_OK;
    }
}


