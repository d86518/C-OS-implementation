#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

void *printhello(void *data){
	int i=0;
	for(int c=0;c<50;c++){
		i=1;
		i=i*10;
		printf("%d",i);
		}
	pthread_exit(NULL);
	}

int main(int argc, char **argv){
	int rc=0;
	pthread_t *thread=malloc(100*sizeof(pthread_t));
	int t =11;
	struct timespec tt1,tt2;
	clock_gettime(CLOCK_REALTIME,&tt1);
	for(int i=0;i<100;i++){
	pthread_create(&thread[i],NULL,printhello,NULL);
	}
	for(int i=0;i<100;i++){
	pthread_join(thread[i],NULL);
	}
	clock_gettime(CLOCK_REALTIME,&tt2);
	printf("Msec:%ld\n",(tt2.tv_nsec-tt1.tv_nsec)/1000);
	return 0;
}
