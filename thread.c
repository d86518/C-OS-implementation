#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

int main(int argc, char **argv){
	int rc=0;
	pthread_t thread;
	struct timespec tt1,tt2;
	clock_gettime(CLOCK_REALTIME,&tt1);
	int i=0;
	for(int c=0;c<5000;c++){
	i=1; i*=10; printf("%d",i);}
	clock_gettime(CLOCK_REALTIME,&tt2);
	printf("Msec:%ld",(tt2.tv_nsec-tt1.tv_nsec)/1000);
	return 0;
}
