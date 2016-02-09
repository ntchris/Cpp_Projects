
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

#define _FILE_OFFSET_BITS 64

using namespace std;

const long long MB1 = (long long )1024 * 1024;
const long long MB10 = (long long )10 * MB1;
const long long MB100 = 100 * MB1;


long long copyFile(string origFile, string destFile)
{
    const unsigned int StepSize = 0xff;

    FILE *fpin, *fpout;

    fpin = fopen(origFile.c_str(), "rb");
    if (!fpin)
    {
        cout << "File can not ben opened ?? " << origFile.c_str();
        return 1;
    }
    //each step is 20MB when we move forward

    long long  origSize = 1;
    fseeko(fpin, 0, SEEK_END);
    origSize = ftello(fpin);

    if (origSize == 0) { cout << "error, orig file size is 0"; return 1; }
    if (origSize < 0) { cout << "error, orig file size is invalid"; return origSize; }


    fseeko(fpin, 0, SEEK_SET);

    fpout = fopen(destFile.c_str(), "wb");

    long long doneSize = 0;

    char *buffer = new char[StepSize];
    do {
        unsigned int readSize = 0;
        if (doneSize + StepSize <= origSize)
        {
            readSize = StepSize;
        }
        else
        {
            // only a few left, can not read the whole stepsize, only read what's left!
            readSize = origSize%StepSize;
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




bool isBufferAllZero(char *buffer, unsigned int size)
{
   if(!buffer) 
   {
      cout<< "buffer is null pointer"<<endl; 
      return true;
   }
   for(unsigned   i=0;i<size;i++)
   {
      //printf("%d ", buffer[i]);
      if(buffer[i]!=0)
      {
         //it's not zero, immediately return false
         return false;
      }
   }
   return true;
}
/*
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
*/

bool isEmptyBlock(FILE *fp, long long readAndTestBlockSize)
{
   //const unsigned smallStepSize=0xffff;
   unsigned int smallStepSize=0xffff;
   unsigned int leftSize= 0;
   leftSize = readAndTestBlockSize%smallStepSize; 
   long long loopTime= readAndTestBlockSize/ smallStepSize;
   char *buffer = new char[smallStepSize];
   

   for(long long i=0;i<=loopTime;i++)
   {
      unsigned toReadSize=0;
      if(i<loopTime)
      {
         toReadSize= smallStepSize;
      }else 
      {
         toReadSize= leftSize;
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


std::vector<DataBlock> *  v1_working_analyzeDiskImageFile(string imgfile, const long long headerBlock, const long long dataBlockSize)
{
    FILE *fp;
    const int NOTINIT = -1;
    fp = fopen(imgfile.c_str(), "rb");
    if (!fp)
    {
        cout << "File can not ben opened ?? " << imgfile.c_str();
        return NULL;
    }
    long long  totalFileSize = 0;
    fseeko(fp, 0, SEEK_END);
    totalFileSize = ftello(fp);

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
 

    std::vector<DataBlock> *result = new std::vector<DataBlock>();

    long long checkStartingAddress = 0;
    DataBlock block;
    if(totalFileSize<= headerBlock)
    {
       //the file is too small, smaller than the must flash block
       //so just return 0 to totalFileSize
       block.start=0;
       block.len = totalFileSize;
       result->push_back(block);
       return result;
    }else
    {
       //the first block, start is 0, len is unknown yet.
       block.start = 0;
       block.len =0; 
       //block.len = FirstMustFlashBlockSize;
       //result->push_back(block);
    }
    
    checkStartingAddress = headerBlock;
    fseeko(fp, checkStartingAddress, SEEK_SET);

    //DataBlock *datablock = findDataBlock(fp, doneAddress+1, totalFileSize);
    //result->push_back(*datablock);

    long long blocklen= headerBlock;

    do{   long long  readAndTestBlockSize =0;
          // get the testReadSize for next loop, 
          if ((checkStartingAddress + dataBlockSize)> totalFileSize)
          {   // only has a little left, just read what's left
             readAndTestBlockSize = totalFileSize - checkStartingAddress;
          }
          else
          {  //still have a lot of to read, read the full step size
             readAndTestBlockSize = dataBlockSize;
          }
       if(block.start== NOTINIT)
       {
          // searching for head of data block
          if( !isEmptyBlock(fp, readAndTestBlockSize) )
          {
             // found the head of data block!
             block.start= checkStartingAddress;
             blocklen = readAndTestBlockSize;

          } 

         checkStartingAddress += readAndTestBlockSize;
         fseeko(fp, checkStartingAddress, SEEK_SET);


         if(checkStartingAddress >= totalFileSize)
          {
             // this is the last block of the file. time to set data and stop reading.
             
             if( block.start == NOTINIT)
             {
                break;
             }else{ 
                block.len=blocklen;
                result->push_back(block);
                break;
             }
          }

       }else 
       {
             // searching for end of data block

             if (isEmptyBlock(fp, readAndTestBlockSize) )
             {
                 // found the end of this block           
                 
                 //record this data block, prepare for next data block
                 block.len = blocklen;
                 result->push_back(block); 
                 block.start= NOTINIT;
                 block.len = 0;

             }else 
             {  // it's not empty
                blocklen += readAndTestBlockSize;

             }
             checkStartingAddress += readAndTestBlockSize;
             fseeko(fp, checkStartingAddress, SEEK_SET);

             if (checkStartingAddress >= totalFileSize)
             {
                //end of file. 
                if(block.start!=NOTINIT)
                {
                   block.len = blocklen;
                   //record this data block, prepare for next data block
                   result->push_back(block);
                   block.start = NOTINIT;
                   block.len =0;
                }
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






std::vector<DataBlock> *  analyzeDiskImageFile(string imgfile, const long long headerBlock, const long long dataBlockSize)
{
   // DD first block: 0 to 100 MB, do not analyze if it's empty or not, just write this block to emmc.
   //long long FirstMustFlashBlockSize = MB100;
   // check the next stepSize, if it's all empty then skip this block (stepSize), otherwise this part need to flash
   FILE *fp;
   const int NOTINIT = -1;
   fp = fopen(imgfile.c_str(), "rb");
   if (!fp)
   {
      cout << "File can not ben opened ?? " << imgfile.c_str();
      return NULL;
   }
   long long  totalFileSize = 0;
   fseeko(fp, 0, SEEK_END);
   totalFileSize = ftello(fp);

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


   std::vector<DataBlock> *result = new std::vector<DataBlock>();

   long long checkStartingAddress = 0;
   DataBlock block;
   if (totalFileSize <= headerBlock)
   {
      //the file is too small, smaller than the must flash block
      //so just return 0 to totalFileSize
      block.start = 0;
      block.len = totalFileSize;
      result->push_back(block);
      return result;
   }
   else
   {
      //the first block, start is 0, len is unknown yet.
      block.start = 0;
      block.len = 0;
      //block.len = FirstMustFlashBlockSize;
      //result->push_back(block);
   }

   checkStartingAddress = headerBlock;
   fseeko(fp, checkStartingAddress, SEEK_SET);

   //the first data block, at this time is not over, we don't know the len of it yet
   block.len = headerBlock;

   do {
      long long  readAndTestBlockSize = 0;
      // get the testReadSize for next loop, 
      if ((checkStartingAddress + dataBlockSize)> totalFileSize)
      {   // only has a little left, just read what's left
         readAndTestBlockSize = totalFileSize - checkStartingAddress;
      }
      else
      {  //still have a lot of to read, read the full step size
         readAndTestBlockSize = dataBlockSize;
      }

      if(!isEmptyBlock(fp, readAndTestBlockSize) )
      {
         // not empty block
         if (block.start == NOTINIT)
         {   // found the head of data block!
            block.start = checkStartingAddress;
            block.len = readAndTestBlockSize;
         }else
         {
            // searching for end of data block
            block.len += readAndTestBlockSize;

         }

      }else
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
         }else
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



void showDataBlockList(std::vector<DataBlock> * datablockVector)
{
   if(datablockVector==NULL)
   {
      cout<< "NULL";
   }
    
   for (unsigned int i = 0; i < datablockVector->size(); i++)
   {
      cout << "block:" << i << "  start: " << datablockVector->at(i).start << "  " << "len: " << datablockVector->at(i).len << endl;
   }
}


bool doTest(string file, long long headerBlockSize, long long dataBlockSize, std::vector<DataBlock> *expected_result)
{
   bool testIsPass= true;
   std::vector<DataBlock> *actual_result = analyzeDiskImageFile(file, headerBlockSize, dataBlockSize);
   if(expected_result==NULL && actual_result==NULL)
   {
      return true;
   }

   if (expected_result == NULL || actual_result == NULL)
   {
      //fail
      cout << "NULL VS NOT NULL";
      return false;
   }


   if(actual_result->size()!= expected_result->size())
   {
      cout<<"Test Failed, result vector size not equal "<< expected_result->size() << " VS " << actual_result->size();

      testIsPass = false;
      cout << "expected result: " << endl;
      showDataBlockList(expected_result);
      cout << "actual result: " << endl;
      showDataBlockList(actual_result);
      return false;

   }

    for(unsigned int i=0;i< expected_result->size();i++)
   {
      if(expected_result->at(i).start != actual_result->at(i).start  || expected_result->at(i).len != actual_result->at(i).len)
      {
         cout << "Fail"<<endl;
         cout<< "expected result: " << endl;
         showDataBlockList(expected_result);
         cout<< "actual result: "<<endl;
         showDataBlockList(actual_result);
         return false;
      }
  
   }
   //all same , true

   if (testIsPass)
   {
      cout << file.c_str() <<  "  Pass" << endl;
   }
   else
   {
      cout << file.c_str() << " Fail" << endl;

   } 
  return testIsPass;
     
}

void doTestAllEmptyFile()
{
   string filename ="doTestAllEmptyFile";
   FILE *fp=fopen(filename.c_str(), "wb");
   int size = 3;
   char *buffer=new char[size];
   memset(buffer,0, size);

   fwrite(buffer, 1,size, fp);
   fclose(fp);
   
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   DataBlock block;
   block.start=0;
   block.len = 1;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   expected_result->push_back(block);
   doTest(filename, headerBlockSize, dataBlockSize, expected_result);
   
}


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

void doTestFile1()
{
   char  buffer[] =   {1,0,0,1,0,1};
   string filename = "doTestFile1";
   FILE *fp = fopen(filename.c_str(), "wb");
   fwrite(buffer, sizeof(buffer), 1, fp);
   fclose(fp);
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   expected_result->push_back({0,1});
   expected_result->push_back({ 3,3 });
   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize=1;
   long long dataBlockSize=2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);
 
}


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
   expected_result->push_back({ 3, 3  });

   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 1;
   long long dataBlockSize = 2;
   bool pass = doTest(filename, headerBlockSize, dataBlockSize, expected_result);

}




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



void doTestFileAvi()
{
   
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
   

   DataBlock block;
   block.start = 0;
   block.len = 0;
   long long headerBlockSize = 20000;
   long long dataBlockSize = 4000;
   bool pass = doTest("out.avi", headerBlockSize, dataBlockSize, expected_result);

}



 
void doTestFileDiskImage()
{
   
   std::vector<DataBlock> *expected_result;
   expected_result = new std::vector<DataBlock>();
    
   expected_result->push_back({ 0, 104857600 });
   expected_result->push_back({ 576716800, 104857600 });
   expected_result->push_back({ 1101004800, 52428800 });
   expected_result->push_back({ 3827302400,  6291456 });


   long long headerBlockSize = 5*MB10;
   long long dataBlockSize = (long long ) 5 * MB10;
   bool pass = doTest("/home/chjiang/Downloads/diskimage.p1.img", headerBlockSize, dataBlockSize, expected_result);

}  
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
   // doTestFileAvi();
   doTestFileDiskImage();




   
   
}


void flashDevice()
{

}


/**
  
   
*/
int main(int argc, char *argv[]) 
{
        
   if(argc!=2)
   {
      cout <<  " use as: ./checkdiskimage /home/username/Downloads/diskimage.p1.img"<<endl;
      return 1;
   }
 
   //argv[1] is the downloaded or built diskimage file with full path.
   string diskimagefilename=argv[1];
   cout<< "checking file... "<< diskimagefilename.c_str()<<endl;
    
      
   std::vector<DataBlock> *datablock_list;
   
   const long long headerBlockSize = 5*MB10;
 
   const long long dataBlockSize = (long long ) 5 * MB10;
 
   datablock_list = analyzeDiskImageFile(diskimagefilename, headerBlockSize, dataBlockSize);
   
   cout<< "checking file done"<<endl;
   
   if(!datablock_list) 
   {
      cout << "abort, analyze disk image failed"<< endl;
      return 1;
   }
   
   const std::string outputfilename = "tempDataBlocks.txt";
   FILE *fpout = fopen(outputfilename.c_str(), "wa");
   if(!fpout)
   {
      cout<< "Error, can not open output file"<< outputfilename.c_str()<<endl;
      return 1;
   }
   
   for(int i=0;i<datablock_list->size();i++)
   {
      fprintf(fpout,"%lld,%lld\n",datablock_list->at(i).start,datablock_list->at(i).len);
      cout<< " " << datablock_list->at(i).start << " " << datablock_list->at(i).len<<endl;
   }
   
   fclose(fpout);
   
   cout<< "done, analyze result is written in file "<< outputfilename.c_str() << endl;

   return 0;
}

