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

#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "o3dgcVector.h"
#include "o3dgcSC3DMCEncodeParams.h"
#include "o3dgcIndexedFaceSet.h"
#include "o3dgcSC3DMCEncoder.h"
#include "o3dgcSC3DMCDecoder.h"


using namespace o3dgc;

#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

class IVec3Cmp 
{
   public:
      bool operator()(const Vec3<long> a,const Vec3<long> b) 
      { 
          if (a.X() != b.X())
          {
              return (a.X() > b.X());
          }
          else if (a.Y() != b.Y())
          {
              return (a.Y() > b.Y());
          }
          return (a.Z() > b.Z());
      }
};



bool LoadOBJ(const std::string & fileName, 
             std::vector< Vec3<Real> > & points,
             std::vector< Vec2<Real> > & texCoords,
             std::vector< Vec3<Real> > & normals,
             std::vector< Vec3<long> > & triangles);

bool SaveOBJ(const char * fileName, 
             const std::vector< Vec3<Real> > & points,
             const std::vector< Vec2<Real> > & texCoords,
             const std::vector< Vec3<Real> > & normals,
             const std::vector< Vec3<long> > & triangles);

int testEncode(const std::string fileName, int qcoord, int qtexCoord, int qnormal, O3DGCSC3DMCStreamType streamType)
{
    std::string folder;
    long found = fileName.find_last_of(PATH_SEP);
    if (found != -1)
    {
        folder = fileName.substr(0,found);
    }
    if (folder == "")
    {
        folder = ".";
    }
    std::string file(fileName.substr(found+1));
    std::string outFileName = folder + PATH_SEP + file.substr(0, file.find_last_of(".")) + ".s3d";
    std::vector< Vec3<Real> > points;
    std::vector< Vec3<Real> > normals;
    std::vector< Vec2<Real> > texCoords;
    std::vector< Vec3<long> > triangles;
    bool ret = LoadOBJ(fileName, points, texCoords, normals, triangles);
    if (!ret)
    {
        std::cout << "Error: LoadOBJ()\n" << std::endl;
        return -1;
    }
    if (points.size() == 0 || triangles.size() == 0)
    {
        std::cout <<  "Error: points.size() == 0 || triangles.size() == 0 \n" << std::endl;
        return -1;
    }

    SC3DMCEncodeParams params;
    params.SetStreamType(streamType);
    IndexedFaceSet ifs;
    params.SetCoordQuantBits(qcoord);
    params.SetNormalQuantBits(qnormal);
    params.SetTexCoordQuantBits(qtexCoord);

    ifs.SetNCoord(points.size());
    ifs.SetNNormal(normals.size());
    ifs.SetNTexCoord(texCoords.size());
    ifs.SetNCoordIndex(triangles.size());

    ifs.SetCoord((Real * const) & (points[0]));
    ifs.SetCoordIndex((long * const ) &(triangles[0]));
    if (normals.size() > 0)
    {
        ifs.SetNormal((Real * const) & (normals[0]));
    }
    if (texCoords.size() > 0)
    {
        ifs.SetTexCoord((Real * const ) & (texCoords[0]));
    }

    // compute min/max
    ifs.ComputeMinMax(O3DGC_SC3DMC_MAX_ALL_DIMS); // O3DGC_SC3DMC_DIAG_BB

    BinaryStream bstream(points.size()*8);

    SC3DMCEncoder encoder;
    encoder.Encode(params, ifs, bstream);

    FILE * fout = fopen(outFileName.c_str(), "wb");
    if (!fout)
    {
        return -1;
    }
    fwrite(bstream.GetBuffer(), 1, bstream.GetSize(), fout);
    fclose(fout);
    return 0;
}
int testDecode(std::string fileName)
{
    std::string folder;
    long found = fileName.find_last_of(PATH_SEP);
    if (found != -1)
    {
        folder = fileName.substr(0,found);
    }
    if (folder == "")
    {
        folder = ".";
    }
    std::string file(fileName.substr(found+1));
    std::string outFileName = folder + PATH_SEP + file.substr(0, file.find_last_of(".")) + "_dec.obj";

    std::vector< Vec3<Real> > points;
    std::vector< Vec3<Real> > normals;
    std::vector< Vec2<Real> > colors;
    std::vector< Vec2<Real> > texCoords;
    std::vector< Vec3<long> > triangles;

    BinaryStream bstream;
    IndexedFaceSet ifs;


    FILE * fin = fopen(fileName.c_str(), "rb");
    if (!fin)
    {
        return -1;
    }
    fseek(fin, 0, SEEK_END);
    unsigned long size = ftell(fin);
    bstream.Allocate(size);
    rewind(fin);
    unsigned long nread = fread((void *) bstream.GetBuffer(), 1, size, fin);
    bstream.SetSize(size);
    if (nread != size)
    {
        return -1;
    }
    fclose(fin);

//    bstream.Load(fileName.c_str());

    SC3DMCDecoder decoder;
    
    // load header
    decoder.DecodeHeader(ifs, bstream);

    // allocate memory
    triangles.resize(ifs.GetNCoordIndex());
    ifs.SetCoordIndex((long * const ) &(triangles[0]));    

    points.resize(ifs.GetNCoord());
    ifs.SetCoord((Real * const ) &(points[0]));    

    if (ifs.GetNNormal() > 0)
    {
        normals.resize(ifs.GetNNormal());
        ifs.SetNormal((Real * const ) &(normals[0]));  
    }
    if (ifs.GetNColor() > 0)
    {
        colors.resize(ifs.GetNColor());
        ifs.SetColor((Real * const ) &(colors[0]));  
    }
    if (ifs.GetNTexCoord() > 0)
    {
        texCoords.resize(ifs.GetNTexCoord());
        ifs.SetTexCoord((Real * const ) &(texCoords[0]));
    }

    // decode mesh
    decoder.DecodePlayload(ifs, bstream);
    int ret = SaveOBJ(outFileName.c_str(), points, texCoords, normals, triangles);
    if (!ret)
    {
        std::cout << "Error: SaveOBJ()\n" << std::endl;
        return -1;
    }
    return 0;
}

enum Mode
{
    UNKNOWN = 0,
    ENCODE  = 1,
    DECODE  = 2
};

int main(int argc, char * argv[])
{
    Mode mode = UNKNOWN;
    std::string inputFileName;
    int qcoord    = 12;
    int qtexCoord = 10;
    int qnormal   = 10;
    O3DGCSC3DMCStreamType streamType = O3DGC_SC3DMC_STREAM_TYPE_BINARY;
    for(int i = 1; i < argc; ++i)
    {
        if ( !strcmp(argv[i], "-c"))
        {
            mode = ENCODE;
        }
        else if ( !strcmp(argv[i], "-d"))
        {
            mode = DECODE;
        }
        else if ( !strcmp(argv[i], "-i"))
        {
            ++i;
            if (i < argc)
            {
                inputFileName = argv[i];
            }
        }
        else if ( !strcmp(argv[i], "-qc"))
        {
            ++i;
            if (i < argc)
            {
                qcoord = atoi(argv[i]);
            }
        }
        else if ( !strcmp(argv[i], "-qn"))
        {
            ++i;
            if (i < argc)
            {
                qnormal = atoi(argv[i]);
            }
        }
        else if ( !strcmp(argv[i], "-qt"))
        {
            ++i;
            if (i < argc)
            {
                qtexCoord = atoi(argv[i]);
            }
        }
        else if ( !strcmp(argv[i], "-st"))
        {
            ++i;
            if (i < argc)
            {
                if (!strcmp(argv[i], "ascii"))
                {
                    streamType = O3DGC_SC3DMC_STREAM_TYPE_ASCII;
                }
            }
        }
    }

    if (inputFileName.size() == 0 || mode == UNKNOWN)
    {
        std::cout << "Usage: ./test_o3dgc [-c|d] [-qc QuantBits] [-qt QuantBits] [-qn QuantBits] -i fileName.obj "<< std::endl;
        std::cout << "\t -c \t Encode"<< std::endl;
        std::cout << "\t -d \t Decode"<< std::endl;
        std::cout << "\t -qc \t Quantization bits for positions (default=11, range = {8,...,15})"<< std::endl;
        std::cout << "\t -qn \t Quantization bits for normals (default=10, range = {8,...,15})"<< std::endl;
        std::cout << "\t -qt \t Quantization bits for texture coordinates (default=10, range = {8,...,15})"<< std::endl;
        std::cout << "\t -st \t Stream type (default=Bin, range = {binary, ascii})"<< std::endl;
        std::cout << "Examples:"<< std::endl;
        std::cout << "\t Encode binary: test_o3dgc -c -i fileName.obj -st binary"<< std::endl;
        std::cout << "\t Encode ascii:  test_o3dgc -c -i fileName.obj -st ascii "<< std::endl;
        std::cout << "\t Decode:        test_o3dgc -d -i fileName.s3d"<< std::endl;
        return -1;
    }

    std::cout << "----------------------------------------"<< inputFileName << std::endl;
    std::cout << "Encode Parameters "<< inputFileName << std::endl;
    std::cout << "   Input           \t "<< inputFileName << std::endl;

    int ret;
    if (mode == ENCODE)
    {
        std::cout << "   Coord Quant.    \t "<< qcoord << std::endl;
        std::cout << "   Normal Quant.   \t "<< qnormal << std::endl;
        std::cout << "   TexCoord Quant. \t "<< qtexCoord << std::endl;
        std::cout << "   Stream Type     \t "<< ((streamType == O3DGC_SC3DMC_STREAM_TYPE_ASCII)? "ASCII" : "Binary") << std::endl;
        ret = testEncode(inputFileName, qcoord, qtexCoord, qnormal, streamType);
    }
    else
    {
        ret = testDecode(inputFileName);
    }
    if (ret)
    {
        std::cout << "Error " << ret << std::endl;
        return ret;
    }
    return 0;
}
bool LoadOBJ(const std::string & fileName, 
             std::vector< Vec3<Real> > & upoints,
             std::vector< Vec2<Real> > & utexCoords,
             std::vector< Vec3<Real> > & unormals,
             std::vector< Vec3<long> > & triangles) 
{   
    const char ObjDelimiters[]=" /";
    const size_t BufferSize = 1024;
    FILE * fid = fopen(fileName.c_str(), "r");
    
    if (fid) 
    {        
        char buffer[BufferSize];
        Real  x[3];
        long ip[3] = {-1, -1, -1};
        long in[3] = {-1, -1, -1};
        long it[3] = {-1, -1, -1};
        char * pch;
        char * str;
        long nv = 0;
        Vec3<long> vertex;
        Vec3<long> triangle;
        std::vector< Vec3<Real> > points;
        std::vector< Vec2<Real> > texCoords;
        std::vector< Vec3<Real> > normals;
        std::map< Vec3<long>, long, IVec3Cmp > vertices;

        while (!feof(fid)) 
        {
            fgets(buffer, BufferSize, fid);
            if (buffer[0] == 'v')
            {
                if (buffer[1] == ' ')
                {                    
                    str = buffer+2;
                    for(int k = 0; k < 3; ++k)
                    {
                        pch = strtok (str, ObjDelimiters);
                        if (pch) x[k] = (Real) atof(pch);
                        else      return false;
                        str = NULL;
                    }
                    points.push_back( Vec3<Real>(x[0], x[1], x[2]) );
                }
                else if (buffer[1] == 'n')
                {
                    str = buffer+2;
                    for(int k = 0; k < 3; ++k)
                    {
                        pch = strtok (str, ObjDelimiters);
                        if (pch) x[k] = (Real) atof(pch);
                        else      return false;
                        str = NULL;
                    }
                    normals.push_back( Vec3<Real>(x[0], x[1], x[2]) );
                }
                else if (buffer[1] == 't')
                {
                    str = buffer+2;
                    for(int k = 0; k < 2; ++k)
                    {
                        pch = strtok (str, ObjDelimiters);
                        if (pch) x[k] = (Real) atof(pch);
                        else      return false;
                        str = NULL;
                    }                  
                    texCoords.push_back( Vec2<Real>(x[0], x[1]) );
                }
            }
            else if (buffer[0] == 'f')
            {                
                str = buffer+2;
                for(int k = 0; k < 3; ++k)
                {
                    pch = strtok (str, ObjDelimiters);
                    if (pch) ip[k] = atoi(pch) - 1;
                    else      return false;
                    str = NULL;
                    if (texCoords.size() > 0)
                    {
                        pch = strtok (NULL, ObjDelimiters);
                        if (pch)  it[k] = atoi(pch) - 1;
                        else return false;
                    }
                    if (normals.size() > 0)
                    {
                        pch = strtok (NULL, ObjDelimiters);
                        if (pch)  in[k] = atoi(pch) - 1;
                        else      return false;
                    }
                }
                for(int k = 0; k < 3; ++k)
                {
                    vertex.X() = ip[k];
                    vertex.Y() = in[k];
                    vertex.Z() = it[k];
                    std::map< Vec3<long>, long, IVec3Cmp >::iterator it = vertices.find(vertex);
                    if ( it == vertices.end() )
                    {
                        vertices[vertex] = nv;
                        triangle[k]         = nv;
                        ++nv;
                    }
                    else
                    {
                        triangle[k]         =  it->second;
                    }
                }                
                triangles.push_back(triangle);
            }
        }
        if (points.size() > 0)
        {
            upoints.resize(nv);
        }
        if (normals.size() > 0)
        {
            unormals.resize(nv);
        }
        if (texCoords.size() > 0)
        {
            utexCoords.resize(nv);
        }
        for (std::map< Vec3<long>, long, IVec3Cmp >::iterator it = vertices.begin(); it != vertices.end(); ++it)
        {
            if (points.size() > 0)
            {
                upoints   [it->second]    = points   [(it->first).X()];
            }
            if (normals.size() > 0)
            {
                unormals  [it->second]    = normals  [(it->first).Y()];
            }
            if (texCoords.size() > 0)
            {
                utexCoords[it->second]    = texCoords[(it->first).Z()];
            }
        }
        fclose(fid);
    }
    else 
    {
        printf( "File not found \n");
        return false;
    }
    return true;
}



bool SaveOBJ(const char * fileName, 
             const std::vector< Vec3<Real> > & points,
             const std::vector< Vec2<Real> > & texCoords,
             const std::vector< Vec3<Real> > & normals,
             const std::vector< Vec3<long> > & triangles)
{
    FILE * fid = fopen(fileName, "w");
    if (fid) 
    {
        const size_t np = points.size();
        const size_t nn = normals.size();
        const size_t nt = texCoords.size();
        const size_t nf = triangles.size();

        fprintf(fid,"####\n");
        fprintf(fid,"#\n");
        fprintf(fid,"# OBJ File Generated by test_o3dgc\n");
        fprintf(fid,"#\n");
        fprintf(fid,"####\n");
        fprintf(fid,"# Object %s\n", fileName);
        fprintf(fid,"#\n");
        fprintf(fid,"# Vertices: %lu\n", np);
        fprintf(fid,"# Faces: %lu\n", nf);
        fprintf(fid,"#\n");
        fprintf(fid,"####\n");
        for(size_t i = 0; i < np; ++i)
        {
            fprintf(fid,"v %f %f %f\n", points[i].X(), points[i].Y(), points[i].Z());
        }
        for(size_t i = 0; i < nn; ++i)
        {
            fprintf(fid,"vn %f %f %f\n", normals[i].X(), normals[i].Y(), normals[i].Z());
        }
        for(size_t i = 0; i < nt; ++i)
        {
            fprintf(fid,"vt %f %f\n", texCoords[i].X(), texCoords[i].Y());
        }
        if (nt > 0 && nn >0)
        {
            for(size_t i = 0; i < nf; ++i)
            {
                fprintf(fid,"f %ld/%ld/%ld %ld/%ld/%ld %ld/%ld/%ld\n", triangles[i].X()+1, triangles[i].X()+1, triangles[i].X()+1,
                                                                       triangles[i].Y()+1, triangles[i].Y()+1, triangles[i].Y()+1, 
                                                                       triangles[i].Z()+1, triangles[i].Z()+1, triangles[i].Z()+1);
            }
        }
        else if (nt == 0 && nn > 0)
        {
            for(size_t i = 0; i < nf; ++i)
            {
                fprintf(fid,"f %ld//%ld %ld//%ld %ld//%ld\n", triangles[i].X()+1, triangles[i].X()+1,
                                                              triangles[i].Y()+1, triangles[i].Y()+1, 
                                                              triangles[i].Z()+1, triangles[i].Z()+1);
            }
        }
        else if (nt > 0 && nn == 0)
        {
            for(size_t i = 0; i < nf; ++i)
            {
                fprintf(fid,"f %ld/%ld %ld/%ld %ld/%ld\n", triangles[i].X()+1, triangles[i].X()+1,
                                                           triangles[i].Y()+1, triangles[i].Y()+1, 
                                                           triangles[i].Z()+1, triangles[i].Z()+1);
            }
        }
        else
        {
            for(size_t i = 0; i < nf; ++i)
            {
                fprintf(fid,"f %ld %ld %ld\n", triangles[i].X()+1, triangles[i].Y()+1, triangles[i].Z()+1);
            }
        }
        fclose(fid);
    }
    else 
    {
        printf( "Not able to create file\n");
    }
    return true;
}



