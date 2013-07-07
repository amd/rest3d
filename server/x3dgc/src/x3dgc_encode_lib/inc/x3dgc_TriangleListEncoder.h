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
#ifndef X3DGC_TRIANGLE_LIST_ENCODER_H
#define X3DGC_TRIANGLE_LIST_ENCODER_H

#include <x3dgc_Common.h>
#include <x3dgc_AdjacencyInfo.h>
#include <x3dgc_BinaryStream.h>
#include <x3dgc_FIFO.h>
#include <x3dgc_TriangleFans.h>

namespace x3dgc
{
	
	//! 
    class TriangleListEncoder
	{
    public:	
		//! Constructor.
																TriangleListEncoder(void);
		//! Destructor.
																~TriangleListEncoder(void);
		//! 
		X3DGCErrorCode											Encode(const long * const triangles, 
																	   const long numTriangles,
																	   const long numVertices,                                                                       
																	   BinaryStream & bstream);	
        X3DGCSC3DMCBinarization                                 GetBinarization() const { return m_binarization; }
        void                                                    SetBinarization(X3DGCSC3DMCBinarization binarization) { m_binarization = binarization; }
		const long * const                                      GetInvVMap() const { return m_invVMap;}
        const long * const                                      GetVMap()    const { return m_vmap;}
        const long * const                                      GetTMap()    const { return m_tmap;}
        const AdjacencyInfo &									GetVertexToTriangle() const { return m_vertexToTriangle;}

		private:
		X3DGCErrorCode											Init(const long * const triangles, 
																 	  long numTriangles, 
																	  long numVertices);

		X3DGCErrorCode											CompueLocalConnectivityInfo(const long focusVertex);

		X3DGCErrorCode											ProcessVertex( long focusVertex);
		X3DGCErrorCode											ComputeTFANDecomposition(const long focusVertex);
		X3DGCErrorCode											CompressTFAN(const long focusVertex);

		long													m_vertexCount;	
		long													m_triangleCount;
		long													m_maxNumVertices;
		long													m_maxNumTriangles;
		long													m_numNonConqueredTriangles;
		long													m_numConqueredTriangles;
		long													m_numVisitedVertices;
		long													m_numTriangles;
		long													m_numVertices;
        long                                                    m_maxSizeVertexToTriangle;
		long const *											m_triangles;
		long *													m_vtags;
		long *													m_ttags;
		long *													m_vmap;
		long *													m_invVMap;
		long *													m_tmap;
		long *													m_nonConqueredTriangles;
		long *													m_nonConqueredEdges;
		long *													m_visitedVertices;
		long *													m_visitedVerticesValence;
		FIFO<long>												m_vfifo;
		AdjacencyInfo											m_vertexToTriangle;
		AdjacencyInfo											m_triangleToTriangle;
		AdjacencyInfo											m_triangleToTriangleInv;
		TriangleFans											m_tfans;
		CompressedTriangleFans									m_ctfans;
        X3DGCSC3DMCBinarization                                 m_binarization;
	};


}
#endif // X3DGC_TRIANGLE_LIST_ENCODER_H