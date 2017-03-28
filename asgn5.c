/*
asgn5.c

Jasmine Jans (jjans, submitter)
Kaylin Hunter
Jessica Peretti

11/4/2016

This program uses the peterson algorithm to protect the critical sections
of two processes and works with linux system shared memory function calls.

To compile: gcc asgn5.c 
To run: ./a.out time_parent time_child time_parent_non_cs time_child_non_cs

*/
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>

void child(int, int, int*, int*, int*, int, int, int);
void parent(int, int, int*, int*, int*, int, int, int);
void cs(char, int);
void non_crit_sect(int);

void main(int argc, char* argv[])
{
 //obtains access to shared memory and returns the 
 //id of a shared memory segment to shmids
 int shmid = shmget(0,1,0777 | IPC_CREAT);
 int shmid1 = shmget(0,1,0777 | IPC_CREAT);
 int shmid2 = shmget(0,1,0777 | IPC_CREAT);
 
 //attaches the memory segments
 int* pr_0 = (int*)shmat(shmid, 0, 0);
 int* pr_1 = (int*)shmat(shmid1, 0, 0);;
 int* turn = (int*)shmat(shmid2, 0, 0);;
 
 //set default times if 0 arguments
 int time_child = 1;
 int time_child_non_cs = 2;
 int time_parent = 1;
 int time_parent_non_cs = 5;
 
 //check for proper arguments
 if(!(argc == 1 || argc == 5))
 {
 	printf("You need to enter 0 or 4 parameters.");
 	return;
 }
 else 
  if(argc == 5)
  {
  		//set times to given command line arguments
 		time_parent = atoi(argv[1]);
 		time_child = atoi(argv[2]);
 		time_parent_non_cs = atoi(argv[3]);
 		time_child_non_cs = atoi(argv[4]);
 	}
 
 //forking the process
 int value;

 if ((value = fork()) < 0)
 	printf("Child could not be created\n");
 else
   if (value == 0) //fork returns 0 to child process, runs child process
   {	
   	child(time_child, time_child_non_cs, pr_0, pr_1, turn, shmid, shmid1, shmid2);
   }
   else //otherwise runs parent process
   {
  		parent(time_parent, time_parent_non_cs, pr_0, pr_1, turn, shmid, shmid1, shmid2);
 	}
}

/*
	the parent process
*/
void parent(int time_crit_sect, int time_non_crit_sect, int* pr_0, int* pr_1, int* turn, int shmid, int shmid1, int shmid2)
{

  for (int i = 0; i < 10; i++)
  {
   //protect this
   *pr_0 = 1;
   *turn = 1;
   while(*pr_1 && *turn);
   cs('p', time_crit_sect);
   *pr_0 = 0;
   //not protected
   non_crit_sect(time_non_crit_sect); 
  }
  
  //the following cleans up shared memory:
  
  //detaches the memory segments
  shmdt(pr_0);
  shmdt(pr_1);
  shmdt(turn);
  
  //removes the shared memory with control call with flag to remove
  shmctl(shmid, IPC_RMID, 0);
  shmctl(shmid1, IPC_RMID, 0);
  shmctl(shmid2, IPC_RMID, 0);
}

/*
	the child process
*/
void child(int time_crit_sect, int time_non_crit_sect, int* pr_0, int* pr_1, int* turn, int shmid, int shmid1, int shmid2)
{
  for (int i = 0; i < 10; i++)
  {
   //protect this
   *pr_1 = 1;
   *turn = 0;
   while(*pr_0 && !*turn);
   cs('c', time_crit_sect);
   *pr_1 = 0;
   //not protected
   non_crit_sect(time_non_crit_sect); 
  }
  
  //detaches the memory segments
  shmdt(pr_0);
  shmdt(pr_1);
  shmdt(turn);
}

/*
	prints notifications of extry and exit from CS
*/
void cs(char process, int time_crit_sect)
{
 if (process == 'p')
  {
   printf("parent in critical section\n");
   sleep(time_crit_sect);
   printf("parent leaving critical section\n");
  }
 else
  {
   printf("child in critical section\n");
   sleep(time_crit_sect);
   printf("child leaving critical section\n");
  }
}

/*
	simple non critical section where process sleeps
*/
void non_crit_sect(int time_non_crit_sect)
{
 sleep(time_non_crit_sect);
}
