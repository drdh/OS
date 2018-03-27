#include "syscall.h"
//#include<stdio.h>
int main()
{
  OpenFileId id;
  char temp[64];
	Create("testfile.txt");
	id=Open("testfile.txt");
	Write("qwwerttyuiiasdfghjk1234567890123223",30,id);
	id=Open("testfile.txt");
	Read(temp,30,id);
	//id=Open("testfile.txt");
	Close(id);
	Remove("testfile.txt");
	//id=Open("testfile.txt");
	//Close(id);
	//class(id);
	//printf("read: \n %s \n",read);

	Halt();

}

