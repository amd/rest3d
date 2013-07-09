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
#ifndef X3DGC_TRIANGLE_FANS_H
#define X3DGC_TRIANGLE_FANS_H

#include "x3dgc_Common.h"
#include "x3dgc_Vector.h"
#include "x3dgc_BinaryStream.h"


namespace x3dgc
{
    const long X3DGC_TFANS_MIN_SIZE_ALLOCATED_VERTICES_BUFFER = 128;
    const long X3DGC_TFANS_MIN_SIZE_TFAN_SIZE_BUFFER          = 8;

    const long X3DGC_NUM_TFANS_SIZE_THRESHOLD                 = 15;
    const long X3DGC_NUM_TFANS_SIZE_BITS                      = 4; 
    const long X3DGC_NUM_TFANS_SIZE_MAX_BITS                  = 8; 
    
    const long X3DGC_TFAN_DEGREE_THRESHOLD                    = 15;
    const long X3DGC_TFAN_DEGREE_BITS                         = 4;
    const long X3DGC_TFAN_DEGREE_MAX_BITS                     = 16;

    const long X3DGC_TFAN_INDEX_THRESHOLD                    = 255;
    const long X3DGC_TFAN_INDEX_BITS                         = 8;
    const long X3DGC_TFAN_INDEX_MAX_BITS                     = 24;

    const long X3DGC_RESIDUAL_THRESHOLD                      = 255;
    const long X3DGC_RESIDUAL_BITS                           = 8;
    const long X3DGC_RESIDUAL_MAX_BITS                       = 16;

    class CompressedTriangleFans
    {
    public:    
        //! Constructor.
                                    CompressedTriangleFans(void)
                                    {
                                        m_binarization = X3DGC_SC3DMC_UNKOWN;
                                    };
        //! Destructor.
                                    ~CompressedTriangleFans(void) {};
        X3DGCSC3DMCBinarization     GetBinarization() const { return m_binarization; }
        void                        SetBinarization(X3DGCSC3DMCBinarization binarization) { m_binarization = binarization; }

        X3DGCErrorCode              Allocate(long numVertices)
                                    {
                                        assert(numVertices > 0);
                                        m_numTFANs.Allocate(numVertices);
                                        m_degrees.Allocate(2*numVertices);
                                        m_configs.Allocate(2*numVertices);
                                        m_operations.Allocate(2*numVertices);
                                        m_indices.Allocate(2*numVertices);
                                        Clear();
                                        return X3DGC_OK;
                                    }
        X3DGCErrorCode              PushNumTFans(long numTFans)
                                    {
                                        assert(numTFans < (1<<X3DGC_NUM_TFANS_SIZE_MAX_BITS) + X3DGC_NUM_TFANS_SIZE_THRESHOLD);
                                        if (m_binarization == X3DGC_SC3DMC_GZIP &&
                                            numTFans >= X3DGC_NUM_TFANS_SIZE_THRESHOLD)
                                        {
                                            m_numTFANs.PushBack(X3DGC_NUM_TFANS_SIZE_THRESHOLD);
                                            numTFans -= X3DGC_NUM_TFANS_SIZE_THRESHOLD;
                                            const long mask = X3DGC_NUM_TFANS_SIZE_THRESHOLD;
                                            for(long h = 0; h < X3DGC_NUM_TFANS_SIZE_MAX_BITS; h+=X3DGC_NUM_TFANS_SIZE_BITS)
                                            {
                                                m_numTFANs.PushBack( numTFans & mask);
                                                numTFans >>= X3DGC_NUM_TFANS_SIZE_BITS;
                                            }
                                        }
                                        else
                                        {
                                            m_numTFANs.PushBack(numTFans);
                                        }
                                        return X3DGC_OK;
                                    }
        long                        ReadNumTFans(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_numTFANs.GetSize());
                                        long numTFans = m_numTFANs[iterator++];
                                        if (m_binarization == X3DGC_SC3DMC_GZIP &&
                                            numTFans == X3DGC_NUM_TFANS_SIZE_THRESHOLD)
                                        {
                                            for(long h = 0; h < X3DGC_NUM_TFANS_SIZE_MAX_BITS; h+=X3DGC_NUM_TFANS_SIZE_BITS)
                                            {
                                                numTFans += (m_numTFANs[iterator++] << h);
                                            }
                                        }
                                        return numTFans;
                                    }
        X3DGCErrorCode              PushDegree(long degree)
                                    {
                                        assert(degree < (1<<X3DGC_TFAN_DEGREE_MAX_BITS) + X3DGC_TFAN_DEGREE_THRESHOLD);
                                        if (m_binarization == X3DGC_SC3DMC_GZIP &&
                                            degree >= X3DGC_TFAN_DEGREE_THRESHOLD)
                                        {
                                            m_degrees.PushBack(X3DGC_TFAN_DEGREE_THRESHOLD);
                                            degree -= X3DGC_TFAN_DEGREE_THRESHOLD;
                                            const long mask  = X3DGC_TFAN_DEGREE_THRESHOLD;
                                            for(long h = 0; h < X3DGC_TFAN_DEGREE_MAX_BITS; h+=X3DGC_TFAN_DEGREE_BITS)
                                            {
                                                m_degrees.PushBack( degree & mask);
                                                degree >>= X3DGC_TFAN_DEGREE_BITS;
                                            }
                                        }
                                        else
                                        {
                                            m_degrees.PushBack(degree);
                                        }
                                        return X3DGC_OK;
                                    }
        long                        ReadDegree(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_degrees.GetSize());
                                        long degree = m_degrees[iterator++];
                                        if (m_binarization == X3DGC_SC3DMC_GZIP &&
                                            degree == X3DGC_TFAN_DEGREE_THRESHOLD)
                                        {
                                            for(long h = 0; h < X3DGC_TFAN_DEGREE_MAX_BITS; h+=X3DGC_TFAN_DEGREE_BITS)
                                            {
                                                degree += (m_degrees[iterator++] << h);
                                            }
                                        }
                                        return degree;
                                    }
        X3DGCErrorCode              PushConfig(long config)
                                    {
                                        m_configs.PushBack(config);
                                        return X3DGC_OK;
                                    }
        long                        ReadConfig(unsigned long & iterator) const
                                    {
                                        return m_configs[iterator++];
                                    }
        X3DGCErrorCode              PushOperation(long op)
                                    {
                                        m_operations.PushBack(op);
                                        return X3DGC_OK;
                                    }
        long                        ReadOperation(unsigned long & iterator) const
                                    {
                                        return m_operations[iterator++];
                                    }
        X3DGCErrorCode              PushIndex(long index)
                                    {
                                        if (index < 0)
                                        {
                                            index = 1-2*index;
                                        }
                                        else
                                        {
                                            index *= 2;
                                        }
                                        if (m_binarization == X3DGC_SC3DMC_GZIP &&
                                            index >= X3DGC_TFAN_INDEX_THRESHOLD)
                                        {
                                            m_indices.PushBack(X3DGC_TFAN_INDEX_THRESHOLD);
                                            index -= X3DGC_TFAN_INDEX_THRESHOLD;
                                            const long mask  = X3DGC_TFAN_INDEX_THRESHOLD;
                                            for(long h = 0; h < X3DGC_TFAN_INDEX_MAX_BITS; h+=X3DGC_TFAN_INDEX_BITS)
                                            {
                                                m_indices.PushBack( index & mask);
                                                index >>= X3DGC_TFAN_INDEX_BITS;
                                            }
                                        }
                                        else
                                        {
                                            m_indices.PushBack(index);
                                        }
                                        return X3DGC_OK;
                                    }
        long                        ReadIndex(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_indices.GetSize());
                                        long index = m_indices[iterator++];
                                        if (m_binarization == X3DGC_SC3DMC_GZIP &&
                                            index == X3DGC_TFAN_INDEX_THRESHOLD)
                                        {
                                            for(long h = 0; h < X3DGC_TFAN_INDEX_MAX_BITS; h+=X3DGC_TFAN_INDEX_BITS)
                                            {
                                                index += (m_indices[iterator++] << h);
                                            }
                                        }
                                        if (index & 1)
                                        {
                                            index = - (index>>1);
                                        }
                                        else
                                        {
                                            index >>= 1;
                                        }
                                        return index;
                                    }
        X3DGCErrorCode              Clear()
                                    {
                                        m_numTFANs.Clear();
                                        m_degrees.Clear();
                                        m_configs.Clear();
                                        m_operations.Clear();
                                        m_indices.Clear();
                                        return X3DGC_OK;
                                    }
        X3DGCErrorCode              Save(BinaryStream & bstream) const;
        X3DGCErrorCode              Load(const BinaryStream & bstream, unsigned long & iterator);

    private:
        Vector<long>                m_numTFANs;
        Vector<long>                m_degrees;
        Vector<long>                m_configs;
        Vector<long>                m_operations;
        Vector<long>                m_indices;
        X3DGCSC3DMCBinarization     m_binarization;
    };

    //! 
    class TriangleFans
    {
    public:    
        //! Constructor.
                                    TriangleFans(long sizeTFAN     = X3DGC_TFANS_MIN_SIZE_TFAN_SIZE_BUFFER, 
                                                 long verticesSize = X3DGC_TFANS_MIN_SIZE_ALLOCATED_VERTICES_BUFFER)
                                    {
                                        assert(sizeTFAN     > 0);
                                        assert(verticesSize > 0);
                                        m_numTFANs              = 0;
                                        m_numVertices           = 0;
                                        m_verticesAllocatedSize = verticesSize;
                                        m_sizeTFANAllocatedSize = sizeTFAN;
                                        m_sizeTFAN              = new long [m_sizeTFANAllocatedSize];
                                        m_vertices              = new long [m_verticesAllocatedSize];
                                    };
        //! Destructor.
                                    ~TriangleFans(void)
                                    {
                                        delete [] m_vertices;
                                        delete [] m_sizeTFAN;
                                    };

        X3DGCErrorCode              Allocate(long sizeTFAN, long verticesSize)
                                    {
                                        assert(sizeTFAN     > 0);
                                        assert(verticesSize > 0);
                                        m_numTFANs    = 0;
                                        m_numVertices = 0;
                                        if (m_verticesAllocatedSize < verticesSize)
                                        {
                                            delete [] m_vertices;
                                            m_verticesAllocatedSize = verticesSize;
                                            m_vertices              = new long [m_verticesAllocatedSize];
                                        }
                                        if (m_sizeTFANAllocatedSize < sizeTFAN)
                                        {
                                            delete [] m_sizeTFAN;
                                            m_sizeTFANAllocatedSize = sizeTFAN;
                                            m_sizeTFAN              = new long [m_sizeTFANAllocatedSize];
                                        }
                                        return X3DGC_OK;
                                    };
        X3DGCErrorCode              Clear()
                                    {
                                        m_numTFANs    = 0;
                                        m_numVertices = 0;
                                        return X3DGC_OK;
                                    }
        X3DGCErrorCode              AddVertex(long vertex) 
                                    {
                                        assert(m_numTFANs    >= 0);
                                        assert(m_numTFANs    <  m_sizeTFANAllocatedSize);
                                        assert(m_numVertices >= 0);
                                        ++m_numVertices;
                                        if (m_numVertices == m_verticesAllocatedSize)
                                        {
                                            m_verticesAllocatedSize *= 2;
                                            long * tmp = m_vertices;
                                            m_vertices = new long [m_verticesAllocatedSize];
                                            memcpy(m_vertices, tmp, sizeof(long) * m_numVertices);
                                            delete [] tmp;
                                        }
                                        m_vertices[m_numVertices-1] = vertex;
                                        ++m_sizeTFAN[m_numTFANs-1];
                                        return X3DGC_OK;
                                    }
        X3DGCErrorCode              AddTFAN()
                                    {
                                        assert(m_numTFANs >= 0);
                                        ++m_numTFANs;
                                        if (m_numTFANs == m_sizeTFANAllocatedSize)
                                        {
                                            m_sizeTFANAllocatedSize *= 2;
                                            long * tmp = m_sizeTFAN;
                                            m_sizeTFAN = new long [m_sizeTFANAllocatedSize];
                                            memcpy(m_sizeTFAN, tmp, sizeof(long) * m_numTFANs);
                                            delete [] tmp;
                                        }
                                        m_sizeTFAN[m_numTFANs-1] = (m_numTFANs > 1) ? m_sizeTFAN[m_numTFANs-2] : 0;
                                        return X3DGC_OK;
                                    }
        long                        Begin(long tfan) const 
                                    {
                                        assert(tfan < m_numTFANs);
                                        assert(tfan >= 0);
                                        return (tfan>0)?m_sizeTFAN[tfan-1]:0;
                                    }
        long                        End(long tfan) const
                                    {
                                        assert(tfan < m_numTFANs);
                                        assert(tfan >= 0);
                                        return m_sizeTFAN[tfan];
                                    }
        long                        GetVertex(long vertex) const
                                    {
                                        assert(vertex < m_numVertices);
                                        assert(vertex >= 0);
                                        return m_vertices[vertex];
                                    }
        long                        GetTFANSize(long tfan)  const 
                                    { 
                                        return End(tfan) - Begin(tfan);
                                    }
        long                        GetNumTFANs()  const 
                                    { 
                                        return m_numTFANs;
                                    }
        long                        GetNumVertices()  const 
                                    { 
                                        return m_numVertices;
                                    }

    private:
        long                        m_verticesAllocatedSize;
        long                        m_sizeTFANAllocatedSize;
        long                        m_numTFANs;
        long                        m_numVertices;
        long *                      m_vertices;
        long *                      m_sizeTFAN;
        
    friend class TriangleListEncoder;
    };
}
#endif // X3DGC_TRIANGLE_FANS_H

