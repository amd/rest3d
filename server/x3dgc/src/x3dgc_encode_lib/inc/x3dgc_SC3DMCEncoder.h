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
#ifndef X3DGC_SC3DMC_ENCODER_H
#define X3DGC_SC3DMC_ENCODER_H

#include <x3dgc_Common.h>
#include <x3dgc_BinaryStream.h>
#include <x3dgc_IndexedFaceSet.h>
#include <x3dgc_SC3DMCEncodeParams.h>
#include <x3dgc_TriangleListEncoder.h>

namespace x3dgc
{	
	//! 
    class SC3DMCEncoder
	{
    public:	
		//! Constructor.
                                                                SC3DMCEncoder(void)
                                                                {
                                                                    m_quantFloatArray     = 0;
                                                                    m_quantFloatArraySize = 0;
																	m_binarization		  = X3DGC_SC3DMC_UNKOWN;
                                                                };
		//! Destructor.
																~SC3DMCEncoder(void)
                                                                {
                                                                    delete [] m_quantFloatArray;
                                                                }
		//! 
		X3DGCErrorCode											Encode(const SC3DMCEncodeParams & params, 
                                                                       const IndexedFaceSet & ifs, 
                                                                       BinaryStream & bstream);	

		private:
        X3DGCErrorCode											EncodeHeader(const SC3DMCEncodeParams & params, 
                                                                             const IndexedFaceSet & ifs, 
                                                                             BinaryStream & bstream);
        X3DGCErrorCode											EncodePayload(const SC3DMCEncodeParams & params, 
                                                                               const IndexedFaceSet & ifs, 
                                                                               BinaryStream & bstream);
        X3DGCErrorCode                                          EncodeFloatArray(const Real * const floatArray, 
																				 unsigned long numfloatArraySize,
																				 unsigned long dimfloatArraySize,
																				 const Real * const minfloatArray,
																				 const Real * const maxfloatArray,
																				 unsigned long nQBits,
																				 const IndexedFaceSet & ifs,
																				 X3DGCSC3DMCPredictionMode predMode,
																				 BinaryStream & bstream);
        X3DGCErrorCode                                          QuantizeFloatArray(const Real * const floatArray, 
                                                                                   unsigned long numfloatArraySize,
                                                                                   unsigned long dimfloatArraySize,
                                                                                   const Real * const minfloatArray,
                                                                                   const Real * const maxfloatArray,
                                                                                   unsigned long nQBits);
		X3DGCErrorCode											EncodeIntArray(const long * const intArray, 
																			   unsigned long numIntArraySize,
																			   unsigned long dimIntArraySize,
																			   BinaryStream & bstream);
		X3DGCErrorCode											EncodePredicionResidual(long predResidual, 
																					    BinaryStream & bstream, 
																						bool predicted);        
        TriangleListEncoder                                     m_triangleListEncoder;
        long *                                                  m_quantFloatArray;
	    unsigned long                                           m_quantFloatArraySize;
		X3DGCSC3DMCBinarization                                 m_binarization;
	};


}
#endif // X3DGC_SC3DMC_ENCODER_H

