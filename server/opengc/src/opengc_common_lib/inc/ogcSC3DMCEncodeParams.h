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
#ifndef OGC_SC3DMC_ENCODE_PARAMS_H
#define OGC_SC3DMC_ENCODE_PARAMS_H

#include "ogcCommon.h"

namespace ogc
{
    class SC3DMCEncodeParams
    {
    public:
        //! Constructor.
                                    SC3DMCEncodeParams(void)
                                    {
                                        memset(this, 0, sizeof(SC3DMCEncodeParams));
                                        m_encodeMode        = OGC_SC3DMC_TFAN;
                                        m_streamTypeMode    = OGC_SC3DMC_STREAM_TYPE_ASCII;
                                        m_coordQuantBits    = 12;
                                        m_normalQuantBits   = 10;
                                        m_colorQuantBits    = 10;
                                        m_texCoordQuantBits = 10;
                                        m_coordPredMode     = OGC_SC3DMC_PARALLELOGRAM_PREDICTION;
                                        m_texCoordPredMode  = OGC_SC3DMC_PARALLELOGRAM_PREDICTION;
                                        m_normalPredMode    = OGC_SC3DMC_DIFFERENTIAL_PREDICTION;  
                                        m_colorPredMode     = OGC_SC3DMC_DIFFERENTIAL_PREDICTION;
                                        for(unsigned long a = 0; a < OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES; ++a)
                                        {
                                            m_floatAttributePredMode[a] = OGC_SC3DMC_DIFFERENTIAL_PREDICTION;
                                        }
                                        for(unsigned long a = 0; a < OGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES; ++a)
                                        {
                                            m_intAttributePredMode[a] = OGC_SC3DMC_NO_PREDICTION;
                                        }
                                    };
        //! Destructor.
                                    ~SC3DMCEncodeParams(void) {};

        OGCSC3DMCStreamType         GetStreamType()    const { return m_streamTypeMode;}
        OGCSC3DMCEncodingMode       GetEncodeMode()    const { return m_encodeMode;}

        unsigned long               GetNFloatAttributes()  const { return m_numFloatAttributes;}
        unsigned long               GetNIntAttributes()    const { return m_numIntAttributes;}

        unsigned long               GetCoordQuantBits()    const { return m_coordQuantBits;}
        unsigned long               GetNormalQuantBits()   const { return m_normalQuantBits;}
        unsigned long               GetColorQuantBits()    const { return m_colorQuantBits;}
        unsigned long               GetTexCoordQuantBits() const { return m_texCoordQuantBits;}
        unsigned long               GetFloatAttributeQuantBits(unsigned long a) const
                                    {
                                       assert(a < OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       return m_floatAttributeQuantBits[a];
                                    }

        OGCSC3DMCPredictionMode     GetCoordPredMode()    const { return m_coordPredMode; }
        OGCSC3DMCPredictionMode     GetNormalPredMode()   const { return m_normalPredMode; }
        OGCSC3DMCPredictionMode     GetColorPredMode()    const { return m_colorPredMode; }
        OGCSC3DMCPredictionMode     GetTexCoordPredMode() const { return m_texCoordPredMode; }
        OGCSC3DMCPredictionMode     GetFloatAttributePredMode(unsigned long a) const
                                    {
                                       assert(a < OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       return m_floatAttributePredMode[a];
                                    }
        OGCSC3DMCPredictionMode     GetIntAttributePredMode(unsigned long a) const
                                    { 
                                        assert(a < OGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
                                        return m_intAttributePredMode[a];
                                    }
        void                        SetStreamType(OGCSC3DMCStreamType streamTypeMode)  { m_streamTypeMode = streamTypeMode;}
        void                        SetEncodeMode(OGCSC3DMCEncodingMode encodeMode)  { m_encodeMode = encodeMode;}
        void                        SetNFloatAttributes(unsigned long numFloatAttributes) 
                                    { 
                                        assert(numFloatAttributes < OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                        m_numFloatAttributes = numFloatAttributes;
                                    }
        void                        SetNIntAttributes  (unsigned long numIntAttributes)
                                    { 
                                        assert(numIntAttributes < OGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
                                        m_numIntAttributes   = numIntAttributes;
                                    }
        void                        SetCoordQuantBits   (unsigned int coordQuantBits   ) { m_coordQuantBits    = coordQuantBits   ; }
        void                        SetNormalQuantBits  (unsigned int normalQuantBits  ) { m_normalQuantBits   = normalQuantBits  ; }
        void                        SetColorQuantBits   (unsigned int colorQuantBits   ) { m_colorQuantBits    = colorQuantBits   ; }
        void                        SetTexCoordQuantBits(unsigned int texCoordQuantBits) { m_texCoordQuantBits = texCoordQuantBits; }
        void                        SetFloatAttributeQuantBits(unsigned long a, unsigned long q) 
                                    { 
                                       assert(a < OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       m_floatAttributeQuantBits[a] = q;
                                    }
        void                        SetCoordPredMode   (OGCSC3DMCPredictionMode coordPredMode   ) { m_coordPredMode    = coordPredMode   ; }
        void                        SetNormalPredMode  (OGCSC3DMCPredictionMode normalPredMode  ) { m_normalPredMode   = normalPredMode  ; }
        void                        SetColorPredMode   (OGCSC3DMCPredictionMode colorPredMode   ) { m_colorPredMode    = colorPredMode   ; }
        void                        SetTexCoordPredMode(OGCSC3DMCPredictionMode texCoordPredMode) { m_texCoordPredMode = texCoordPredMode; }
        void                        SetFloatAttributePredMode(unsigned long a, OGCSC3DMCPredictionMode p) 
                                    { 
                                       assert(a < OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
                                       m_floatAttributePredMode[a] = p;
                                    }                       
        void                        SetIntAttributePredMode(unsigned long a, OGCSC3DMCPredictionMode p) 
                                    { 
                                        assert(a < OGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
                                        m_intAttributePredMode[a] = p;
                                    }
    private:
        unsigned long               m_numFloatAttributes;
        unsigned long               m_numIntAttributes;
        unsigned long               m_coordQuantBits;
        unsigned long               m_normalQuantBits;
        unsigned long               m_colorQuantBits;
        unsigned long               m_texCoordQuantBits;
        unsigned long               m_floatAttributeQuantBits[OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES];
        
        OGCSC3DMCPredictionMode     m_coordPredMode;
        OGCSC3DMCPredictionMode     m_texCoordPredMode; 
        OGCSC3DMCPredictionMode     m_normalPredMode; 
        OGCSC3DMCPredictionMode     m_colorPredMode; 
        OGCSC3DMCPredictionMode     m_floatAttributePredMode[OGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES];
        OGCSC3DMCPredictionMode     m_intAttributePredMode  [OGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES];
        OGCSC3DMCStreamType         m_streamTypeMode;
        OGCSC3DMCEncodingMode       m_encodeMode;
    };
}
#endif // OGC_SC3DMC_ENCODE_PARAMS_H

