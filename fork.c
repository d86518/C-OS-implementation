#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
//fork will create a same process as parent process to be child
//but the pid will be different
void main(){
int A[] = {1,2,3,4,5,6};
int B[] = {1,2,3};
//child:sum parent:multiply
int sum=0, pdt=1, PID,PIDKID,process=0,sumkid=0,i;
printf("PID of entrance point = %d\n",getpid());
//will first be executed (parent process) 
if ((PID=fork())==0) {
	if((PIDKID=fork())==0){
		process=3;
		printf("Grand Child Process: %d\n",process);
		printf("Grand Child PID: %d\n",getpid());
		for(i=0;i<3;i++) sumkid += B[i];
		printf("This is grandchild %d\n",sumkid);
	}
        if(PIDKID>0){
		wait(&sumkid);
		process=2;
		printf("Child Process: %d\n",process);
		printf("Child PID = %d\n",getpid());
		for(i=0;i<6;i++) sum += A[i];
		printf("This is child process computed sum %d \n" , sum);
	}
}
if (PID<0){
	printf("PID<0 = %d\n",getpid());
	fprintf(stderr,"problem creating a process \n");
	exit(1);
	}
//sleep(20)
printf("OUT PID = %d\n",getpid());  //at last it will be child process!!!!!PID3(afte a cycle)
// second step because it is still processing 
// then it will go down;not go back to the PID=fork() procedure
if (PID>0){
	wait(&sum);
	process=1;
	printf("Parent Process: %d\n",process);
	printf("Parent PID = %d\n",getpid());
	for (i=0;i<6;i++) pdt *= A[i];
	printf("The parent process complete %d \n",pdt);//it will be print out third
	}
	printf("OUT Process: %d\n",process);
}
