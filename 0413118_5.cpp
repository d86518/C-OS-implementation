#include<stdlib.h>
#include<stdio.h>
#include<map>
#include<queue>
#include<string>
#include<sys/time.h>

using namespace std ;
struct NODE{
    string key ;
    struct NODE *prv ;
    struct NODE *next ;
};


struct comparator{
    bool operator()(pair<int , int> *lhs ,pair<int, int> *rhs){
        if(lhs->first < rhs->first){
    		return true ;
		}else if(lhs->first == rhs->first){
			return (lhs->second< rhs->second );
		}else{
			return false;
		}
    }
};
struct NODE *head = NULL , *tail = NULL ;

void insert(struct NODE *node) ;
void remove(struct NODE *node) ;
void clear() ;
string str[11000000] ;

int main(){
   struct timeval startt ,endt ,startt2 ,endt2; 
   int frame ,i = 0 , j ;
   FILE *fp = fopen("sample.txt" , "r") ;
   char s[20] ;
   while(!feof(fp)){
       fscanf(fp , "%s\n" , s) ;
       string t(s) ;
       if(t.length()>0){
	   str[i] = t ;
       i++ ;
       }
   }
   fclose(fp) ;
    //LFU
    printf("LFU policy:\nframe \t hit \t\t miss \t\t page fault ratio\n") ;
    gettimeofday(&startt , 0) ;
	for(frame = 128 ; frame <= 1024 ;frame*=2){
       int miss=0 , hit=0 ;
       map<string , pair<int , int> * >lfu ;
       map<pair<int , int>*, string ,comparator > pr ;

       for(j = 0 ; j < i ; j++){
           //check miss or hit
           if(lfu.find(str[j]) != lfu.end() ){
                hit++ ;
                pair<int ,int> *temp = lfu[str[j]] ;
                pr.erase(temp) ;
                temp -> first++ ;
                pr[temp] = str[j] ;

            }else{
                miss++ ;
                pair<int ,int> *p = new pair<int,int> ;
                p -> first = 1 ;
                p -> second = j ;
               if(lfu.size() < frame){
               }else{
                 pair<int ,int> *victim = pr.begin()->first ;
                 lfu.erase(pr.begin()->second) ;
                 pr.erase(pr.begin()->first) ;
                 delete victim ;
               }
               lfu[str[j]]=p ;
               pr[p] = str[j] ;
           }
       }
      printf("%d \t %d \t %d \t %.10f\n" ,frame , hit ,miss , (double)miss / (miss +hit)) ;
    }
    gettimeofday(&endt , 0) ;
    int sec = endt.tv_sec - startt.tv_sec ;
    int usec = endt.tv_usec - startt.tv_usec ;
	printf("Total elapsed time: %.4f sec\n\n",sec + usec/1000000.0);
	//LRU
    printf("LRU policy:\nframe \t hit \t\t miss \t\t page fault ratio\n") ;
    gettimeofday(&startt2 , 0) ;
	for(frame = 128 ; frame <= 1024 ; frame *= 2){
          int hit = 0 , miss = 0 ;
          head =  NULL ; tail = NULL ;
          map<string , struct NODE *> lru ;
          for(j = 0 ; j < i ; j++){
              if(lru.find(str[j]) != lru.end()){
                 hit++ ;
                 remove(lru[str[j]]) ;
                 insert(lru[str[j]]) ;
              }else{
                 miss++ ;
                 struct NODE *new_node = new NODE ;
                  new_node -> key = str[j] ;
                 if(lru.size() < frame){
                      insert(new_node) ;
                  }else{
                      struct NODE *victim = tail ;
                      remove(victim) ;
                      lru.erase(victim -> key) ;
                      delete victim ;
                      insert(new_node) ;
                  }
                  lru[str[j]] = new_node ;
              }
          }
        printf("%d \t %d \t %d \t %.10f\n" ,frame ,hit , miss  , (double)miss / (miss +hit)) ;
        clear() ;
     }
     gettimeofday(&endt2 , 0) ;
     int sec2 = endt2.tv_sec - startt2.tv_sec ;
     int usec2 = endt2.tv_usec - startt2.tv_usec ;
	 printf("Total elapsed time: %.4f sec\n",sec2 + usec2/1000000.0);

}

void insert(struct NODE *node){
    node->next = head ;
    if(head == NULL) tail = node ;
    else head->prv = node ;
    node->prv = NULL ;
    head = node ;
}
void remove(struct NODE *node){
     if(node -> prv == NULL) head = node -> next ;
     else node -> prv -> next = node -> next ;
     if(node -> next == NULL) tail = node -> prv ;
     else node -> next -> prv = node -> prv ;

     node -> next = NULL ;
     node -> prv = NULL ;
}
void clear(){
     while(head != NULL){
        struct NODE *temp = head ;
	remove(temp) ;
        delete temp ;
	}
}
