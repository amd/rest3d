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
#ifndef X3DGC_SC3DMC_ENCODE_PARAMS_H
#define X3DGC_SC3DMC_ENCODE_PARAMS_H

#include "x3dgc_Common.h"

namespace x3dgc
{
    class SC3DMCEncodeParams
    {
    public:
        //! Constructor.
                                    SC3DMCEncodeParams(void)
                                    {
                                        memset(this, 0, sizeof(SC3DMCEncodeParams));
                                        m_encodeMode        = X3DGC_SC3DMC_TFAN;
                                        m_binarizationMode  = X3DGC_SC3DMC_GZIP;
                                        m_coordQuantBits    = 12;
                                        m_normalQuantBits   = 10;
                                        m_colorQuantBits    = 10;
                                        m_texCoordQuantBits = 10;
                                        m_coordPredMode     = X3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
                                        m_texCoordPredMode  = X3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
                                        m_normalPredMode    = X3DGC_SC3DMC_DIFFERENTIAL_PREDICTION;  
                                        m_colorPredMode     = X3DGC_SC3DMC_DIFFERENTIAL_PREDICTION;
                                        for(unsigned long a = 0; a < X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES; ++a)
                                        {
                                            m_floatAttributePredMode[a] = X3DGC_SC3DMC_DIFFERENTIAL_PREDICTION;
                                        }
                                        for(unsigned long a = 0; a < X3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES; ++a)
                                        {
                                            m_intAttributePredMode[a] = X3DGC_SC3DMC_NO_PREDICTION;
                                        }
                                    };
        //! Destructor.
                                    ~SC3DMCEncodeParams(void) {};

        X3DGCSC3DMCBinarization     GetBinarization()  const { return m_binarizationMode;}
        X3DGCSC3DMCEncodingMode     GetEncodeMode()    const { return m_encodeMode;}

        unsigned long               GetNFloatAttributes()  const { return m_numFloatAttributes;}
        unsigned long               GetNIntAttributes()    const { return m_numIntAttributes;}

        unsigned long               GetCoordQuantBits()    const { return m_coordQuantBits;}
        unsigned long               GetNormalQuantBits()   const { return m_normalQuantBits;}
        unsigned long               GetColorQuantBits()    const { return m_colorQuantBits;}
        unsigned long               GetTexCoordQuantBits() const { return m_texCoordQuantBits;}
        unsigned long               GetFloatAttributeQuantBits(unsigned long a) const
                                    {
                                       assert(a < X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       return m_floatAttributeQuantBits[a];
                                    }

        X3DGCSC3DMCPredictionMode   GetCoordPredMode()    const { return m_coordPredMode; }
        X3DGCSC3DMCPredictionMode   GetNormalPredMode()   const { return m_normalPredMode; }
        X3DGCSC3DMCPredictionMode   GetColorPredMode()    const { return m_colorPredMode; }
        X3DGCSC3DMCPredictionMode   GetTexCoordPredMode() const { return m_texCoordPredMode; }
        X3DGCSC3DMCPredictionMode   GetFloatAttributePredMode(unsigned long a) const
                                    {
                                       assert(a < X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       return m_floatAttributePredMode[a];
                                    }
        X3DGCSC3DMCPredictionMode   GetIntAttributePredMode(unsigned long a) const
                                    { 
                                        assert(a < X3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
                                        return m_intAttributePredMode[a];
                                    }
        void                        SetBinarization(X3DGCSC3DMCBinarization binarizationMode)  { m_binarizationMode = binarizationMode;}
        void                        SetEncodeMode(X3DGCSC3DMCEncodingMode encodeMode)  { m_encodeMode = encodeMode;}
        void                        SetNFloatAttributes(unsigned long numFloatAttributes) 
                                    { 
                                        assert(numFloatAttributes < X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                        m_numFloatAttributes = numFloatAttributes;
                                    }
        void                        SetNIntAttributes  (unsigned long numIntAttributes)
                                    { 
                                        assert(numIntAttributes < X3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
                                        m_numIntAttributes   = numIntAttributes;
                                    }
        void                        SetCoordQuantBits   (unsigned int coordQuantBits   ) { m_coordQuantBits    = coordQuantBits   ; }
        void                        SetNormalQuantBits  (unsigned int normalQuantBits  ) { m_normalQuantBits   = normalQuantBits  ; }
        void                        SetColorQuantBits   (unsigned int colorQuantBits   ) { m_colorQuantBits    = colorQuantBits   ; }
        void                        SetTexCoordQuantBits(unsigned int texCoordQuantBits) { m_texCoordQuantBits = texCoordQuantBits; }
        void                        SetFloatAttributeQuantBits(unsigned long a, unsigned long q) 
                                    { 
                                       assert(a < X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       m_floatAttributeQuantBits[a] = q;
                                    }
        void                        SetCoordPredMode   (X3DGCSC3DMCPredictionMode coordPredMode   ) { m_coordPredMode    = coordPredMode   ; }
        void                        SetNormalPredMode  (X3DGCSC3DMCPredictionMode normalPredMode  ) { m_normalPredMode   = normalPredMode  ; }
        void                        SetColorPredMode   (X3DGCSC3DMCPredictionMode colorPredMode   ) { m_colorPredMode    = colorPredMode   ; }
        void                        SetTexCoordPredMode(X3DGCSC3DMCPredictionMode texCoordPredMode) { m_texCoordPredMode = texCoordPredMode; }
        void                        GetFloatAttributePredMode(unsigned long a, X3DGCSC3DMCPredictionMode p) 
                                    { 
                                       assert(a < X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       m_floatAttributePredMode[a] = p;
                                    }                       
        void                        GetIntAttributePredMode(unsigned long a, X3DGCSC3DMCPredictionMode p) 
                                    { 
                                        assert(a < X3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
                                        m_intAttributePredMode[a] = p;
                                    }
    private:
        unsigned long               m_numFloatAttributes;
        unsigned long               m_numIntAttributes;
        unsigned long               m_coordQuantBits;
        unsigned long               m_normalQuantBits;
        unsigned long               m_colorQuantBits;
        unsigned long               m_texCoordQuantBits;
        unsigned long               m_floatAttributeQuantBits[X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES];
        
        X3DGCSC3DMCPredictionMode   m_coordPredMode;
        X3DGCSC3DMCPredictionMode   m_texCoordPredMode; 
        X3DGCSC3DMCPredictionMode   m_normalPredMode; 
        X3DGCSC3DMCPredictionMode   m_colorPredMode; 
        X3DGCSC3DMCPredictionMode   m_floatAttributePredMode[X3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES];
        X3DGCSC3DMCPredictionMode   m_intAttributePredMode  [X3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES];
        X3DGCSC3DMCBinarization     m_binarizationMode;
        X3DGCSC3DMCEncodingMode     m_encodeMode;
    };
}
#endif // X3DGC_SC3DMC_ENCODE_PARAMS_H

