/*****************************************************************************************
checkdiskimage.cpp

checkdiskimage is a utility to help us to do a quick flash.
This ultility doesn't do the device flash, it only analyzes, the file, so it's 100% safe.

It analyzes the diskimage file, and create a output text datafile containing the information
of all the useful datablock for the diskimage file. Each line is a datablock to flash on the device.
In each line, there are two numbers, first number is the starting address, 
second number is the length of the datablock.

The qflash.sh script file uses the output text file to do a DD flash for each datablock.

*****************************************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

// So we can handle huge size files.
#define _FILE_OFFSET_BITS 64

using namespace std;

// the output data text file name produced by this utility
const std::string outputfilename = "tempDataBlocks.txt";

// define useful constants , 1MB , 10MB, 100MB size (bytes)
const long long MB1 = (long long)1024 * 1024;
const long long MB10 = (long long)10 * MB1;
const long long MB100 = 100 * MB1;

/**
  The simple data structure for data block,
  containing the starting address and length of the datablock.
*/

struct DataBlock
{
   long long start, len;
};


/**
   check if the buffer is all zero, note the size is uint.
   @param buffer the buffer to check
   @param size the size of the buffer
   @return true if the buffer is all zero, false otherwise
*/

bool isBufferAllZero(char *buffer, unsigned int size)
{
   if (!buffer)
   {
      cout << "buffer is null pointer" << endl;
      return true;
   }
   for (unsigned i = 0; i < size; i++)
   {
      //printf("%d ", buffer[i]);
      if (buffer[i] != 0)
      {
         //it's not zero, immediately return false
         return false;
      }
   }
   return true;
}

/**
   isEmptyBlock check if a block in the file is all zero
   @param fp the FILE pointer for the file to check, it should be already open, and the file seek pointer is already in the right position
   @param readAndTestBlockSize the data block size
   @return true if the file datablock is all zero, false otherwise
*/
bool isEmptyBlock(FILE *fp, long long readAndTestBlockSize)
{
   unsigned int smallStepSize = 0xffff;
   unsigned int leftSize = 0;
   leftSize = readAndTestBlockSize%smallStepSize;
   long long loopTime = readAndTestBlockSize / smallStepSize;
   char *buffer = new char[smallStepSize];


   for (long long i = 0; i <= loopTime; i++)
   {
      unsigned toReadSize = 0;
      if (i < loopTime)
      {
         toReadSize = smallStepSize;
      }
      else
      {
         toReadSize = leftSize;
      }
      unsigned int actualReadSize = fread(buffer, 1, toReadSize, fp);
      if (actualReadSize != toReadSize)
      {
         cout << "error, actualReadSize " << actualReadSize << ", not equal to freadSize " << toReadSize << endl;
         return false;
      }

      if (!isBufferAllZero(buffer, toReadSize))
      {

         //it is not all zero in this block
         delete buffer;
         return false;
      }

   }

   delete buffer;
   return true;

}


/**
   analyzeDiskImageFile
   analyze the disk image file and produce a output data structure containing all the datablock list.
   Assume we always DD the first block: 0 to 100 MB (headerBlock), do not analyze if it's empty or not, just include them in the output.
   @param imgfile the diskimage file name with path
   @param headerBlock the size of first part (from 0 to ) of the file we must put in the result no matter it's all zero or not.
   @param dataBlockSize the size of each data block we test. 
   @return vector<DataBlock> the list of each non-all-zero datablock
*/
std::vector<DataBlock> *  analyzeDiskImageFile(string imgfile, const long long headerBlock, const long long dataBlockSize)
{   
   FILE *fp;
   const int NOTINIT = -1;
   fp = fopen(imgfile.c_str(), "rb");
   if (!fp)
   {
      cout << "File can not be opened ?? " << imgfile.c_str();
      return NULL;
   }
   long long  totalFileSize = 0;
   fseeko(fp, 0, SEEK_END);
   totalFileSize = ftello(fp);

   if (totalFileSize <= 0) {
      fclose(fp);
      cout << "error, image file size is invalid";
      return NULL;
   }
   
   std::vector<DataBlock> *result = new std::vector<DataBlock>();

   long long checkStartingAddress = 0;
   DataBlock block;
   if (totalFileSize <= headerBlock)
   {
      //the file is too small, smaller than the must flash headerBlock
      //so just return 0 to totalFileSize
      block.start = 0;
      block.len = totalFileSize;
      result->push_back(block);
      fclose(fp);
      return result;
   }
   else
   {
      //the first block, start is 0, len is unknown yet.
      block.start = 0;
      block.len = 0;

   }

   checkStartingAddress = headerBlock;
   fseeko(fp, checkStartingAddress, SEEK_SET);

   //the first data block, at this time is not over, we don't know the len of it yet
   block.len = headerBlock;

   do {
      long long  readAndTestBlockSize = 0;
      // get the testReadSize for next loop, 
      if ((checkStartingAddress + dataBlockSize) > totalFileSize)
      {  // only has a little left, just read what's left
         readAndTestBlockSize = totalFileSize - checkStartingAddress;
      }
      else
      {  //still have a lot of to read, read the full step size: dataBlockSize
         readAndTestBlockSize = dataBlockSize;
      }

      if (!isEmptyBlock(fp, readAndTestBlockSize))
      {
         // not empty block
         if (block.start == NOTINIT)
         {  // found the head of data block!
            block.start = checkStartingAddress;
            block.len = readAndTestBlockSize;
         }
         else
         {
            // searching for end of data block
            block.len += readAndTestBlockSize;
         }

      }
      else
      {
         // it's empty block
         if (block.start != NOTINIT)
         {
            //met an empty block, this data block is over.
            result->push_back(block);
            block.start = NOTINIT;
            block.len = 0;
         }
      }

      checkStartingAddress += readAndTestBlockSize;
      fseeko(fp, checkStartingAddress, SEEK_SET);

      if (checkStartingAddress >= totalFileSize)
      {
         //met the end of file. before we quit, need to check the current work status   
         if (block.start != NOTINIT)
         {
            //already met the end of file, but we only have block start , no len yet
            //record this data block, prepare for next data block
            result->push_back(block);
         }
         else
         {
            // block.start is NOTINIT
            // so it means the job is done the last block is empty, the previous block is already saved, we can just quit.
         }
         break;
      }
   } while (true);

   fclose(fp);
   return result;
}


/** 
   for test, output the datablock info to screen
*/
void showDataBlockList(std::vector<DataBlock> * datablockVector)
{
   if (datablockVector == NULL)
   {
      cout << "NULL";
   }

   for (unsigned int i = 0; i < datablockVector->size(); i++)
   {
      cout << "block:" << i << "  start: " << datablockVector->at(i).start << "  " << "len: " << datablockVector->at(i).len << endl;
   }
}

/**
 * for test
 * generic test method for analyzeDiskImageFile()
 *
 */
bool doTest(string file, long long headerBlockSize, long long dataBlockSize, std::vector<DataBlock> *expected_result)
{
   bool testIsPass = true;
   std::vector<DataBlock> *actual_result = analyzeDiskImageFile(file, headerBlockSize, dataBlockSize);
   if (expected_result == NULL && actual_result == NULL)
   {
      return true;
   }

   if (expected_result == NULL || actual_result == NULL)
   {
      //fail
      cout << "NULL VS NOT NULL";
      return false;
   }


   if (actual_result->size() != expected_result->size())
   {
      cout << "Test Failed, result vector size not equal " << expected_result->size() << " VS " << actual_result->size();

      testIsPass = false;
      cout << "expected result: " << endl;
      showDataBlockList(expected_result);
      cout << "actual result: " << endl;
      showDataBlockList(actual_result);
      return false;

   }

   for (unsigned int i = 0; i < expected_result->size(); i++)
   {
      if (expected_result->at(i).start != actual_result->at(i).start || expected_result->at(i).len != actual_result->at(i).len)
      {
         cout << "Fail" << endl;
         cout << "expected result: " << endl;
         showDataBlockList(expected_result);
         cout << "actual result: " << endl;
         showDataBlockList(actual_result);
         return false;
      }

   }
   //all same , true

   if (testIsPass)
   {
      cout << file.c_str() << "  Pass" << endl;
   }
   else
   {
      cout << file.c_str() << " Fail" << endl;

   }
   return testIsPass;

}

/**
   for test.
   test a empty file
*/
void doTestAllEmptyFile()
{
   string filename = "doTestAllEmptyFile";
   FILE *fp = fopen(filename.c_str(), "wb");
   int size = 3;
   char *buffer = new char[size];
   memset(buffer, 0, size);

   fwrite(buffer, 1, size, fp);
   fclose(fp);

   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   DataBlock block;
   block.start = 0;
   block.len = 1;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   expected_result->push_back(block);
   doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}


/**
 for test.
 test a file full of data
*/
void doTestAllFullFile()
{
   string filename = "doTestAllFullFile";
   FILE *fp = fopen(filename.c_str(), "wb");
   int size = 5;
   char *buffer = new char[size];
   memset(buffer, 1, size);

   fwrite(buffer, 1, size, fp);
   fclose(fp);

   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   DataBlock block;
   block.start = 0;
   block.len = size;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   expected_result->push_back(block);
   doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}

/**
   for test
   test a file scenario 1
*/
void doTestFile1()
{
   char  buffer[] = { 1,0,0,1,0,1 };
   string filename = "doTestFile1";
   FILE *fp = fopen(filename.c_str(), "wb");
   fwrite(buffer, sizeof(buffer), 1, fp);
   fclose(fp);
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   expected_result->push_back({ 0,1 });
   expected_result->push_back({ 3,3 });
   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}

/**
  for test
  test a file scenario 2
*/
void doTestFile2()
{
   char  buffer[] = { 1,0,1,1,0,1 };
   string filename = "doTestFile2";
   FILE *fp = fopen(filename.c_str(), "wb");
   fwrite(buffer, sizeof(buffer), 1, fp);
   fclose(fp);
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   expected_result->push_back({ 0, 6 });
   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}

/**
  for test
  test a file scenario 3
*/
void doTestFile3()
{
   char  buffer[] = { 1,0,0,1 };
   string filename = "doTestFile3";
   FILE *fp = fopen(filename.c_str(), "wb");
   fwrite(buffer, sizeof(buffer), 1, fp);
   fclose(fp);
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   expected_result->push_back({ 0, 1 });
   expected_result->push_back({ 3, 1 });

   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}


/**
  for test
  test a file scenario 4
*/
void doTestFile4()
{
   char  buffer[] = { 1,0,0,0,1,1 };
   string filename = "doTestFile4";
   FILE *fp = fopen(filename.c_str(), "wb");
   fwrite(buffer, sizeof(buffer), 1, fp);
   fclose(fp);
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   expected_result->push_back({ 0, 1 });
   expected_result->push_back({ 3, 3 });

   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);
}

/**
  for test
  test a file scenario 5
*/
void doTestFile5()
{
   //char  buffer[] = { 1,0,0,0,1,1,0,0,0,0 };
   char  buffer[] = { 1,0,0,1,0,0,0,0 };
   string filename = "doTestFile5";
   FILE *fp = fopen(filename.c_str(), "wb");
   fwrite(buffer, sizeof(buffer), 1, fp);
   fclose(fp);
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   expected_result->push_back({ 0, 1 });
   expected_result->push_back({ 3, 2 });

   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}


/**
  for test
  test a file scenario 6
*/
void doTestFile6()
{
   char  buffer[] = { 1,0,0,0,1,1,0,0,0,0 };
   string filename = "doTestFile6";
   FILE *fp = fopen(filename.c_str(), "wb");
   fwrite(buffer, sizeof(buffer), 1, fp);
   fclose(fp);
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   expected_result->push_back({ 0, 1 });
   expected_result->push_back({ 3, 4 });

   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}

/**
  for test
  test a genueine diskimage file scenario
*/
void doTestFileDiskImage()
{

   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();

   expected_result->push_back({ 0, 104857600 });
   expected_result->push_back({ 576716800, 104857600 });
   expected_result->push_back({ 1101004800, 52428800 });
   expected_result->push_back({ 3827302400,  6291456 });


   long long headerBlockSize = 5 * MB10;
   long long dataBlockSize = (long long)5 * MB10;
   bool pass = doTest("/home/chjiang/Downloads/diskimage.p1.img", headerBlockSize, dataBlockSize, expected_result);

}

/**
  for test.
  run all tests.
*/
void test()
{
   doTestAllEmptyFile();

   doTestFile1();

   doTestFile6();

   doTestFile5();
   doTestFile4();
   doTestFile2();
   doTestFile3();
   doTestAllFullFile();
   doTestFileDiskImage();

}


/**
  the main function of this utility
*/
int main(int argc, char *argv[])
{

   if (argc != 2)
   {
      cout << " use as: ./checkDiskImage /home/username/Downloads/diskimage.p1.img" << endl;
      return 1;
   }

      //argv[1] is the downloaded or built diskimage file with full path.
   string diskimagefilename = argv[1];
   cout << "checking file... " << diskimagefilename.c_str() << endl;

   std::vector<DataBlock> *datablock_list;

   // Assume the first 1XX MB data is always need to flash
   const long long headerBlockSize = 110 * MB1;
   //the size of each data block
   const long long dataBlockSize = (long long)33 * MB1;

   datablock_list = analyzeDiskImageFile(diskimagefilename, headerBlockSize, dataBlockSize);

   cout << "checking file done" << endl;

   if (!datablock_list)
   {
      cout << "abort, analyze disk image failed" << endl;
      return 1;
   }

   FILE *fpout = fopen(outputfilename.c_str(), "wa");
   if (!fpout)
   {
      cout << "Error, can not open output file" << outputfilename.c_str() << endl;
      return 1;
   }

   for (int i = 0; i < datablock_list->size(); i++)
   {
      fprintf(fpout, "%lld,%lld\n", datablock_list->at(i).start, datablock_list->at(i).len);
      cout << "" << datablock_list->at(i).start << "," << datablock_list->at(i).len << endl;
   }

   fclose(fpout);

   cout << "done, analyze result is written in file " << outputfilename.c_str() << endl;

   return 0;
}

