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
#ifndef X3DGC_VECTOR_H
#define X3DGC_VECTOR_H

#include "x3dgc_Common.h"

namespace x3dgc
{
    const size_t X3DGC_DEFAULT_VECTOR_SIZE = 32;

    //! 
    template < typename T > class Vector
    {
    public:    
        //! Constructor.
                                Vector()
                                {
                                    m_allocated = 0;
                                    m_size      = 0;
                                    m_buffer    = 0;
                                };
        //! Destructor.
                                ~Vector(void)
                                {
                                    delete [] m_buffer;
                                };
        T &                     operator[](size_t i)
                                { 
                                    return m_buffer[i];
                                }
        const T &               operator[](size_t i) const
                                { 
                                    return m_buffer[i];
                                }
        void                    Allocate(size_t size)
                                {
                                    if (size > m_allocated)
                                    {
                                        m_allocated = size;
                                        T * tmp     = new T [m_allocated];
                                        if (m_size > 0)
                                        {
                                            memcpy(tmp, m_buffer, m_size * sizeof(T) );
                                            delete [] m_buffer;
                                        }
                                        m_buffer = tmp;
                                    }
                                };
        void                    PushBack(const T & value)
                                {
                                    if (m_size == m_allocated)
                                    {
                                        m_allocated *= 2;
                                        if (m_allocated < X3DGC_DEFAULT_VECTOR_SIZE)
                                        {
                                            m_allocated = X3DGC_DEFAULT_VECTOR_SIZE;
                                        }
                                        T * tmp      = new T [m_allocated];
                                        if (m_size > 0)
                                        {
                                            memcpy(tmp, m_buffer, m_size * sizeof(T) );
                                            delete [] m_buffer;
                                        }
                                        m_buffer = tmp;
                                    }
                                    assert(m_size < m_allocated);
                                    m_buffer[m_size++] = value;
                                }
        const T * const         GetBuffer() const { return m_buffer;};
        const T *               GetBuffer()       { return m_buffer;};
        size_t                  GetSize()   const { return m_size;};
        void                    SetSize(size_t size)
                                { 
                                    assert(size <= m_allocated);
                                    m_size = size;
                                };
        size_t                  GetAllocatedSize() const { return m_allocated;};
        void                    Clear(){ m_size = 0;};

    private:
        T *                     m_buffer;
        size_t                  m_allocated;
        size_t                  m_size;
    };




    //!    Vector dim 3.
    template < typename T > class Vec3
    {
    public:
        T &                 operator[](size_t i) { return m_data[i];}
        const T      &      operator[](size_t i) const { return m_data[i];}
        T &                 X();
        T &                 Y();
        T &                 Z();
        const T      &      X() const;
        const T      &      Y() const;
        const T      &      Z() const;
        void                Normalize();
        T                   GetNorm() const;
        void                operator= (const Vec3 & rhs);
        void                operator+=(const Vec3 & rhs);
        void                operator-=(const Vec3 & rhs);
        void                operator-=(T a);
        void                operator+=(T a);
        void                operator/=(T a);
        void                operator*=(T a);
        Vec3                operator^ (const Vec3 & rhs) const;
        T                   operator* (const Vec3 & rhs) const;
        Vec3                operator+ (const Vec3 & rhs) const;
        Vec3                operator- (const Vec3 & rhs) const;
        Vec3                operator- () const;
        Vec3                operator* (T rhs) const;
        Vec3                operator/ (T rhs) const;
                            Vec3();
                            Vec3(T a);
                            Vec3(T x, T y, T z);
                            Vec3(const Vec3 & rhs);
                            ~Vec3(void);

    private:
        T                    m_data[3];
    };
    //!    Vector dim 2.
    template < typename T > class Vec2
    {
    public:
        T &                 operator[](size_t i) { return m_data[i];}
        const T &           operator[](size_t i) const { return m_data[i];}
        T &                 X();
        T &                 Y();
        const T &           X() const;
        const T &           Y() const;
        void                Normalize();
        T                   GetNorm() const;
        void                operator= (const Vec2 & rhs);
        void                operator+=(const Vec2 & rhs);
        void                operator-=(const Vec2 & rhs);
        void                operator-=(T a);
        void                operator+=(T a);
        void                operator/=(T a);
        void                operator*=(T a);
        T                   operator^ (const Vec2 & rhs) const;
        T                   operator* (const Vec2 & rhs) const;
        Vec2                operator+ (const Vec2 & rhs) const;
        Vec2                operator- (const Vec2 & rhs) const;
        Vec2                operator- () const;
        Vec2                operator* (T rhs) const;
        Vec2                operator/ (T rhs) const;
                            Vec2();
                            Vec2(T a);
                            Vec2(T x, T y);
                            Vec2(const Vec2 & rhs);
                            ~Vec2(void);

    private:
        T                   m_data[2];
    };
}
#include "x3dgc_Vector.inl"    // template implementation
#endif // X3DGC_VECTOR_H

