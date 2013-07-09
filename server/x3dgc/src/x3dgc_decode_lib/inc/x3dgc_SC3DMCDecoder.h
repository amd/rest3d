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
#ifndef X3DGC_SC3DMC_H
#define X3DGC_SC3DMC_H

#include "x3dgc_Common.h"
#include "x3dgc_BinaryStream.h"
#include "x3dgc_IndexedFaceSet.h"
#include "x3dgc_SC3DMCEncodeParams.h"
#include "x3dgc_TriangleListDecoder.h"

namespace x3dgc
{    
    //! 
    class SC3DMCDecoder
    {
    public:    
        //! Constructor.
                                    SC3DMCDecoder(void)
                                    {
                                        m_iterator              = 0;
                                        m_streamSize          = 0;
                                        m_quantFloatArray     = 0;
                                        m_quantFloatArraySize = 0;
                                        m_binarization          = X3DGC_SC3DMC_UNKOWN;
                                    };
        //! Destructor.
                                    ~SC3DMCDecoder(void)
                                    {
                                        delete [] m_quantFloatArray;
                                    }
        //!
        X3DGCErrorCode              DecodeHeader(IndexedFaceSet & ifs,
                                                 const BinaryStream & bstream);
        //! 
        X3DGCErrorCode              DecodePlayload(IndexedFaceSet & ifs,
                                           const BinaryStream & bstream);

    private:
        X3DGCErrorCode              DecodeFloatArray(Real * const floatArray,
                                                     unsigned long numfloatArraySize,
                                                     unsigned long dimfloatArraySize,
                                                     const Real * const minfloatArray,
                                                     const Real * const maxfloatArray,
                                                     unsigned long nQBits,
                                                     const IndexedFaceSet & ifs,
                                                     X3DGCSC3DMCPredictionMode predMode,
                                                     const BinaryStream & bstream);
        X3DGCErrorCode              IQuantizeFloatArray(Real * const floatArray,
                                                       unsigned long numfloatArraySize,
                                                       unsigned long dimfloatArraySize,
                                                       const Real * const minfloatArray,
                                                       const Real * const maxfloatArray,
                                                       unsigned long nQBits);
        X3DGCErrorCode              DecodePredicionResidual(long & predResidual,
                                                            const BinaryStream & bstream,
                                                            bool predicted);
        X3DGCErrorCode              DecodeIntArray(long * const intArray, 
                                                   unsigned long numIntArraySize,
                                                   unsigned long dimIntArraySize,
                                                   const BinaryStream & bstream);

        unsigned long               m_iterator;
        unsigned long               m_streamSize;
        SC3DMCEncodeParams          m_params;
        TriangleListDecoder         m_triangleListDecoder;
        long *                      m_quantFloatArray;
        unsigned long               m_quantFloatArraySize;
        X3DGCSC3DMCBinarization     m_binarization;
    };


}
#endif // X3DGC_SC3DMC_H

