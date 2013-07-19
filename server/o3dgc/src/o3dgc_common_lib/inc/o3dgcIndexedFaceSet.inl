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
#ifndef O3DGC_INDEXED_FACE_SET_INL
#define O3DGC_INDEXED_FACE_SET_INL

#include <math.h>
namespace o3dgc
{
    void ComputeVectorMinMax(const Real * const tab, 
                             size_t size, 
                             size_t dim,
                             Real * minTab,
                             Real * maxTab,
                             O3DGCSC3DMCQuantizationMode quantMode)
    {
        if (size == 0 || dim == 0)
        {
            return;
        }
        size_t p = 0;    
        for(size_t d = 0; d < dim; ++d)
        {
            maxTab[d] = minTab[d] = tab[p++];
        }        
        for(size_t i = 1; i < size; ++i)
        {
            for(size_t d = 0; d < dim; ++d)
            {
                if (maxTab[d] < tab[p]) maxTab[d] = tab[p];
                if (minTab[d] > tab[p]) minTab[d] = tab[p];
                ++p;
            }
        }

        if (quantMode == O3DGC_SC3DMC_DIAG_BB)
        {
            Real diag = 0.0;
            Real r;
            for(size_t d = 0; d < dim; ++d)
            {
                r     = (maxTab[d] - minTab[d]);
                diag += r*r;
            } 
            diag = sqrt(diag);
            for(size_t d = 0; d < dim; ++d)
            {
                 maxTab[d] = minTab[d] + diag;
            } 
        }
        else if (quantMode == O3DGC_SC3DMC_MAX_ALL_DIMS)
        {            
            Real maxr = (maxTab[0] - minTab[0]);
            Real r;
            for(size_t d = 1; d < dim; ++d)
            {
                r = (maxTab[d] - minTab[d]);
                if ( r > maxr)
                {
                    maxr = r;
                }
            } 
            for(size_t d = 0; d < dim; ++d)
            {
                 maxTab[d] = minTab[d] + maxr;
            } 
        }
    }
    template <class T>
    void IndexedFaceSet<T>::ComputeMinMax(O3DGCSC3DMCQuantizationMode quantMode)
    {
        ComputeVectorMinMax(m_coord   , m_nCoord   , 3, m_coordMin   , m_coordMax   , quantMode);
        ComputeVectorMinMax(m_normal  , m_nNormal  , 3, m_normalMin  , m_normalMax  , quantMode);
        ComputeVectorMinMax(m_color   , m_nColor   , 3, m_colorMin   , m_colorMax   , quantMode);
        ComputeVectorMinMax(m_texCoord, m_nTexCoord, 2, m_texCoordMin, m_texCoordMax, quantMode);
        unsigned long numFloatAttributes = GetNumFloatAttributes();
        for(unsigned long a = 0; a < numFloatAttributes; ++a)
        {
            ComputeVectorMinMax(m_floatAttribute[a], m_nFloatAttribute[a],m_dimFloatAttribute[a],
                                m_minFloatAttribute + (a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES), 
                                m_maxFloatAttribute + (a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES), quantMode);
        }
    }
}
#endif // O3DGC_INDEXED_FACE_SET_INL
