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

#include "x3dgc_SC3DMCEncoder.h"

//#define DEBUG_VERBOSE

namespace x3dgc
{
    X3DGCErrorCode SC3DMCEncoder::Encode(const SC3DMCEncodeParams & params, 
                                  const IndexedFaceSet & ifs, 
                                  BinaryStream & bstream)
    {
        // Encode header
        unsigned long start = bstream.GetSize();
        EncodeHeader(params, ifs, bstream);
        // Encode payload
        EncodePayload(params, ifs, bstream);
        bstream.WriteUInt32(4, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode SC3DMCEncoder::EncodeHeader(const SC3DMCEncodeParams & params, 
                                        const IndexedFaceSet & ifs, 
                                        BinaryStream & bstream)
    {
        bstream.WriteUInt32(X3DGC_SC3DMC_START_CODE);
        bstream.WriteUInt32(0); // to be filled later

        bstream.WriteUChar8(X3DGC_SC3DMC_TFAN);
        bstream.WriteFloat32((float)ifs.GetCreaseAngle());
          
        unsigned char mask = 0;
        bool markerBit0 = false;
        bool markerBit1 = false;
        bool markerBit2 = false;
        bool markerBit3 = false;

        mask += (ifs.GetCCW()                  );
        mask += (ifs.GetSolid()            << 1);
        mask += (ifs.GetConvex()           << 2);
        mask += (ifs.GetIsTriangularMesh() << 3);
        mask += (markerBit0                << 4);
        mask += (markerBit1                << 5);
        mask += (markerBit2                << 6);
        mask += (markerBit3                << 7);

        bstream.WriteUChar8(mask);

        bstream.WriteUInt32(ifs.GetNCoord());
        bstream.WriteUInt32(ifs.GetNNormal());
        bstream.WriteUInt32(ifs.GetNColor());
        bstream.WriteUInt32(ifs.GetNTexCoord());
        bstream.WriteUInt32(ifs.GetNumFloatAttributes());
        bstream.WriteUInt32(ifs.GetNumIntAttributes());

        if (ifs.GetNCoord() > 0)
        {
            bstream.WriteUInt32(ifs.GetNCoordIndex());
             for(int j=0 ; j<3 ; ++j)
            {
                bstream.WriteFloat32((float) ifs.GetCoordMin(j));
                bstream.WriteFloat32((float) ifs.GetCoordMax(j));
            }            
            bstream.WriteUChar8((unsigned char) params.GetCoordQuantBits());
        }
        if (ifs.GetNNormal() > 0)
        {
            bstream.WriteUInt32(0);
             for(int j=0 ; j<3 ; ++j)
            {
                bstream.WriteFloat32((float) ifs.GetNormalMin(j));
                bstream.WriteFloat32((float) ifs.GetNormalMax(j));
            }
            bstream.WriteUChar8(true); //(unsigned char) ifs.GetNormalPerVertex()
            bstream.WriteUChar8((unsigned char) params.GetNormalQuantBits());
        }
        if (ifs.GetNColor() > 0)
        {
            bstream.WriteUInt32(0);
             for(int j=0 ; j<3 ; ++j)
            {
                bstream.WriteFloat32((float) ifs.GetColorMin(j));
                bstream.WriteFloat32((float) ifs.GetColorMax(j));
            }
            bstream.WriteUChar8(true); // (unsigned char) ifs.GetColorPerVertex()
            bstream.WriteUChar8((unsigned char) params.GetColorQuantBits());
        }
        if (ifs.GetNTexCoord() > 0)
        {
            bstream.WriteUInt32(0);
             for(int j=0 ; j<2 ; ++j)
            {
                bstream.WriteFloat32((float) ifs.GetTexCoordMin(j));
                bstream.WriteFloat32((float) ifs.GetTexCoordMax(j));
            }
            bstream.WriteUChar8((unsigned char) params.GetTexCoordQuantBits());
        }
        for(unsigned long a = 0; a < ifs.GetNumFloatAttributes(); ++a)
        {
            bstream.WriteUInt32(ifs.GetNFloatAttribute(a));
            if (ifs.GetNFloatAttribute(a) > 0)
            {
                assert(ifs.GetFloatAttributeDim(a) < (unsigned long) X3DGC_MAX_UCHAR8);
                bstream.WriteUInt32(0);
                unsigned char d = (unsigned char) ifs.GetFloatAttributeDim(a);
                bstream.WriteUChar8(d);
                for(unsigned char j = 0 ; j < d ; ++j)
                {
                    bstream.WriteFloat32((float) ifs.GetFloatAttributeMin(a, j));
                    bstream.WriteFloat32((float) ifs.GetFloatAttributeMax(a, j));
                }
                bstream.WriteUChar8(true); //(unsigned char) ifs.GetFloatAttributePerVertex(a)
                bstream.WriteUChar8((unsigned char) params.GetFloatAttributeQuantBits(a));
            }
        }
        for(unsigned long a = 0; a < ifs.GetNumIntAttributes(); ++a)
        {
            bstream.WriteUInt32(ifs.GetNIntAttribute(a));
            if (ifs.GetNIntAttribute(a) > 0)
            {
                assert(ifs.GetFloatAttributeDim(a) < (unsigned long) X3DGC_MAX_UCHAR8);
                bstream.WriteUInt32(0);
                bstream.WriteUChar8((unsigned char) ifs.GetIntAttributeDim(a));
                bstream.WriteUChar8(true); // (unsigned char) ifs.GetIntAttributePerVertex(a)
            }
        }    
        return X3DGC_OK;
    }    
    X3DGCErrorCode SC3DMCEncoder::QuantizeFloatArray(const Real * const floatArray, 
                                                   unsigned long numFloatArraySize,
                                                   unsigned long dimFloatArraySize,
                                                   const Real * const minFloatArray,
                                                   const Real * const maxFloatArray,
                                                   unsigned long nQBits)
    {
        const unsigned long size = numFloatArraySize * dimFloatArraySize;
        Real delta[X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        Real r;
        for(unsigned long d = 0; d < dimFloatArraySize; d++)
        {
            r = maxFloatArray[d] - minFloatArray[d];
            if (r > 0.0f)
            {
                delta[d] = (float)((1 << nQBits) - 1) / r;
            }
            else
            {
                delta[d] = 1.0f;
            }
        }        
        if (m_quantFloatArraySize < size)
        {
            delete [] m_quantFloatArray;
            m_quantFloatArraySize = size;
            m_quantFloatArray     = new long [size];
        }                                  
        for(unsigned long v = 0; v < numFloatArraySize; ++v)
        {
            for(unsigned long d = 0; d < dimFloatArraySize; ++d)
            {
                m_quantFloatArray[v * dimFloatArraySize + d] = (long)((floatArray[v * dimFloatArraySize + d]-minFloatArray[d]) * delta[d] + 0.5f);
            }
        }
        return X3DGC_OK;
    }
    inline X3DGCErrorCode SC3DMCEncoder::EncodePredicionResidual(long predResidual, 
                                                                 BinaryStream & bstream, 
                                                                 bool predicted)
    {
        if (predicted)
        {
            if (predResidual < 0)
            {
                predResidual = 1-2*predResidual;
            }
            else
            {
                predResidual *= 2;
            }
        }
        if (m_binarization == X3DGC_SC3DMC_GZIP &&
            predResidual >= X3DGC_RESIDUAL_THRESHOLD)
        {
            bstream.WriteUChar8(X3DGC_RESIDUAL_THRESHOLD);
            predResidual -= X3DGC_RESIDUAL_THRESHOLD;
            const long mask = X3DGC_RESIDUAL_THRESHOLD;
            for(long h = 0; h < X3DGC_RESIDUAL_MAX_BITS; h+=X3DGC_RESIDUAL_BITS)
            {
                bstream.WriteUChar8((unsigned char) (predResidual & mask));
                predResidual >>= X3DGC_RESIDUAL_BITS;
            }
        }
        else
        {
            bstream.WriteUChar8((unsigned char) predResidual);
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode SC3DMCEncoder::EncodeFloatArray(const Real * const floatArray, 
                                                   unsigned long numFloatArraySize,
                                                   unsigned long dimFloatArraySize,
                                                   const Real * const minFloatArray,
                                                   const Real * const maxFloatArray,
                                                   unsigned long nQBits,
                                                   const IndexedFaceSet & ifs,
                                                   X3DGCSC3DMCPredictionMode predMode,
                                                   BinaryStream & bstream)
    {
        assert(dimFloatArraySize <  X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES);
        const AdjacencyInfo & v2T    = m_triangleListEncoder.GetVertexToTriangle();
        const long * const vmap      = m_triangleListEncoder.GetVMap();
        const long * const invVMap   = m_triangleListEncoder.GetInvVMap();
        const long * const triangles = ifs.GetCoordIndex();
        long vpred[X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        long tpred[X3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES];
        long nv, nt;
        long v;
        long predResidual;        
        const long nvert = (long) numFloatArraySize;

        unsigned long start = bstream.GetSize();        
        bstream.WriteUInt32(0); 

        QuantizeFloatArray(floatArray, numFloatArraySize, dimFloatArraySize, minFloatArray, maxFloatArray, nQBits);
        for (long vm=0; vm < nvert; ++vm) 
        {
            v = invVMap[vm];
            assert( v >= 0 && v < nvert);
            nv = 0;
            nt = 0;

            if ( v2T.GetNumNeighbors(v) > 0 && 
                 predMode != X3DGC_SC3DMC_NO_PREDICTION)
            {
                for (unsigned long i = 0; i < dimFloatArraySize; i++) 
                {
                    vpred[i] = 0;
                    tpred[i] = 0;
                }
                int u0 = v2T.Begin(v);
                int u1 = v2T.End(v);
                for (long u = u0; u < u1; u++) 
                {
                    long ta = v2T.GetNeighbor(u);
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
                        if ( vmap[a] < vm && vmap[b] < vm)
                        {
                            int u0 = v2T.Begin(a);
                            int u1 = v2T.End(a);
                            for (long u = u0; u < u1; u++) 
                            {
                                long tb = v2T.GetNeighbor(u);
                                long c = -1;
                                bool foundB = false;
                                for(long k = 0; k < 3; ++k)
                                {
                                    long x = triangles[tb*3 + k];
                                    if (x == b)
                                    {
                                        foundB = true;
                                    }
                                    if (vmap[x] < vm && x != a && x != b)
                                    {
                                        c = x;
                                    }
                                }
                                if (c != -1 && foundB)
                                {
                                    ++nt;
                                    for (unsigned long i = 0; i < dimFloatArraySize; i++) 
                                    {
                                        tpred[i] += m_quantFloatArray[a*dimFloatArraySize+i] + 
                                                    m_quantFloatArray[b*dimFloatArraySize+i] - 
                                                    m_quantFloatArray[c*dimFloatArraySize+i];
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
                            if ( vmap[w] < vm )
                            {
                                ++nv;
                                for (unsigned long i = 0; i < dimFloatArraySize; i++) 
                                {
                                    vpred[i] += m_quantFloatArray[w*dimFloatArraySize+i];
                                }
                            }
                        }
                    }        
                }
            }
            if (nt > 0)
            {
                for (unsigned long i = 0; i < dimFloatArraySize; i++) 
                {
                    predResidual = m_quantFloatArray[v*dimFloatArraySize+i] - (tpred[i] + nt/2) / nt;
                    EncodePredicionResidual(predResidual, bstream, true);
                }
            }
            else if (nv > 0)
            {
                for (unsigned long i = 0; i < dimFloatArraySize; i++) 
                {
                    predResidual = m_quantFloatArray[v*dimFloatArraySize+i] - (vpred[i] + nv/2) / nv ;
                    EncodePredicionResidual(predResidual, bstream, true);
                }
            }
            else
            {
                for (unsigned long i = 0; i < dimFloatArraySize; i++) 
                {
                    predResidual = m_quantFloatArray[v*dimFloatArraySize+i];
                    EncodePredicionResidual(predResidual, bstream, false);
                }
            }
        }
        bstream.WriteUInt32(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode SC3DMCEncoder::EncodeIntArray(const long * const intArray, 
                                                 unsigned long numIntArraySize,
                                                 unsigned long dimIntArraySize,
                                                 BinaryStream & bstream)
    {
        const long * const invVMap   = m_triangleListEncoder.GetInvVMap();
        unsigned long start = bstream.GetSize();
        bstream.WriteUInt32(0); 
        long v;
        for (unsigned long vm=0; vm < numIntArraySize; ++vm) 
        {
            v = invVMap[vm];
            assert( v >= 0 && v < (long) numIntArraySize);
            for (unsigned long i = 0; i < dimIntArraySize; i++) 
            {
                assert( intArray[v*dimIntArraySize+i] < (1<<X3DGC_RESIDUAL_MAX_BITS) &&
                        intArray[v*dimIntArraySize+i] >= 0);
                EncodePredicionResidual(intArray[v*dimIntArraySize+i], bstream, false);
            }
        }
        bstream.WriteUInt32(start, bstream.GetSize() - start);
        return X3DGC_OK;
    }
    X3DGCErrorCode SC3DMCEncoder::EncodePayload(const SC3DMCEncodeParams & params, 
                                                const IndexedFaceSet & ifs, 
                                                BinaryStream & bstream)
    {
        m_binarization = params.GetBinarization();
        // encode triangle list        
        m_triangleListEncoder.SetBinarization(params.GetBinarization());
        m_triangleListEncoder.Encode(ifs.GetCoordIndex(), ifs.GetNCoordIndex(), ifs.GetNCoord(), bstream);

        // encode coord
        if (ifs.GetNCoord() > 0)
        {
            EncodeFloatArray(ifs.GetCoord(), ifs.GetNCoord(), 3, ifs.GetCoordMin(), ifs.GetCoordMax(), 
                                params.GetCoordQuantBits(), ifs, params.GetCoordPredMode(), bstream);
        }
        // encode Normal
        if (ifs.GetNNormal() > 0)
        {
            EncodeFloatArray(ifs.GetNormal(), ifs.GetNNormal(), 3, ifs.GetNormalMin(), ifs.GetNormalMax(), 
                                params.GetNormalQuantBits(), ifs, params.GetNormalPredMode(), bstream);
        }
        // encode Color
        if (ifs.GetNColor() > 0)
        {
            EncodeFloatArray(ifs.GetColor(), ifs.GetNColor(), 3, ifs.GetColorMin(), ifs.GetColorMax(), 
                                params.GetColorQuantBits(), ifs, params.GetColorPredMode(), bstream);
        }
        // encode TexCoord
        if (ifs.GetNTexCoord() > 0)
        {
            EncodeFloatArray(ifs.GetTexCoord(), ifs.GetNTexCoord(), 2, ifs.GetTexCoordMin(), ifs.GetTexCoordMax(), 
                                params.GetTexCoordQuantBits(), ifs, params.GetTexCoordPredMode(), bstream);
        }
        for(unsigned long a = 0; a < ifs.GetNumFloatAttributes(); ++a)
        {
            EncodeFloatArray(ifs.GetFloatAttribute(a), ifs.GetNFloatAttribute(a), ifs.GetFloatAttributeDim(a), ifs.GetFloatAttributeMin(a), ifs.GetFloatAttributeMax(a), 
                                params.GetFloatAttributeQuantBits(a), ifs, params.GetFloatAttributePredMode(a), bstream);
        }
        for(unsigned long a = 0; a < ifs.GetNumIntAttributes(); ++a)
        {
            EncodeIntArray(ifs.GetIntAttribute(a), ifs.GetNIntAttribute(a), ifs.GetIntAttributeDim(a), bstream);
        }
        return X3DGC_OK;
    }
}


