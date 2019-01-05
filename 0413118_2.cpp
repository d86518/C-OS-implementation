#include<cstdio>
#include<cstdlib>
#include<time.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<assert.h>

int forkNum;

unsigned int* in[2];

void solve(uint* shmPtr, int tpid, int n){
	int seg = n/forkNum+(n%forkNum>0);
	int base = tpid*seg;
	int last = base+seg<=n?base+seg:n;
	for(unsigned int i=base;i<last;++i)
	for(unsigned int j=0;j<n;++j){
		shmPtr[i*n+j]=0;
		for(unsigned int k=0;k<n;++k)
			shmPtr[i*n+j]+=in[0][i*n+k]*in[1][k*n+j];
	}
}

int main(){
	unsigned int n;
	struct timeval start, end;
	printf("Input the matrix dimension: ");
	scanf("%d",&n); 
	puts("");
	// IPC_PRIVATE:���ͤ@�ӷs���@�ɰO������q
	// S_IRUSR: Ū�O������q
    // _SIWUSR: �g�O������q
    // PIC_CREAT :�T�O�}�Ҫ��O����O�s��,�Ӥ��O�{�s���O����.
    //  |0666 : �@������ ,  ubuntu�n�[
	int shmID = shmget(IPC_PRIVATE, 3*n*n*sizeof(unsigned int), IPC_CREAT | 0666);
	if(shmID>=0){
		//shmaddr=NULL -> automatically map
		unsigned int* shmPtr = (unsigned int*) shmat(shmID,NULL,0);
		in[0] = shmPtr+n*n, in[1]=shmPtr+n*n*2;
		for(int i=0;i<n;++i)
			for(int j=0;j<n;++j)
				in[0][i*n+j] = i*n+j, in[1][i*n+j] = i*n+j;
		for(forkNum=1;forkNum<=16;++forkNum){
			printf("Multiplying matrices using %d process%s\n",forkNum,forkNum>1?"es":"");
			gettimeofday(&start, 0);
			clock_t begin = clock();
			for(int i=forkNum-1;i;--i)
				if(!fork()){
					solve(shmPtr,i,n);
					exit(0);
				}
			solve(shmPtr,0,n);
			int status;
			unsigned int sum=0;
			while(wait(&status)!=-1);
			for(int i=0;i<n;++i){
				for(int j=0;j<n;++j){
					sum+=shmPtr[i*n+j];
					}
				}
			gettimeofday(&end, 0);
			int sec = end.tv_sec-start.tv_sec;
			int usec = end.tv_usec-start.tv_usec;
			//��gettimeofday�n�����~�t 
			printf("Elapsed time: %lf sec, Checksum: %u\n",(double)(clock()-begin)/CLOCKS_PER_SEC,sum);
		}
	//detach
	shmdt((void*)shmPtr);
	//�屼 RemoveID
	shmctl(shmID, IPC_RMID, NULL);
	exit(0);
	} 
 return 0;
}
