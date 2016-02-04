// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


#include <iostream>
#define _CRT_SECURE_NO_DEPRECATE


#include <stdio.h>
#include <stdlib.h>
#include <vector>




using namespace std;

const   long MB1 = 1024 * 1024;

const   long MB100 = 100 * MB1;

int copyFile(string origFile, string destFile)
{
    const long StepSize = 20 * MB1;

    FILE *fpin, *fpout;

    fpin = fopen(origFile.c_str(), "rb");
    if (!fpin)
    {
        cout << "File can not ben opened ?? " << origFile.c_str();
        return 1;
    }
    //each step is 20MB when we move forward
    // fseek(fp, startFrom, SEEK_SET);

    long long  origSize = 1;
    fseek(fpin, 0, SEEK_END);
    origSize = _ftelli64(fpin);

    if (origSize == 0) { cout << "error, orig file size is 0"; return 1; }
    if (origSize < 0) { cout << "error, orig file size is invalid"; return origSize; }


    cout << "file size is " << origSize << endl;
    _fseeki64(fpin, 0, SEEK_SET);

    fpout = fopen(destFile.c_str(), "wb");

    long long doneSize = 0;

    char *buffer = new char[StepSize];
    do {
        long long readSize = 0;
        if (doneSize + StepSize <= origSize)
        {
            readSize = StepSize;
        }
        else
        {
            // only a few left, can not read the whole stepsize, only read what's left!
            readSize = origSize - doneSize;
        }

        fread(buffer, readSize, 1, fpin);
        fwrite(buffer, readSize, 1, fpout);
        doneSize += readSize;

    } while (doneSize < origSize);
    delete buffer;

    fclose(fpin);
    fclose(fpout);
    return 0;
}

struct ContentBlock 
{
    long long start, len;
};


std::vector<ContentBlock> *  analyzeDiskImageFile(string imgfile)
{ 
    std::vector<ContentBlock> *result = new std::vector<ContentBlock>();
    

    //ContentBlock *block;
    ContentBlock block;
    int i;
    for (i = 0; i < 5; i++)
    {
        //block = new ContentBlock();
        // block->start = i * 10;
        block.start = i * 10;
        // block->len = i * 10 + i * 5;
        block.len = i * 10 + i * 5;

        result->push_back(block);
    }
    return result;
}


int main()
{


    string filenameIn = "d:\\in.txt";
    string filenameOut = "d:\\out.txt";
    string imageFile = "diskimage.txt";

    std::vector<ContentBlock> *result= 
    analyzeDiskImageFile(imageFile );
    if (!result) { cout << " result is null";  return 1; }

    ContentBlock block;
    int i;
    for (i = 0; i < result->size();i++)
    {
       
        cout << "start " << result->at(i).start << "   " << "len: " << result->at(i).len << endl;
    }

    delete result;
    //copyFile(filenameIn, filenameOut);

    //do DD to write everything to EMMC before the starting point;
    //Therefore we don't need to check useful content before starting point.
    long startFrom = MB100;

    cin.get();

    return 0;
}

