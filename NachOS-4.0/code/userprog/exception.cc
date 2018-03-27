// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
      {
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;
      }

//usr
	case SC_Sub:
	{
	int subResult,op1,op2;
	op1=(int)kernel->machine->ReadRegister(4);
	op2=(int)kernel->machine->ReadRegister(5);
	subResult=SysSub(op1,op2);
	kernel->machine->WriteRegister(2,(int)subResult);
	printf("%d-%d=%d\n",op1,op2,subResult);

	/*Modify return point*/
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	}
	
	
	case SC_Create:
	{
	int CreateAddr;
	char *CreateName=new char[128];
	int CreateValue;
	int CreateCount=0;
	//int CreateSize;
	
	CreateAddr=(int)kernel->machine->ReadRegister(4);
	//CreateSize=(int)kernel->machine->ReadRegister(5);
	
	do{
	  kernel->machine->ReadMem(CreateAddr+CreateCount,1,&CreateValue);
	  CreateName[CreateCount]=*(char *)&CreateValue;
	  CreateCount++;
	}while(*(char *)&CreateValue != '\0' && CreateCount<128);
	
	//kernel->machine->ReadMem(CreateAddr,32,CreateName);
	
	if(SysCreate(CreateName))
	{
		kernel->machine->WriteRegister(2,1);
		printf("Create file %s succeeded\n\n",CreateName);
	}
	else
	{
		kernel->machine->WriteRegister(2,0);
		printf("Create file %s failed\n\n",CreateName);
	}
	
	{
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	}
	
	case SC_Remove:
	{
	  int RemoveAddr;
	char *RemoveName=new char[128];
	int RemoveValue;
	int RemoveCount=0;
	//int CreateSize;
	
	RemoveAddr=(int)kernel->machine->ReadRegister(4);
	//CreateSize=(int)kernel->machine->ReadRegister(5);
	
	do{
	  kernel->machine->ReadMem(RemoveAddr+RemoveCount,1,&RemoveValue);
	  RemoveName[RemoveCount]=*(char *)&RemoveValue;
	  RemoveCount++;
	}while(*(char *)&RemoveValue != '\0' && RemoveCount<128);
	
	//kernel->machine->ReadMem(CreateAddr,32,CreateName);
	
	if(SysRemove(RemoveName))
	{
		kernel->machine->WriteRegister(2,1);
		printf("Remove file %s succeeded\n\n",RemoveName);
	}
	else
	{
		kernel->machine->WriteRegister(2,0);
		printf("Remove file %s failed\n\n",RemoveName);
	}
	
	{
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	  
	}
	
	
	
	case SC_Open:
	{
	  int OpenAddr;
	char *OpenName=new char[128];
	int OpenValue;
	int OpenCount=0;
	OpenFileId OpenId;
	
	OpenAddr=(int)kernel->machine->ReadRegister(4);
	
	do{
	  kernel->machine->ReadMem(OpenAddr+OpenCount,1,&OpenValue);
	  OpenName[OpenCount]=*(char *)&OpenValue;
	  OpenCount++;
	}while(*(char *)&OpenValue != '\0' && OpenCount<128);
	
	OpenId=SysOpen(OpenName);
	if(OpenId)
	{
	  printf("Open file %s succeeded\n\n",OpenName);
	//  printf("file id is %d \n",OpenId);
	  kernel->machine->WriteRegister(2,(int)OpenId);
	}
	else
	{
	  printf("Open file %s failed\n\n",OpenName);
	  kernel->machine->WriteRegister(2,0);
	}
	
	{
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	}
	
	
	
	case SC_Close:
	{
	  int CloseId=kernel->machine->ReadRegister(4);
	  SysClose(CloseId);
	  /*
	  if(SysClose(CloseId))
	  {
	    printf("close file suceeded\n\n");
	    kernel->machine->WriteRegister(2,1);
	  }
	  else{
	    printf("close file failed\n\n");
	    kernel->machine->WriteRegister(2,0);
	  }*/
	  
	  {
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	return;
	ASSERTNOTREACHED();
	break;
	}
	
	
	case SC_Read:
	{
	  int ReadBuff,ReadSize,ReadId;
	  ReadBuff=kernel->machine->ReadRegister(4);
	  ReadSize=kernel->machine->ReadRegister(5);
	  ReadId=kernel->machine->ReadRegister(6);
	  
	  int HasRead;
	//  printf("ReadId is %d\n",ReadId);
	  HasRead=SysRead(ReadBuff,ReadSize,ReadId);
	  kernel->machine->WriteRegister(2,int(HasRead));
	  printf("Read file succeeded, %d bytes\n\n",HasRead);
	  
	  {
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	 }
	 return;
	 ASSERTNOTREACHED();
	 break;
	}
	
      	case SC_Write:
	{
	  int WriteBuff,WriteSize,WriteId;
	  WriteBuff=kernel->machine->ReadRegister(4);
	  WriteSize=kernel->machine->ReadRegister(5);
	  WriteId=kernel->machine->ReadRegister(6);
	  
	  int HasWrite;
	//  printf("WriteId is %d\n",WriteId);
	  HasWrite=SysWrite(WriteBuff,WriteSize,WriteId);
	  kernel->machine->WriteRegister(2,int(HasWrite));
	  printf("Write file succeeded, %d bytes\n\n",HasWrite);
	  {
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	 }
	 return;
	 ASSERTNOTREACHED();
	 break;
	}
	

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      
      
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
