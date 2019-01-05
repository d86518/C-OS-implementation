#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>
#include<queue>
using namespace std;

int n ,thread  ;
int multi[1000000] , buffer[1000000] ;
int gleft[16] , gright[16] ;
int thread_state[9] , done_job[9] ;
int pairing[8] = {0};
sem_t s[9] , finish  , mutex  ;
queue<int> job ;

struct timeval startt ,endt ; 

void swap(int *x , int *y){
   int temp = *x ;
   *x = *y ;
   *y = temp ;
}

void Merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
    int L[n1], R[n2]; 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
    i = 0; // first subarray 
    j = 0; // second subarray 
    k = l; // merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 

void bubble(int arr[] , int start , int end){
   int i ;     
   while(start < end){
     for(i = start ; i < end ; i++ ){
         if(arr[i] > arr[i+1]) swap( &arr[i] , &arr[i+1]) ;
     }
     end-- ;
   }
}
void *dispatcher(void *ptr){
    int i = 0 , j , qsize = 0;
    int num = 0;
	while(1){
	//wait for initial
	sem_wait(&s[0]) ;
	for (int num = 1; num<=7 ; num++){
		gleft[num*2] = gleft[num];
		gright[num*2] = (gleft[num]+gright[num])/2;
		gleft[num*2+1] = (gleft[num]+gright[num])/2+1;
		gright[num*2+1] = gright[num];
	}
	sem_wait(&mutex) ;
	for(i = 8 ; i <=15 ; i++){
		job.push(i);
	}
	sem_post(&mutex) ;
	qsize +=8 ;
	
	//做完離開、還沒就指派工作 
	for(j = 1 ; j <= thread ;j++){
			if(qsize == 0) break ;
			if(thread_state[j] == -1){
				thread_state[j] = 1 ;
				sem_post(&s[j]) ;
				qsize -- ;
				}
			}
					
	for(i = 0 ; i < 15 ;i++){
		//wait for someone finish(0 for dispatcher thread) 
		sem_wait(&s[0]) ;
//		printf("hello");
		//check  whcih job was finished
		for(j = 1 ; j <= thread ;j++){
			if(done_job[j] != -1 ){ //done_job[2]=9 第2個thread完成第9個job 
				//append the new job to queue(mutex)
//					printf("%d",j);
					pairing[done_job[j]/2]++; //j號剛剛工作做完就++，當下面兩工作做完(pairing=2)才會執行 
					if(pairing[done_job[j]/2]==2){
						sem_wait(&mutex) ;
						job.push(done_job[j]/2) ;
						sem_post(&mutex) ;
						qsize += 1 ;
					}
				thread_state[j] = -1 ; //因為做完了，worker thread閒下來 
				done_job[j] = -1 ;
				break ;
				}
		}
		
		//dispatch free thread
		for(j = 1 ; j <= thread ;j++){
			if(qsize == 0) break ;
			if(thread_state[j] == -1){
				thread_state[j] = 1 ;
				sem_post(&s[j]) ;
				qsize -- ;
				}
			}
		}
		
	//finish the sorting
	sem_post(&finish) ;
	}
}

void *worker_thread(void *ptr){
	
	int *thread_num = (int *)ptr ;
	int job_num ;
	while(1){
	//wait for signal
	sem_wait(&s[*thread_num]) ;
	//take a job(mutex)
	sem_wait(&mutex);
		job_num = job.front() ;
		job.pop() ;
	sem_post(&mutex);
	//do the job
	if(job_num < 8){
	//do merge
	   Merge(buffer , gleft[job_num*2] ,gright[job_num*2], gright[job_num*2+1]) ;
	}
	else{
	   bubble(buffer , gleft[job_num] , gright[job_num]) ;
	}
	//finish and then signal dispatcher
	done_job[*thread_num] = job_num ;	
//	printf("t: %d  j : %d\n",*thread_num , done_job[*thread_num]);

	sem_post(&s[0]) ;
	}
//	printf("%d\n",done_job[*thread_num]);
}

int main(int argc , char **argv){
   char file[100] ;
   int i  , d , num[9];
	
   for(i = 0 ; i < 9 ;i++) num[i] = i ;
   printf("Enter the input file name: ") ;
   scanf("%s" , file) ;
   FILE *fin = fopen(file , "r" ) ; 
 
   //read file for multi thread
   fscanf(fin , "%d" , &n) ;
   for(i = 0 ; i < n ; i++){
      fscanf(fin , "%d" , &d) ;
      multi[i] = d ;
   }
   
   //initialize semorphore and data
   for(i = 0 ; i < 9 ; i++){
      sem_init(&s[i] , 0 , 0) ;
   }
   sem_init(&finish , 0 , 0);
   sem_init(&mutex , 0 , 1) ;
   gleft[1] = 0 ;
   gright[1] = n - 1 ;

   //create thread pool and dispacher
   pthread_t multid[9]  ;
   for(i = 1 ; i < 9 ; i++){
      pthread_create(&multid[i] , NULL  , worker_thread , (void *)&num[i]  ) ;
   }
   pthread_create(&multid[0] , NULL , dispatcher , NULL ) ;

   for(thread = 1 ;  thread <= 8 ; thread++){
	for(i = 0 ; i < n ; i++) buffer[i] = multi[i] ;
	for(i = 0 ; i < 9 ; i++){
		thread_state[i] = -1 ;
		done_job[i] = -1 ;
	}
	for(i = 0 ; i < 8 ; i++){
		pairing[i] = 0 ;
	}
	//start timing
	gettimeofday(&startt , 0) ;
	//signal dispatcher
	sem_post(&s[0]) ;
	//wait for finish
	sem_wait(&finish);
	//end timeing
	gettimeofday(&endt , 0) ;
	//print the elapsed time
	int sec = endt.tv_sec - startt.tv_sec ;
	int usec = endt.tv_usec - startt.tv_usec ;
	printf("pool with %d thread elapsed %f s\n" , thread , sec + usec/1000000.0) ;
	//output file
	char outname[13] = "output_n.txt" ;
	outname[7] = thread + '0' ;
	FILE *fout = fopen(outname , "w");
	for(i = 0 ; i < n ;i++) fprintf(fout , "%d " , buffer[i]) ;
   }
   

   //destroy all sem
   for(i = 0 ; i < 9 ; i++){
      sem_destroy(&s[i]) ;
   }
   sem_destroy(&finish) ; 
   sem_destroy(&mutex) ;
   fclose(fin);
   

}
