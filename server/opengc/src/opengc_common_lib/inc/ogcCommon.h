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
#ifndef OGC_COMMON_H
#define OGC_COMMON_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace ogc
{
    typedef float Real;

    const long OGC_MIN_LONG      = -2147483647;
    const long OGC_MAX_LONG      =  2147483647;
    const long OGC_MAX_UCHAR8    = 255;
    const long OGC_MAX_TFAN_SIZE = 256;

    const unsigned long OGC_SC3DMC_START_CODE               = 0x00001F1;
    const unsigned long OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES = 256;
    const unsigned long OGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES   = 256;
    const unsigned long OGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES = 8;
    const unsigned long OGC_SC3DMC_MAX_DIM_INT_ATTRIBUTES   = 8;

    enum OGCEndianness
    {
        OGC_BIG_ENDIAN     = 0,
        OGC_LITTLE_ENDIAN  = 1
    };
    enum OGCErrorCode
    {
        OGC_OK,
        OGC_ERROR_BUFFER_FULL,
        OGC_ERROR_CREATE_FILE,
        OGC_ERROR_OPEN_FILE,
        OGC_ERROR_READ_FILE,
        OGC_ERROR_CORRUPTED_STREAM
    };
    enum OGCSC3DMCBinarization
    {
        OGC_SC3DMC_BINARIZATION_FL     = 0,            // Fixed Length (not supported)
        OGC_SC3DMC_BINARIZATION_BP     = 1,            // BPC (not supported)
        OGC_SC3DMC_BINARIZATION_FC     = 2,            // 4 bits Coding (not supported)
        OGC_SC3DMC_BINARIZATION_AC     = 3,            // Arithmetic Coding (not supported)
        OGC_SC3DMC_BINARIZATION_AC_EGC = 4,            // Arithmetic Coding & EGCk
        OGC_SC3DMC_BINARIZATION_ASCII  = 5             // Arithmetic Coding & EGCk
    };
    enum OGCSC3DMCStreamType
    {
        OGC_SC3DMC_STREAM_TYPE_UNKOWN = 0,
        OGC_SC3DMC_STREAM_TYPE_ASCII  = 1,
        OGC_SC3DMC_STREAM_TYPE_BINARY = 2
    };


    enum OGCSC3DMCQuantizationMode
    {
        OGC_SC3DMC_DIAG_BB             = 0, // supported
        OGC_SC3DMC_MAX_ALL_DIMS        = 1, // supported
        OGC_SC3DMC_MAX_SEP_DIM         = 2  // supported
    };
    enum OGCSC3DMCPredictionMode
    {
        OGC_SC3DMC_NO_PREDICTION                   = 0, // supported
        OGC_SC3DMC_DIFFERENTIAL_PREDICTION         = 1, // supported
//      OGC_SC3DMC_XORPrediction                  = 2, // not supported
//      OGC_SC3DMC_AdaptiveDifferentialPrediction = 3, // not supported
//      OGC_SC3DMC_CircularDifferentialPrediction = 4, // not supported
        OGC_SC3DMC_PARALLELOGRAM_PREDICTION        = 5  // supported
    };
    enum OGCSC3DMCEncodingMode
    {
        OGC_SC3DMC_QBCR       = 0,        // not supported
        OGC_SC3DMC_SVA        = 1,        // not supported
        OGC_SC3DMC_TFAN       = 2,        // supported
    };    
    template<class T> 
    inline const T& min(const T& a, const T& b)
    {
        return (b < a) ? b : a;
    }
    template<class T> 
    inline const T& max(const T& a, const T& b)
    {
        return (b > a) ? b : a;
    }
    template<class T> 
    inline void swap(T& a, T& b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }
    inline OGCEndianness SystemEndianness()
    {
        unsigned long num = 1;
        return ( *((char *)(&num)) == 1 )? OGC_LITTLE_ENDIAN : OGC_BIG_ENDIAN ;
    }
}
#endif // OGC_COMMON_H

