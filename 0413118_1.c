#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
//#include<iostream>

//using namespace std;

int main(){
	printf(">");
	char buf[1000];
	pid_t pid;
	//字串存放位置,大小,方法
	while(fgets(buf , sizeof(buf) , stdin)){
	  char *argv[20] = {0} ;
		//把enter拿掉
	  char *ptr = strtok(buf , "\n") ;
	  int i = 0 ;
	  int instant = 0 , redirect= 0 , pip = 0 ;
	  ptr = strtok(buf , " ");
	  while(ptr != NULL){
      //&不用等child process 結束
      if(strcmp(ptr , "&") == 0 ){instant = 1 ;  break;}
      //將內容存取從stdout redirect
      if(strcmp(ptr, ">") == 0 ){redirect = 1; break ;}
      //一個程式的output當另一個程式input
      if(strcmp(ptr, "|") == 0 ){pip = 1; break ;}
      argv[i] = ptr ;
      i++;
      //從上次切完的位置往下切
      ptr = strtok(NULL , " ");
	  }
	if(pip == 1){
		//pipe file descriptor
		int pfd[2] ;
		//建連線 0是readonly 1是writeonly/input
		if(pipe(pfd) < 0 ){
      fprintf(stderr , "Pipe creation Faile\n") ; 
      exit(0);
      }
		if( (pid = fork() )< 0 ){
      fprintf(stderr , "Pipe fork Error\n");
      exit(0);
      }
		else if(pid == 0){
		//child process
		   close(pfd[0]) ;
			 //關output口 開input口
		   dup2(pfd[1] , 1) ;//1是stdout 原本要輸出到螢幕的丟到pfd[1]
		   execvp(argv[0] , argv); //執行前面的動作
		   fprintf(stderr , "pre command error\n");
		   exit(0) ;
		}
		else{
		//parent process
		   pid_t sib_pid = fork() ;
		   if(sib_pid < 0){
         fprintf(stderr , "Sibling Fork Error\n") ;
         exit(0) ;
         }
		   else if(sib_pid == 0){
			//sibling process
			char *argv2[20] = {0} ;
			i = 0 ;
			ptr = strtok(NULL , " ");
			while(ptr != NULL){
				argv2[i] = ptr ;
				i++;
				ptr = strtok(NULL , " ") ;
				}
			close(pfd[1]) ;
			//把寫的那端關掉 開readonly 把pre command送過來
			dup2(pfd[0] , 0) ;
			execvp(argv2[0] ,argv2) ;
			fprintf(stderr ,"post command error\n") ;
		  exit(0);
			}
		  else{
			//parent process
			close(pfd[0]) ;
			close(pfd[1]) ;
			waitpid(pid ,NULL , 0) ;
			waitpid(sib_pid , NULL ,0) ;
			printf(">");
	 		}
	  }
	}
	else if((pid = fork() ) < 0){ 
    fprintf(stderr,"Fork Error\n");
    exit(0);
    }
	else if(pid == 0) {
	  //child process
	 if(redirect == 1){
		 //redirection把一個程式的oupput寫到檔案裡面
		char *filename;
		int fd ;
		filename = strtok(NULL , " ") ;
		fd = open(filename , O_WRONLY | O_CREAT | O_TRUNC , 0666);
		//return file descriptor index會+上去
		if(fd < 0) {
      fprintf(stderr , "Open file fails\n") ; 
      exit(0) ;
      }
		//原本寫到stdout的內容寫到fd
		dup2(fd , 1) ;
		close(fd);
		if(execvp(argv[0] , argv) < 0){
      fprintf(stderr , "command error\n");
      exit(0) ;}
		}
	 else if(instant == 1){
// & 不用到child process
		pid_t pid2  = fork() ;
//pid2子孫
		if(pid2 == 0){
			if(execvp(argv[0] , argv) < 0) {
				fprintf(stderr , "command error\n");exit(0);
				}
		 }
		 //兒子就離開
		 //孫子在做的時候 parent就可以坐下件事 不用等
		else if(pid2 > 0) exit(1) ;
		else { fprintf(stderr,"Fork2 Error\n") ;exit(0);}
		}
	  else if(execvp(argv[0] , argv) < 0 ) {
      fprintf(stderr , "command error\n") ;
      exit(0);}
	 }
	 else{
	 //parent process
	 wait(NULL) ;
	 printf(">"); 
		}
	}
	return 0 ;
}
