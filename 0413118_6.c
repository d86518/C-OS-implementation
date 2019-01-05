#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

void getfilename(char * , char *) ;
void find(char *) ;

int opt_inode = 0;
unsigned long int ino=-1 ;
int opt_name  = 0;
char name[256] = {0} ;
int opt_min = 0 ;
int min_size=-1 ;
int opt_max  = 0;
int max_size=-1 ;

int main(int argc , char **argv){
     int i ;
     if(argv[1][strlen(argv[1])-1] == '/') argv[1][strlen(argv[1])-1] = 0 ;
     for(i = 2  ; i < argc ; i+=2 ){
        if(strcmp( argv[i] , "-inode" )== 0 ){
	    opt_inode = 1 ;
	    ino = atol(argv[i+1]) ;
	}
	else if(strcmp(argv[i] , "-name")==0){
	    opt_name = 1 ;
	    strncpy(name ,argv[i+1],sizeof(name)) ;
	}
	else if(strcmp(argv[i] , "-size_min") == 0){
	    opt_min = 1 ;
	    min_size = 1024*1024*atoi(argv[i+1]) ;
	}
	else if(strcmp(argv[i] , "-size_max") == 0){
	    opt_max = 1 ;
	    max_size = 1024*1024*atoi(argv[i+1]) ;
	}
	else{
	    printf("Error options\n") ;exit(-1) ;
	}
     } 
     find(argv[1]) ;   
	
}

void getfilename(char *pathname,char *filename){
     int len = strlen(pathname)  , i;
     for(i = len - 1 ; i >= 0 ; i--) {
          if(pathname[i] == '/') break;
     }
     strncpy(filename , &pathname[i+1] , 256) ;
}
void find(char *pathname){
    struct stat metadata ;
    char filename[256] ;
    stat(pathname , &metadata) ;
    getfilename(pathname , filename) ;

    if( (!opt_inode | (opt_inode && (metadata.st_ino == ino)) ) &&
	(!opt_name  | (opt_name && (strcmp(filename ,name)==0)) )&&
	(!opt_min | (opt_min &&(metadata.st_size >= min_size)) ) &&
	(!opt_max | (opt_max &&(metadata.st_size <= max_size)) )	
      ) 
     printf("%s %lu %f MB %s \n" ,pathname , metadata.st_ino , (float)(metadata.st_size/1048576.0), ctime(&metadata.st_ctime)); //2^20 = 1048576
    if(S_ISREG(metadata.st_mode)) return ;
    if(S_ISDIR(metadata.st_mode)){
    	DIR *directory = opendir(pathname) ;
	if(directory == NULL) {
		printf("Open directory %s failed\n" , pathname) ;exit(-1) ;
	}
	struct dirent *entry ;
	while( (entry = readdir(directory)) != NULL  ){
           if(strcmp(entry -> d_name , ".") == 0 || strcmp(entry -> d_name , "..") == 0 ) continue;
	   char new_path[4096] ;
	   snprintf(new_path , sizeof(new_path) ,"%s/%s" , 
			   pathname ,entry -> d_name ) ;
	  find(new_path) ; 
	}
	return ;
	
    }

}


