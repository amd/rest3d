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

#include "x3dgc_TriangleListDecoder.h"

//#define DEBUG_VERBOSE

namespace x3dgc
{

#ifdef DEBUG_VERBOSE
        FILE* g_fileDebugDecTL = NULL;
#endif //DEBUG_VERBOSE

    X3DGCErrorCode TriangleListDecoder::Init(long * const  triangles,
                                             const long numTriangles,
                                             const long numVertices,
                                             const long maxSizeV2T)
    {
        assert(numVertices  > 0);
        assert(numTriangles > 0);

        m_numTriangles  = numTriangles;
        m_numVertices   = numVertices;
        m_triangles     = triangles;
        m_vertexCount   = 0;
        m_triangleCount = 0;
        m_itNumTFans    = 0;
        m_itDegree      = 0;
        m_itConfig      = 0;
        m_itOperation   = 0;
        m_itIndex       = 0;
        
        if  (m_numVertices > m_maxNumVertices)
        {
            m_maxNumVertices         = m_numVertices;
            delete [] m_visitedVerticesValence;
            delete [] m_visitedVertices;
            m_visitedVerticesValence = new long [m_numVertices];
            m_visitedVertices        = new long [m_numVertices];
        }
        
        m_ctfans.SetStreamType(m_streamType);
        m_ctfans.Allocate(m_numVertices);
        m_tfans.Allocate(2 * m_numVertices, 8 * m_numVertices);

        // compute vertex-to-triangle adjacency information
        m_vertexToTriangle.AllocateNumNeighborsArray(numVertices);
        for(long i = 0; i < numVertices; ++i)
        {
            m_vertexToTriangle.m_numNeighbors[i] = maxSizeV2T;
        }
        m_vertexToTriangle.AllocateNeighborsArray();
        m_vertexToTriangle.ClearNeighborsArray();
        return X3DGC_OK;
    }
    X3DGCErrorCode TriangleListDecoder::Decompress()
    {
#ifdef DEBUG_VERBOSE
        g_fileDebugDecTL = fopen("tfans_dec_new.txt", "w");
#endif //DEBUG_VERBOSE

        for(long focusVertex = 0; focusVertex < m_numVertices; ++focusVertex)
        {
            if (focusVertex == m_vertexCount)
            {
                m_vertexCount++; // insert focusVertex
            }
            CompueLocalConnectivityInfo(focusVertex);
            DecompressTFAN(focusVertex);
#ifdef DEBUG_VERBOSE
            fflush(g_fileDebugDecTL);
#endif //DEBUG_VERBOSE
        }
#ifdef DEBUG_VERBOSE
        fclose(g_fileDebugDecTL);
#endif //DEBUG_VERBOSE
        return X3DGC_OK;
    }
    X3DGCErrorCode TriangleListDecoder::CompueLocalConnectivityInfo(const long focusVertex)
    {
        long t = 0;
        long p, v;
        m_numConqueredTriangles    = 0;
        m_numVisitedVertices       = 0;
        for(long i = m_vertexToTriangle.Begin(focusVertex); (t >= 0) && (i < m_vertexToTriangle.End(focusVertex)); ++i)
        {
            t = m_vertexToTriangle.GetNeighbor(i);
            if ( t >= 0)
            {
                ++m_numConqueredTriangles;
                p = 3*t;
                // extract visited vertices
                for(long k = 0; k < 3; ++k)
                {
                    v = m_triangles[p+k];
                    if (v > focusVertex) // vertices are insertices by increasing traversal order
                    {
                        bool foundOrInserted = false;
                        for (long j = 0; j < m_numVisitedVertices; ++j)
                        {
                            if (v == m_visitedVertices[j])
                            {
                                m_visitedVerticesValence[j]++;
                                foundOrInserted = true;
                                break;
                            }
                            else if (v < m_visitedVertices[j])
                            {
                                ++m_numVisitedVertices;
                                for (long h = m_numVisitedVertices-1; h > j; --h)
                                {
                                    m_visitedVertices[h]        = m_visitedVertices[h-1];
                                    m_visitedVerticesValence[h] = m_visitedVerticesValence[h-1];
                                }
                                m_visitedVertices[j]        = v;
                                m_visitedVerticesValence[j] = 1;
                                foundOrInserted = true;
                                break;
                            }
                        }
                        if (!foundOrInserted)
                        {
                            m_visitedVertices[m_numVisitedVertices]        = v;
                            m_visitedVerticesValence[m_numVisitedVertices] = 1;
                            m_numVisitedVertices++;
                        }
                    }
                }
            }
        }
        // re-order visited vertices by taking into account their valence (i.e., # of conquered triangles incident to each vertex)
        // in order to avoid config. 9
        if (m_numVisitedVertices > 2)
        {
            long y;
            for(long x = 1; x < m_numVisitedVertices; ++x)
            {

                if (m_visitedVerticesValence[x] == 1)
                {
                    y = x;
                    while( (y > 0) && (m_visitedVerticesValence[y] < m_visitedVerticesValence[y-1]) )
                    {
                        swap(m_visitedVerticesValence[y], m_visitedVerticesValence[y-1]);
                        swap(m_visitedVertices[y], m_visitedVertices[y-1]);
                        --y;
                    }
                }
            }
        }
        return X3DGC_OK;
    }
    X3DGCErrorCode TriangleListDecoder::DecompressTFAN(const long focusVertex)
    {
        long ntfans; 
        long degree, config;
        long op;
        long index;
        long k0, k1;
        long b, c, t;

        ntfans = m_ctfans.ReadNumTFans(m_itNumTFans);

#ifdef DEBUG_VERBOSE
        printf("#fans %i\n", (int) ntfans);
        fprintf(g_fileDebugDecTL, "#fans %i\n", (int) ntfans);
#endif //DEBUG_VERBOSE


        if (ntfans > 0) 
        {
            for(long f = 0; f != ntfans; f++) 
            {
                m_tfans.AddTFAN();
                degree     = m_ctfans.ReadDegree(m_itDegree) +2 - m_numConqueredTriangles;
                config     = m_ctfans.ReadConfig(m_itConfig);
                k0         = m_tfans.GetNumVertices();
                m_tfans.AddVertex(focusVertex);
#ifdef DEBUG_VERBOSE
                    printf("VisitedVertices (%i) \t", focusVertex);
                    fprintf(g_fileDebugDecTL, "VisitedVertices (%i) \t", focusVertex);
                    for(long u=0; u < m_numVisitedVertices; ++u)
                    {
                        printf("%i, ", m_visitedVertices[u]);
                        fprintf(g_fileDebugDecTL, "%i, ", m_visitedVertices[u]);
                    }
                    printf("\n");
                    fprintf(g_fileDebugDecTL, "\n");
#endif //DEBUG_VERBOSE
#ifdef DEBUG_VERBOSE
                    printf("Case %i, degree= %i \n", config, degree);
                    fprintf(g_fileDebugDecTL, "Case %i, degree= %i \n", config, degree);
#endif //DEBUG_VERBOSE
                switch(config)
                {
                    case 0:// ops: 1000001 vertices: -1 -2
                        m_tfans.AddVertex(m_visitedVertices[0]);
                        for(long u = 1; u < degree-1; u++)
                        {
                            m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                            m_tfans.AddVertex(m_vertexCount++);
                        }
                        m_tfans.AddVertex(m_visitedVertices[1]);
                        break;
                    case 1: // ops: 1xxxxxx1 vertices: -1 x x x x x -2
                        m_tfans.AddVertex(m_visitedVertices[0]);
                        for(long u = 1; u < degree-1; u++)
                        {
                            op = m_ctfans.ReadOperation(m_itOperation);
                            if (op == 1) 
                            {
                                index = m_ctfans.ReadIndex(m_itIndex);
#ifdef DEBUG_VERBOSE
                                printf("index %i\n", index);
                                fprintf(g_fileDebugDecTL, "index %i\n", index);
#endif //DEBUG_VERBOSE
                                if ( index < 0) 
                                {
                                    m_tfans.AddVertex(m_visitedVertices[-index-1]);
                                }
                                else 
                                {
                                    m_tfans.AddVertex(index + focusVertex);
                                }
                            }
                            else 
                            {
                                m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                                m_tfans.AddVertex(m_vertexCount++);
                            }
                        }
                        m_tfans.AddVertex(m_visitedVertices[1]);
                        break;
                    case 2: // ops: 00000001 vertices: -1
                        for(long u = 0; u < degree-1; u++)
                        {
                            m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                            m_tfans.AddVertex(m_vertexCount++);
                        }
                        m_tfans.AddVertex(m_visitedVertices[0]);
                        break;
                    case 3: // ops: 00000001 vertices: -2
                        for(long u=0; u < degree-1; u++)
                        {
                            m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                            m_tfans.AddVertex(m_vertexCount++);
                        }
                        m_tfans.AddVertex(m_visitedVertices[1]);
                        break;
                    case 4: // ops: 10000000 vertices: -1
                        m_tfans.AddVertex(m_visitedVertices[0]);
                        for(long u = 1; u < degree; u++)
                        {
                            m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                            m_tfans.AddVertex(m_vertexCount++);
                        }
                        break;
                    case 5: // ops: 10000000 vertices: -2
                        m_tfans.AddVertex(m_visitedVertices[1]);
                        for(long u = 1; u < degree; u++)
                        {
                            m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                            m_tfans.AddVertex(m_vertexCount++);
                        }
                        break;
                    case 6:// ops: 00000000 vertices:
                        for(long u = 0; u < degree; u++)
                        {
                            m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                            m_tfans.AddVertex(m_vertexCount++);
                        }
                        break;
                    case 7: // ops: 1000001 vertices: -2 -1
                        m_tfans.AddVertex(m_visitedVertices[1]);
                        for(long u = 1; u < degree-1; u++)
                        {
                            m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                            m_tfans.AddVertex(m_vertexCount++);
                        }
                        m_tfans.AddVertex(m_visitedVertices[0]);
                        break;
                    case 8: // ops: 1xxxxxx1 vertices: -2 x x x x x -1
                        m_tfans.AddVertex(m_visitedVertices[1]);
                        for(long u = 1; u < degree-1; u++)
                        {
                            op = m_ctfans.ReadOperation(m_itOperation);
                            if (op == 1) 
                            {
                                index = m_ctfans.ReadIndex(m_itIndex);
#ifdef DEBUG_VERBOSE
                                printf("index %i\n", index);
                                fprintf(g_fileDebugDecTL, "index %i\n", index);
#endif //DEBUG_VERBOSE
                                if ( index < 0) 
                                {
                                    m_tfans.AddVertex(m_visitedVertices[-index-1]);
                                }
                                else 
                                {
                                    m_tfans.AddVertex(index + focusVertex);
                                }
                            }
                            else 
                            {
                                m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                                m_tfans.AddVertex(m_vertexCount++);
                            }
                        }
                        m_tfans.AddVertex(m_visitedVertices[0]);
                        break;
                    case 9: // general case
                        for(long u = 0; u < degree; u++)
                        {
                            op = m_ctfans.ReadOperation(m_itOperation);
                            if (op == 1) 
                            {
                                index = m_ctfans.ReadIndex(m_itIndex);
#ifdef DEBUG_VERBOSE
                                printf("index %i\n", index);
                                fprintf(g_fileDebugDecTL, "index %i\n", index);
#endif //DEBUG_VERBOSE
                                if ( index < 0) 
                                {
                                    m_tfans.AddVertex(m_visitedVertices[-index-1]);
                                }
                                else 
                                {
                                    m_tfans.AddVertex(index + focusVertex);
                                }
                            }
                            else 
                            {
                                m_visitedVertices[m_numVisitedVertices++] = m_vertexCount;
                                m_tfans.AddVertex(m_vertexCount++);
                            }
                        }
                        break;

                }
                //logger.write_2_log("\t degree=%i \t cas = %i\n", degree, cas);
                k1 = m_tfans.GetNumVertices();
                b  = m_tfans.GetVertex(k0+1);
                for (long k = k0+2; k < k1; k++)
                {
                    c = m_tfans.GetVertex(k);
                    t = m_triangleCount*3;
                 
                    m_triangles[t++] = focusVertex;
                    m_triangles[t++] = b;
                    m_triangles[t  ] = c;

#ifdef DEBUG_VERBOSE
                    printf("tri -> %i, %i, %i\n", focusVertex, b, c);
                    fprintf(g_fileDebugDecTL, "tri -> %i, %i, %i\n", focusVertex, b, c);
                    fflush(g_fileDebugDecTL);
#endif //DEBUG_VERBOSE

                    m_vertexToTriangle.AddNeighbor(focusVertex, m_triangleCount);
                    m_vertexToTriangle.AddNeighbor(b          , m_triangleCount);
                    m_vertexToTriangle.AddNeighbor(c          , m_triangleCount);
                    b=c;
                    m_triangleCount++;
                }
            }
        }
        return X3DGC_OK;
    }
}

