#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>

const int Max = 1000;
int n ;
int multi[1000000] ;
int single[1000000];
int gleft[15] , gright[15] ;
sem_t s[15] , s2[15] ,go_single , finish ;
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

void *multi_sort(void *ptr ){
    int *temp = (int *)ptr ;
    int num = *temp ;
    int i;
   	if(num < 8){
   		sem_wait(&s[num-1]);
   		if(num == 1){
        gettimeofday(&startt , 0) ;
      	}  
		//partition 下往上傳 
  		gleft[num*2-1] = gleft[num-1];
		gright[num*2-1] = (gleft[num-1]+gright[num-1])/2;
		gleft[num*2] = (gleft[num-1]+gright[num-1])/2+1;
		gright[num*2] = gright[num-1];
//		printf("%d partition complete \n",num);
		//往下叫 
		sem_post(&s[num*2-1]);
		sem_post(&s[num*2]);
		
		//等merge用的sem   
		sem_wait(&s2[num*2-1]) ; 
        sem_wait(&s2[num*2]);
        
		//等到底下兩個thread結束後merge
		//Merge(,左邊的最右邊,) 
		Merge(multi,gleft[num*2-1],gright[num*2-1],gright[num*2]);
//	    printf("%d merge complete \n",num);
		
		if(num!=1) {
		sem_post(&s2[num-1]);
		}
		else{
		  //time end
	      gettimeofday(&endt , 0) ;
	      //output file
	      FILE *fout = fopen("output1.txt" , "w") ;
	      fprintf(fout , "%d" , multi[0]) ; 
	      for( i = 1 ; i < n ;i++){
	         fprintf(fout , " %d" , multi[i]);
	      }
	      fclose(fout) ; 
	      //output time
	      int sec = endt.tv_sec - startt.tv_sec ; 
	      int usec = endt.tv_usec - startt.tv_usec ;
	      printf("multi thread elapsed %f s\n" , sec + usec/1000000.0 ) ;
	      sem_post(&go_single) ; 
	       //exit thread
	      pthread_exit(0);
		} 
    }
    
    else{
      //num >= 8 , bubblesort
      sem_wait(&s[num-1]);
//	  printf("%d partition complete \n",num);
	  bubble(multi , gleft[num-1] , gright[num-1]) ;
      sem_post(&s2[num-1]);  
	  //exit thread
	  pthread_exit(0);      
    }
  
}
void *single_sort(void *ptr){	
  	//wait for multi finish 
    sem_wait(&go_single) ; 
    //time start
    gettimeofday(&startt , 0) ; 
    int i;
    for(i = 1 ; i <= 7 ; i++){
		//partition 下往上傳 
  		gleft[i*2-1] = gleft[i-1];
		gright[i*2-1] = (gleft[i-1]+gright[i-1])/2;
		gleft[i*2] = (gleft[i-1]+gright[i-1])/2+1;
		gright[i*2] = gright[i-1];
	}
	   //do bubble sort
    for(i = 8 ; i <= 15 ; i++){
       bubble(single , gleft[i-1] , gright[i-1]) ;
    }
    
    for(i = 7 ; i >= 1 ; i--){
		Merge(single,gleft[i*2-1],gright[i*2-1],gright[i*2]);
	} 	
	
    //time end
    gettimeofday(&endt , 0) ;
    //output file
    FILE *fout = fopen("output2.txt" , "w") ;
    i = 0 ; 
    fprintf(fout , "%d" , single[i]) ; 
    for(i = 1 ; i < n ;i++){
       fprintf(fout , " %d" , single[i]) ;
    }
    fclose(fout) ;
    //output time
    int sec = endt.tv_sec - startt.tv_sec ; 
    int usec = endt.tv_usec - startt.tv_usec ;
    printf("single thread elapsed %f s\n" , sec + usec/1000000.0 ) ; 
    //signal finish
    sem_post(&finish);
    //pthread exit
    pthread_exit(0) ;
}

int main(int argc , char **argv){
   char file[100] ;
   int i , num[15] , d;
   for(i = 0 ; i < 15 ; i++) num[i]= i+1 ;
   printf("Enter the input file name: ") ;
   scanf("%s" , file) ;
   FILE *fin = fopen(file , "r" ) ; 
    
   //read file for multi thread
   fscanf(fin , "%d" , &n);
   
   
   for(i = 0 ; i < n ; i++){
      fscanf(fin , "%d" , &d) ;
      multi[i] = d ;
      single[i] = d;
   }
   
   	//紀錄 array切開的index 
	gleft[0] = 0;
	gright[0] = n-1;
   
   for(i = 0 ; i < 15 ; i++){
   	//前15個partition用，後15個merge用 
      sem_init(&s[i] , 0 , 0) ;
      sem_init(&s2[i] , 0 , 0) ;
   }
   sem_init(&finish , 0 , 0);
   sem_init(&go_single , 0 , 0) ;
   pthread_t multid[15] , sintid ;
   for(i = 0 ; i < 15 ; i++){
      pthread_create(&multid[i] , NULL  , multi_sort , (void *) &num[i]  ) ;
   }
   pthread_create(&sintid , NULL , single_sort , NULL ) ;
   
   //signal 
   sem_post(&s[0]);
   
   //wait single finish
   sem_wait(&finish) ; 
   //destroy all sem
   for(i = 0 ; i < 15 ; i++){
      sem_destroy(&s[i]) ;
      sem_destroy(&s2[i]) ;
   }
   sem_destroy(&go_single) ;
   sem_destroy(&finish) ; 

   fclose(fin);
}
