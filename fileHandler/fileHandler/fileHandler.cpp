// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


#include <iostream>
#define _CRT_SECURE_NO_DEPRECATE


#include <stdio.h>
#include <stdlib.h>
#include <vector>




using namespace std;

const long long MB1 = 1024 * 1024;
const long long MB10 = 10 * MB1;
const long long MB100 = 100 * MB1;


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

struct DataBlock
{
    long long start, len;
};




bool isBufferAllZero(char *buffer, long long size)
{
   if(!buffer) 
   {
      cout<< "buffer is null pointer"<<endl; 
      return true;
   }
   for(int i=0;i<size;i++)
   {
      if(buffer[i]!=0)
      {
         //it's not zero, immediately return false
         return false;
      }
   }
   return true;
}

// find and return the start/end address of the next data block
DataBlock * zfindDataBlock(FILE*fp, const long long start, const long long totalFileSize)
{
   long long stepSize = 30 * MB1;
   char *buffer = new char[stepSize];
   DataBlock *datablock=new DataBlock();

   long long doneAddress= start;
   _fseeki64(fp, start, SEEK_SET);

   // Step 1 find the start address of the data block
   do {
      long long readSize;

      if ((doneAddress + stepSize)> totalFileSize)
      {   // only has a little left, just read what's left
         readSize = totalFileSize - doneAddress;
      }
      else
      {  //still have a lot of to read, read the full step size
         readSize = stepSize;
      }

      fread(buffer, readSize, 1, fp);
      if (!isBufferAllZero(buffer, readSize))
      {
         //it has content in this block, record the start address and stop searching
         datablock->start = doneAddress;
         //record the doneSize for next step
         doneAddress +=readSize;
         break;
      }

      doneAddress += readSize;

   } while (doneAddress < totalFileSize);

   // Step 2 find the end address of the data block
   do {
      long long readSize;

      if ((doneAddress + stepSize)> totalFileSize)
      {   // only has a little left, just read what's left
         readSize = totalFileSize - doneAddress;
      }
      else
      {  //still have a lot of to read, read the full step size
         readSize = stepSize;
      }

      fread(buffer, readSize, 1, fp);
      if (isBufferAllZero(buffer, readSize))
      {
         //it is all zero in this block, meaning the starting address is the end of the data block. 
         datablock->len = doneAddress- start;    
         break;
      }

      doneAddress += readSize;

   }while (doneAddress < totalFileSize);

   delete buffer; 
   return datablock;   
}


bool isEmptyBlock(FILE *fp, long long readAndTestBlockSize)
{
   char *buffer = new char[readAndTestBlockSize];
   //unsigned int actualReadSize = fread(buffer,  1, freadSize, fp);
   unsigned int actualReadSize = fread(buffer, 1, readAndTestBlockSize, fp);
   if(actualReadSize!= readAndTestBlockSize)
   {
      cout<< "error, actualReadSize " << actualReadSize << ", not equal to freadSize " << readAndTestBlockSize << endl;
      return false;
   }
   if (isBufferAllZero(buffer, readAndTestBlockSize))
   {
      //it is all zero in this block
      return true;
   }

   delete buffer;
   return false;

}


std::vector<DataBlock> *  analyzeDiskImageFile(string imgfile)
{
    // DD first block: 0 to 100 MB, do not analyze if it's empty or not, just write this block to emmc.
    //long long FirstMustFlashBlockSize = MB100;
    long long FirstMustFlashBlockSize = 1;
    // check the next stepSize, if it's all empty then skip this block (stepSize), otherwise this part need to flash
    //const long long DataBlockSize = 30 * MB1;
    const long long DataBlockSize = 2 ;
    FILE *fp;

    fp = fopen(imgfile.c_str(), "rb");
    if (!fp)
    {
        cout << "File can not ben opened ?? " << imgfile.c_str();
        return NULL;
    }
    long long  totalFileSize = 0;
    _fseeki64(fp, 0, SEEK_END);
    totalFileSize = _ftelli64(fp);

    if (totalFileSize == 0) {
        fclose(fp);
        cout << "error, image file size is 0";
        return NULL;
    }
    if (totalFileSize < 0)
    {
        fclose(fp);
        cout << "error, image file size is invalid";
        return NULL;
    }
    cout << "file size is " << totalFileSize << endl;


    std::vector<DataBlock> *result = new std::vector<DataBlock>();

    long long checkStartingAddress = 0;
    DataBlock block;
    if(totalFileSize<=  FirstMustFlashBlockSize)
    {
       //the file is too small, smaller than the must flash block
       //so just return 0 to totalFileSize
       block.start=0;
       block.len = totalFileSize;
       result->push_back(block);
       return result;
    }else
    {
       block.start = 0; 
       block.len = FirstMustFlashBlockSize;
       result->push_back(block);
    }
    
    checkStartingAddress = FirstMustFlashBlockSize;
    _fseeki64(fp, checkStartingAddress, SEEK_SET);

    //DataBlock *datablock = findDataBlock(fp, doneAddress+1, totalFileSize);
    //result->push_back(*datablock);

    block.start=0;block.len=0;
    long long blocklen=0;

    do{   long long  readAndTestBlockSize =0;
          // get the testReadSize for next loop, 
          if ((checkStartingAddress + DataBlockSize)> totalFileSize)
          {   // only has a little left, just read what's left
             readAndTestBlockSize = totalFileSize - checkStartingAddress;
          }
          else
          {  //still have a lot of to read, read the full step size
             readAndTestBlockSize = DataBlockSize;
          }
       if(block.start==0)
       {
          // searching for head of data block
          if( !isEmptyBlock(fp, readAndTestBlockSize) )
          {
             // found the head of data block!
             block.start= checkStartingAddress;
             checkStartingAddress += readAndTestBlockSize;
             blocklen = readAndTestBlockSize;

          }

          if(checkStartingAddress + readAndTestBlockSize >= totalFileSize)
          {
             // this is the last block of the file. time to set data and stop reading.
             block.len=blocklen;
             result->push_back(block);

          }

       }else if(block.start!=0)
       {
             // searching for end of data block


             if (isEmptyBlock(fp, readAndTestBlockSize) )
             {
                 // found the end of this block           
                 blocklen += readAndTestBlockSize;
                 //record this data block, prepare for next data block
                 block.len = blocklen;
                 result->push_back(block);
                 block.start=0;
                 block.len = 0;

             }else 
             {  // it's not empty
                blocklen += readAndTestBlockSize;

             }
             checkStartingAddress += readAndTestBlockSize;

             if (checkStartingAddress >= totalFileSize)
             {
                //end of file. 
                block.len = blocklen;
                //record this data block, prepare for next data block
                result->push_back(block);
                break;
             }

       }
    
       if( checkStartingAddress>= totalFileSize) 
       {
          break;
       }
    }while (true); 

    fclose(fp);
 
    return result;
}


int main()
{
    string filenameIn = "d:\\in.txt";
    string filenameOut = "d:\\out.txt";
    string imageFile = "d:\\diskimage.txt";

    std::vector<DataBlock> *result = analyzeDiskImageFile(imageFile);
    if (!result) { cout << " result is null";   cin.get();  return 1; }
 
    for (int i = 0; i < result->size(); i++)
    {
        cout << "block:"<<i<<"  start: " << result->at(i).start << "  " << "len: " << result->at(i).len << endl;
    }

    delete result;
    //copyFile(filenameIn, filenameOut);

    //do DD to write everything to EMMC before the starting point;
    //Therefore we don't need to check useful content before starting point.
    long startFrom = MB100;

    cin.get();

    return 0;
}

