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
#ifndef O3DGC_SC3DMC_ENCODER_H
#define O3DGC_SC3DMC_ENCODER_H

#include "o3dgcCommon.h"
#include "o3dgcBinaryStream.h"
#include "o3dgcIndexedFaceSet.h"
#include "o3dgcSC3DMCEncodeParams.h"
#include "o3dgcTriangleListEncoder.h"

namespace o3dgc
{    
    //! 
    template<class T>
    class SC3DMCEncoder
    {
    public:    
        //! Constructor.
                                    SC3DMCEncoder(void)
                                    {
                                        m_quantFloatArray     = 0;
                                        m_quantFloatArraySize = 0;
                                        m_sizeBufferAC        = 0;
                                        m_bufferAC            = 0;
                                        m_streamType          = O3DGC_SC3DMC_STREAM_TYPE_UNKOWN;
                                    };
        //! Destructor.
                                    ~SC3DMCEncoder(void)
                                    {
                                        delete [] m_quantFloatArray;
                                        delete [] m_bufferAC;
                                    }
        //! 
        O3DGCErrorCode              Encode(const SC3DMCEncodeParams & params, 
                                           const IndexedFaceSet<T> & ifs, 
                                           BinaryStream & bstream);    

        private:
        O3DGCErrorCode              EncodeHeader(const SC3DMCEncodeParams & params, 
                                                 const IndexedFaceSet<T> & ifs, 
                                                 BinaryStream & bstream);
        O3DGCErrorCode              EncodePayload(const SC3DMCEncodeParams & params, 
                                                  const IndexedFaceSet<T> & ifs, 
                                                  BinaryStream & bstream);
        O3DGCErrorCode              EncodeFloatArray(const Real * const floatArray, 
                                                     unsigned long numfloatArray,
                                                     unsigned long dimfloatArray,
                                                     const Real * const minfloatArray,
                                                     const Real * const maxfloatArray,
                                                     unsigned long nQBits,
                                                     const IndexedFaceSet<T> & ifs,
                                                     O3DGCSC3DMCPredictionMode predMode,
                                                     BinaryStream & bstream);
        O3DGCErrorCode              QuantizeFloatArray(const Real * const floatArray, 
                                                       unsigned long numFloatArray,
                                                       unsigned long dimFloatArray,
                                                       const Real * const minfloatArray,
                                                       const Real * const maxfloatArray,
                                                       unsigned long nQBits);
        O3DGCErrorCode              EncodeIntArray(const long * const intArray, 
                                                   unsigned long numIntArray,
                                                   unsigned long dimIntArray,
                                                   O3DGCSC3DMCPredictionMode predMode,
                                                   BinaryStream & bstream);
        TriangleListEncoder<T>      m_triangleListEncoder;
        long *                      m_quantFloatArray;
        unsigned long               m_quantFloatArraySize;
        unsigned char *             m_bufferAC;
        unsigned long               m_sizeBufferAC;
        O3DGCSC3DMCStreamType       m_streamType;
    };
}
#include "o3dgcSC3DMCEncoder.inl"    // template implementation
#endif // O3DGC_SC3DMC_ENCODER_H

