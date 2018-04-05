/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "openfile.h"




void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}


//user
int SysSub(int op1,int op2)
{
  return op1-op2;
}



//fileSystem
int FileID[64]={0};
int TotalFile=5;
int SysCreate(char *name)
{
  if(kernel->fileSystem->Create(name))
    return 1;
  else 
    return 0;
}

int SysRemove(char *name)
{
  if(kernel->fileSystem->Remove(name))
    return 1;
  else 
    return 0;
}

OpenFileId SysOpen(char *name)
{
  //return (OpenFileId)kernel->fileSystem->Open(name);
  int p=(int)kernel->fileSystem->Open(name);
  if(!p)
    return 0;
  TotalFile++;
  FileID[TotalFile]=p;
  return (OpenFileId)TotalFile;
}

void SysClose(int id)
{
  
  OpenFile *file=(OpenFile *)(FileID[id]);
  file->~OpenFile();
}


int SysRead(int buff,int size,int id)
{
  OpenFile *file=(OpenFile *)(FileID[id]);
  char temp[size];
  int hasRead=file->Read(temp,size);
  printf("what I read:\n %s\n",temp);
  int i;
  
  for(i=0;i<hasRead;i++)
  {
    kernel->machine->WriteMem(buff+i,1,(int)(temp[i]));
    
  }
  return hasRead;
}

int SysWrite(int buff,int size,int id)
{
    char temp[size];  
    int value;
    int count=0;
    do{
	kernel->machine->ReadMem(buff+count,1,&value);
	temp[count]=*(char *)&value;
	count++;
	}while(count<size);
    OpenFile *file=(OpenFile *)(FileID[id]);
    int hasWrite=file->Write(temp,size);
    return hasWrite;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
