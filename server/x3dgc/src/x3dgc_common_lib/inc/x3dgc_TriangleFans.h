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

    class CompressedTriangleFans
    {
    public:    
        //! Constructor.
                                    CompressedTriangleFans(void)
                                    {
                                        m_streamType   = X3DGC_SC3DMC_STREAM_TYPE_UNKOWN;
                                        m_bufferAC     = 0;
                                        m_sizeBufferAC = 0;
                                    };
        //! Destructor.
                                    ~CompressedTriangleFans(void) 
                                    {
                                        delete [] m_bufferAC;
                                    };
        X3DGCSC3DMCStreamType     GetStreamType() const { return m_streamType; }
        void                        SetStreamType(X3DGCSC3DMCStreamType streamType) { m_streamType = streamType; }

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
                                         m_numTFANs.PushBack(numTFans);
                                        return X3DGC_OK;
                                    }
        long                        ReadNumTFans(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_numTFANs.GetSize());
                                        return m_numTFANs[iterator++];
                                    }
        X3DGCErrorCode              PushDegree(long degree)
                                    {
                                        m_degrees.PushBack(degree);
                                        return X3DGC_OK;
                                    }
        long                        ReadDegree(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_degrees.GetSize());
                                        return m_degrees[iterator++];
                                    }
        X3DGCErrorCode              PushConfig(long config)
                                    {
                                        m_configs.PushBack(config);
                                        return X3DGC_OK;
                                    }
        long                        ReadConfig(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_configs.GetSize());
                                        return m_configs[iterator++];
                                    }
        X3DGCErrorCode              PushOperation(long op)
                                    {
                                        m_operations.PushBack(op);
                                        return X3DGC_OK;
                                    }
        long                        ReadOperation(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_operations.GetSize());
                                        return m_operations[iterator++];
                                    }
        X3DGCErrorCode              PushIndex(long index)
                                    {
                                        m_indices.PushBack(index);
                                        return X3DGC_OK;
                                    }
        long                        ReadIndex(unsigned long & iterator) const
                                    {
                                        assert(iterator < m_indices.GetSize());
                                        return m_indices[iterator++];
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
        X3DGCErrorCode              Save(BinaryStream & bstream,
                                         X3DGCSC3DMCStreamType streamType);
        X3DGCErrorCode              Load(const BinaryStream & bstream, 
                                         unsigned long & iterator, 
                                         X3DGCSC3DMCStreamType streamType);

    private:
        X3DGCErrorCode              SaveBinAC(const Vector<long> & data,
                                              BinaryStream & bstream);
        X3DGCErrorCode              SaveUIntAC(const Vector<long> & data,
                                               const unsigned long M,
                                               BinaryStream & bstream);
        X3DGCErrorCode              SaveIntACEGC(const Vector<long> & data,
                                                 const unsigned long M,
                                                 BinaryStream & bstream);

        Vector<long>                m_numTFANs;
        Vector<long>                m_degrees;
        Vector<long>                m_configs;
        Vector<long>                m_operations;
        Vector<long>                m_indices;
        unsigned char *             m_bufferAC;
        unsigned long               m_sizeBufferAC;
        X3DGCSC3DMCStreamType       m_streamType;
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

