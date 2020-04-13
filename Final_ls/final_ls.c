///////////////////////////////////////////////////////////
// File Name       : Final ls	                         //  
// Date            : 2019/04/26                          //                                           
// Os              : Ubuntu 16.04 LTS 64bits             //                                            
// Author          : Han Sung Min                        //                                           
// Student ID      : 2015722088                          //
// ------------------------------------------------------//    
// Title : System Programing Assignment #2-3(Final ls)   //
// Decription: ...                                       //
///////////////////////////////////////////////////////////

# include <stdio.h>
# include <dirent.h>
# include <stdlib.h>//header for dynamic alloacation
# include <string.h>
# include <unistd.h>
# include <sys/stat.h>//header for struct stat
# include <pwd.h>//header for password 
# include <grp.h>//header for group
# include <time.h>///header for time
# include <string.h>
# include <fnmatch.h>//header for fnmatch
///////////////////////////////////////////////////////////
// _Node                                                 //
// ====================================================  //
// name ==> for keeping file name			 //
// buf ==> for keeping file information by stat structure//
// Node * next ==> for pointing next node 		 //
///////////////////////////////////////////////////////////
typedef struct _Node{
	char 	name[255];
	struct stat buf;//for stat information
	struct _Node *next;//for keep next node
	struct _Node *prev;//for keep preve node
}Node;

/////////////////////////////////////////////////////////
// sort_list 	                                       //
// ====================================================//
// p ==> Head node information			       //
// a ==> new node                                      //
// b ==> new file stat information                     //
// Sflg==>is there Sflag?                              //
// input: pNew -> for new node                         //
//        pPrev -> have prev point		       //	
// output: Node* --> node Head address                 //
// Purpose: Inserting and sorting node                 //
/////////////////////////////////////////////////////////
Node* sort_list(Node *p, struct dirent *a, struct stat * b, int Sflg){//func of sort_list
	Node *pTemp =NULL;
	Node *pNew  = NULL;
	Node *pPrev = NULL;
	Node *pCur = NULL;
	Node *pCurTemp =NULL;
	pNew =(Node*) malloc(sizeof(Node));//make dynamic allocation for new node
	strcpy(pNew->name,a->d_name);//make copy new file's name
	pNew->buf = *b;//insert new stat information to node
	pNew->next = NULL;
	pNew->prev =NULL;
	pTemp = p;
	pPrev = p;//intialize pTemp pPrev pointing origin Head address
	if(Sflg!=1){
	if(p == NULL){//first HEAD node
		p = pNew;
	}
	else if(p->next == NULL){
		if(strcasecmp(p->name,a->d_name)>0){//cmp for sorting
			pNew->next = pTemp;
			pTemp->next=NULL;
			pTemp->prev= pNew;
			p = pNew;
		}
		else{
			p->next = pNew;
			pNew->next = NULL;
			pNew->prev = p; 
		}	
	}
	else{//after setting
	for(;;){
		if(strcasecmp(p->name,a->d_name)>0){//cmp for sorting
			pNew->next = pTemp;
			pTemp->prev = pNew;
			p = pNew;
			return p;//return new Head address
		}
		pPrev = pTemp;
		pTemp = pTemp->next;
		if(pTemp == NULL) break;
		if(strcasecmp(pTemp->name,a->d_name)>0){//cmp for sorting
			pPrev->next = pNew;
			pNew->prev = pPrev;
			pNew->next = pTemp;
			pTemp->prev = pNew;
			return p;//return new Head address
		}
	}
	pNew->prev = pPrev;
	pPrev->next = pNew;
	}
	return p;//return new Head address
	}
	else if(Sflg ==1){//if Sflag is 1
		if(p == NULL){//first HEAD node struct stat *b -->new node information 
        	        p = pNew;
			return p;
       		 }
   	        else if(p->next == NULL){
              	  if(((p->buf.st_size)-(pNew->buf.st_size))<0){//cmp for sorting p<pNew
              	          pNew->next = pTemp;
              	          pTemp->next=NULL;
              	          pTemp->prev= pNew;
              	          p = pNew;
			 return p;
              	      }
              	  else{//p>=pNew
			if(((p->buf.st_size)-(pNew->buf.st_size))==0){//p==pNew
			      if(strcasecmp(p->name,a->d_name)>0){//Alpha cmp p >a
                       		 pNew->next = pTemp;
                      		  pTemp->next=NULL;
                     		  pTemp->prev= pNew;
                       		 p = pNew;
				return p;
              	               }
		 	        else{//Alpha cmp a>=p
					p->next = pNew;
                        	        pNew->next = NULL;
                        	        pNew->prev = p;
                        	        return p;
				 }
			}
              		else{//p>pNew;
                	        p->next = pNew;
                	        pNew->next = NULL;
                	        pNew->prev = p;
				return p;
               	 	    }
        	  }
		}
        else{//after setting 
        for(;;){
                if(((p->buf.st_size)-(pNew->buf.st_size))<0){//cmp for sorting size pNew > size p
			pNew -> next = p;
			p ->prev = pNew;
			p = pNew;
                        return p;//return new Head address
                }
		else if(((p->buf.st_size)-(pNew->buf.st_size))==0){//size p == size pNew
			if(strcasecmp(p->name,a->d_name)>0){//cmp for sorting alpha p > alpha a
		 	  pNew -> next = p;
			  p ->prev = pNew;
			  p = pNew;	
                      	  return p;//return new Head address
			}
		}
                pPrev = pTemp;// pPrev has head address
                pTemp = pTemp->next;//pTemp has head ->next address
		pCur = pTemp;//pCur has head ->next address
                if(pTemp == NULL) break;
                if(((pTemp->buf.st_size)-(pNew->buf.st_size))<0){//cmp for sorting        strcasecmp(pTemp->name,a->d_name)>0
			pPrev->next = pNew;
                       pNew->prev = pPrev;
                        pNew->next = pTemp;
                       pTemp->prev = pNew;
                        return p;//return new Head address
                }
		else if(((pTemp->buf.st_size)-(pNew->buf.st_size))==0){//cmp for sorting        strcasecmp(pTemp->name,a->d_name)>0
			 if(strcasecmp(pTemp->name,a->d_name)>0){//cmp for sorting
                    	    pPrev->next = pNew;
                    	    pNew->prev = pPrev;
                    	    pNew->next = pTemp;
                    	    pTemp->prev = pNew;
                    	    return p;//return new Head address
        	       	}
			else{//////////////////////////////////////////////////////////////
			  for(;((pCur->buf.st_size)-(pNew->buf.st_size))==0;){ 
				if(strcasecmp(pCur->name,a->d_name)<0){
						if(pCur->next == NULL){
						  pCur->next = pNew;
						  pNew->prev = pCur;
						  return p;  
						}
						pCur = pCur ->next;
						continue;
				} 
				else{ 
					pNew->next =pCur;
					pCurTemp = pCur->prev;
					pCur->prev =pNew;
					pCur = pCurTemp;
					pCur->next =pNew;
					pNew->prev = pCur;
					return p;
				 }	
		         	}
			}
 	     	}
	 	}//for(;;) the end 
	   pNew->prev = pPrev;
           pPrev->next = pNew;   
        }
        return p;//return new Head ad	
	}
}

/////////////////////////////////////////////////////////
// reverse_node_swap                                   //
// ====================================================//
// p ==> Head node information                         //
// input: temp -> for moving node                      //
//  changed_node_Head -> Node for changing             //       
// output: Node* --> node Head address                 //
// Purpose: re sorting for reverse case                //
/////////////////////////////////////////////////////////
Node* reverse_node_swap(Node* p){
	Node* temp = NULL;
	Node* changed_node_Head =NULL;
	temp = p;
	changed_node_Head = p;
	while(temp->next!=NULL){
		temp = temp->next;
	}//by this loop temp has last node of Head p
	changed_node_Head = temp;//node address for reversed node Head
	while(temp->prev != p){//looping  for reversed 
		temp ->next = temp->prev;
		temp = temp->prev;
	}
		temp ->next = temp->prev;
		temp = temp->next;
		p ->next =NULL;
	return changed_node_Head;//retunr new address
}

/////////////////////////////////////////////////////////
// print_type                                          //
// ====================================================//
// stat a --> argument that has file information       //
// output: printf(print type)			       //
// purpose: print file type			       //			
/////////////////////////////////////////////////////////
void print_type(struct stat  a){
	char c;
	if(S_ISREG(a.st_mode))
		c = '-';
	else if(S_ISDIR(a.st_mode))
		c = 'd';
	else if(S_ISCHR(a.st_mode))
		c = 'c';
	else if(S_ISBLK(a.st_mode))
		c = 'b';
	else if(S_ISLNK(a.st_mode))
		c = 'l';
	else if(S_ISSOCK(a.st_mode))
		c = 's';
	
	printf("%c\t",c);
}
/////////////////////////////////////////////////////////
// print_permit                                        //
// ====================================================//
// stat a --> argument that has file information       //
// output: printf(print permit)                        //
// purpose: print file permit                          //                       
/////////////////////////////////////////////////////////
void print_permit(struct stat a){
	mode_t tmp;
	tmp = a.st_mode;
	for(int i=0; i<3; i++){
		if(tmp & S_IRUSR)
			printf("r");
		else 
			printf("-");

		if(tmp & S_IWUSR)
			printf("w");
		else 
			printf("-");
		
		if(tmp & S_IXUSR)
			printf("x");
		else
			printf("-");

		tmp <<=3;
	}
}	
/////////////////////////////////////////////////////////
// print_ID(user.group)                                //
// ====================================================//
// stat a --> argument that has file information       //
// output: printf(print ID) 	                       //
// purpose: print file user or group ID                //                       
/////////////////////////////////////////////////////////
void print_userID(struct stat a){
	struct passwd *pwd;
	pwd=getpwuid(a.st_uid);//get ID information
	if((pwd=getpwuid(a.st_uid))!=NULL){
	printf("%s\t",pwd->pw_name);
	}
	else 
	printf("Wrong input\n");
}
void print_groupID(struct stat a){
	struct group *grp;
	if((grp =getgrgid(a.st_uid))!=NULL){//get ID Information
	printf("%s\t",grp->gr_name);
	}
	else
	printf("Wrong input\n");
}
/////////////////////////////////////////////////////////
// print_time 	                                       //
// ====================================================//
// stat a --> argument that has file information       //
// output: printf(print time)                          //
// purpose: print file time information                //                       
/////////////////////////////////////////////////////////
void print_time(struct stat a){
	struct tm * ptm;
	ptm = localtime(&(a.st_mtime));//get time information
	switch(ptm->tm_mon){
	       case 0: printf("Jan");
                      break;
               case 1: printf("Feb");
                      break;
               case 2: printf("Mar");
                      break;
               case 3: printf("Apr");
                      break;
               case 4: printf("May");
                      break;
               case 5: printf("Jun");
                      break;
               case 6: printf("Jul");
                      break;
               case 7: printf("Aug");
                      break;
               case 8: printf("Sep");
                      break;
               case 9: printf("Oct");
                      break;
               case 10: printf("Nov");
                      break;
               case 11: printf("Dec");
                      break;
	}
	printf(" %02d %02d %02d\t",ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
}	
/////////////////////////////////////////////////////////
// print_size 	                                       //
// ====================================================//
// stat a -->argument that has file information        //
// int hflg-->argument that has hflag info	       //
// output: return;                                     //
// purpose: for printing file size                     //                       
/////////////////////////////////////////////////////////
void print_size(struct stat a,int hflg){
	if(hflg==1){
		if( (unsigned int)(a.st_size)/(1024 * 1024 * 1024) !=0){
			printf("%.1fG\t",(float)a.st_size/(1024*1024*1024));	
		}
		else if( (unsigned int)(a.st_size)/(1024 * 1024) !=0){
                        printf("%.1fM\t",(float)a.st_size/(1024*1024));
                }
		else if( (unsigned int)(a.st_size)/(1024) !=0){
                        printf("%.1fK\t",(float)a.st_size/(1024));
                }
		else {
			printf("%u\t",(unsigned int)a.st_size);
		}

	}
	else{
		printf("%u\t",(unsigned int)a.st_size);
	}
	return ;
}


/////////////////////////////////////////////////////////
// two_Array_free                                      //
// ====================================================//
// Node * remove_node --> node  for free               //
// output: return;                                     //
// purpose: for node free   			       //                       
/////////////////////////////////////////////////////////
void free_node(Node * array){

	Node * remove_node;
	for(int i=0; array!=NULL;i++){
		remove_node = array;
		array = array->next;
		free(remove_node);
	}	
	return;
}
/////////////////////////////////////////////////////////
// inspecting_wild                                     //
// ====================================================//
// a for inspecting file                               //
// char* inspecting ==> value for inspecting           //
// purpose: for inspecing wildcard, passing argument   //                       
/////////////////////////////////////////////////////////
char* inspecting_wild(char*a){
	char * inspecting =NULL;
	inspecting = strchr(a,'*');//inspecting * case
	if(inspecting !=NULL){
		return a;
	}
	inspecting = strchr(a,'[');//inspecting [sequence] case
	if(inspecting !=NULL){
		inspecting = strchr(inspecting,']');
		if(inspecting !=NULL){
			return a;
		}
	}
	inspecting = strchr(a,'?');//inspecting ? case
	if(inspecting !=NULL){
		return a;
	}
	return NULL;		
}
/////////////////////////////////////////////////////////
// is_is_wild_path                                     //
// ====================================================//
// a for inspecting file                               //
// b for keep wildcard or passing argument count       //
// c for count      ==> value for inspecting           //
/////////////////////////////////////////////////////////
int is_it_wild_path(char *a, char**b,int c){
	
	for(int i=0; i<c;i++){
		if((strcmp(a,b[i])==0)) return 1;
	}
	return 0;
}
/////////////////////////////////////////////////////////
// except_node                                         //
// ====================================================//
// a Node of Head  	                               //
// b Node of exception				       //
/////////////////////////////////////////////////////////
Node* except_node(Node *a, Node *b){
	Node * Cur = a;
	if(Cur == b){Cur = Cur ->next; return Cur;}
	while(Cur ->next != b && Cur !=b){
	  Cur = Cur ->next;
	}
	
	Cur ->next = b->next;
	return a;
}

/////////////////////////////////////Main function///////////////////////////////
int main(int argc, char * argv[]){
	DIR *drip;//for stream
	struct dirent *dir;//for directory
	char **wd;/////input To work dirctory
	char **fi_info;//for keeping correct file input 
	char cwd[255];//for print current directory
	Node *Head=NULL;//for node list Head addiress
	Node *Cur = NULL;//for iterate node list
	Node *abCur =NULL;
	Node *Head_ab_print =NULL;
	int aflag =0, lflag=0, hflag=0, rflag=0, Sflag=0;//for flag
	int c=0;//for getting flag or for using function stop signal
	struct stat file_info;//for getting file info
	struct stat file_info2;//for getting file info
	int count =1;//for counting
	long int b_count=0;//for block counting
	char fwd[255];//first working directory
	int fi_count=0;//for correct file counting
	char *temp = NULL;
	int fi_wd_memory =0;//for keep memory sizeof wd and fi_info
	char *op_find =NULL;//for option find
	int w_card_count =0;//for wild card counting
	char **w_card =NULL; // for keeping  wild card input data
	Node *w_c_Head =NULL;//for keep wild card node Head address
	char * ab_path_tok =NULL;//for ab_path tokkenize
	int is_star_last =0;//for wild card is absolut or not?
	char * pattern = NULL;//for keep pattern
	char keep_ap_path[255];//for keeping absolute path
////////////////////////////loop for option setting/////////////////////////////////////
	while((c = getopt(argc,argv,"alhrS")) != -1)//getting flag information
	{
		count ++;//counting file argument 
		switch(c)
		{
		case 'a':
			aflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'h':
			hflag =	1;
			break;
		case 'r':
			rflag = 1;
			break;
		case 'S':
			Sflag = 1;
			break;
		case '?': 
			return 0;
		}
	}
///////////////////////////End of option setting loop///////////////////////////////////
	getcwd(fwd,255);	
	count = argc - count;//real file argument to make dynamic allocation
	wd = (char**) malloc(sizeof(char*)*255);
	fi_info = (char **) malloc(sizeof(char*)*255);
	fi_wd_memory = count;//save count to fi_wd_memory for free wd fi_info memory
	count =0;//count reset
	for(int i=1; argv[i]!=NULL;i++){//find real file except -option
		op_find = strchr(argv[i],'-');//for inspecting not option input
		if(op_find !=NULL){//if it is option input
			if((op_find = strchr(argv[i],'['))!=NULL){
				wd[count] = argv[i];	
				count ++;	
				}		
		continue;
		}		
		wd[count] = argv[i];//else this input is not option input
		count ++;
	}

////////////////////////loop for counting wild card argument///////////////////	
	for(int i=0; i<count;i++){
		if(inspecting_wild(wd[i])!=NULL){
			w_card_count++;}
	}	
///////////////////////End of loop/////////////////////////////////////////

	w_card = (char**) malloc(sizeof(char*)*w_card_count);//dynmaic allocation for wildcard keeping

/////////////////////////Loop for keeping wild card input//////////////
	w_card_count =0;
	for(int i=0; i<count;i++){
		if(inspecting_wild(wd[i])!=NULL){
			w_card[w_card_count] = wd[i];
			w_card_count ++;
			}
	}
////////////////////////End of loop////////////////////////////////////

/////////////////////////////////////loop for file inspecting not directory///////////////////
	for(int i=0; i<=count; i++){ 
		if(count ==0) break;
		if(i == count) break;
		c =0;
		drip = opendir(wd[i]);//open directory for inspecting file
		 if(drip == NULL){//not direc just input file
                        drip = opendir(".");//open current directory
                        while((dir = readdir(drip))!=NULL){
                                if((strcmp(dir->d_name,wd[i]))==0){
                                        c++;  
					fi_info[fi_count] = wd[i];//keeping the correct file name
					fi_count ++;
					break;
                                }
                        }
                        rewinddir(drip);
                if(c==0){
			if((is_it_wild_path(wd[i],w_card,w_card_count))==0){
                         printf("cannot access %s: No such file or directory\n",wd[i]);}
                        continue;}//printf message if is error
		closedir(drip);//close directory
		}
	}

/////////////////////////////////////End of loop for file printing not directory///////////////
	
/////////////////////////////////////Loop for sorting the file//////////////////////
	if(fi_count !=0){
	    if(Sflag ==1){//case for Sflag on	
		for(int i=0; i<fi_count;i++){//loop for Sflag sort
			lstat(fi_info[i],&file_info);//input compare file stat
                        for(int j=i; j<fi_count;j++){
				lstat(fi_info[j],&file_info2);//input compare file stat
                                if((file_info.st_size)<(file_info2.st_size)){//compare by size
                                        temp = fi_info[i];
                                        fi_info[i] = fi_info[j];
                                        fi_info[j] = temp;
					lstat(fi_info[i],&file_info);
                                }
				else if((file_info.st_size)==(file_info2.st_size)){//if file size same
                                 if(strcasecmp(fi_info[i],fi_info[j])>0){//sorting by name 
				       temp = fi_info[i];
                                        fi_info[i] = fi_info[j];
                                        fi_info[j] = temp;
				}
                                }

                        }
                }
		if(rflag ==1){//case for r S flag on
		for(int i=0; i<fi_count/2; i++){//sort reverse 
				temp = fi_info[i];
                                fi_info[i] = fi_info[fi_count-1-i];
                                fi_info[fi_count-1-i] = temp;
                        }
		}
	}
	else if(Sflag!=1&&rflag==1){//case rflag on
		 for(int i=0; i<fi_count;i++){//firstly sorting by name
                        for(int j=i; j<fi_count;j++){
                                if(strcasecmp(fi_info[i],fi_info[j])>0){
                                        temp = fi_info[i];
                                        fi_info[i] = fi_info[j];
                                        fi_info[j] = temp;
                                }
                        }
                }
		 for(int i=0; i<fi_count/2; i++){//sort reverse
                                temp = fi_info[i];
                                fi_info[i] = fi_info[fi_count-1-i];
                                fi_info[fi_count-1-i] = temp;
                        }
	}
	else if(Sflag!=1 && rflag!=1){//theres is not -r S option
              for(int i=0; i<fi_count;i++){
                        for(int j=i; j<fi_count;j++){
                                if(strcasecmp(fi_info[i],fi_info[j])>0){
                                        temp = fi_info[i];
                                        fi_info[i] = fi_info[j];
                                        fi_info[j] = temp;              
                                }
                        }       
                }
	}
	}
////////////////////////////////////End loop for sorting///////////////////////////////// 


/////////////////////////////////////Loop for printing correct file /////////////////////////
	if(fi_count!=0){
	for(int i=0; i<fi_count; i++){
		lstat(fi_info[i],&file_info);
                        if(aflag==0 && lflag == 1){
                        print_type(file_info);
                        print_permit(file_info);
                        printf("\t%u\t",(unsigned int)file_info.st_nlink);
                        print_userID(file_info);
                        print_groupID(file_info);
                       // printf("%u\t",(unsigned int)file_info.st_size);
			print_size(file_info,hflag);
                        print_time(file_info);
                        printf("%s\n",fi_info[i]);
                        }
                        else if(aflag ==1 && lflag ==1){
                        print_type(file_info);
                        print_permit(file_info);
                        printf("\t%u\t",(unsigned int)file_info.st_nlink);
                        print_userID(file_info);
                        print_groupID(file_info);
                       // printf("%u\t",(unsigned int)file_info.st_size);
			print_size(file_info,hflag);
                        print_time(file_info);
                        printf("%s\n",fi_info[i]);
                        }
                        else if(aflag ==0 && lflag ==0){
                        printf("%s\n",fi_info[i]);
                        }
                        else if(aflag ==1 && lflag ==0){
                        printf("%s\n",fi_info[i]);
                        }
	}
	}
////////////////////////////////End for loop/////////////////////////////////	

///////////////////////////////////loop for directory input printing//////////////////
	for(int i=0;i<=count;i++){ 
	Head = NULL;
	if(count == 0) drip = opendir(".");//if input is none
	else if(i==count) break;
	else{//count reset
		drip = opendir(wd[i]);//if there is input, open directory
		chdir(wd[i]); //char to that directory
	}
	if(drip ==NULL) continue;
	///////////if for casing by flag to make node list
	if(lflag == 1 && aflag ==0){
		while((dir = readdir(drip))!=NULL){
		     if(dir->d_name[0] !='.'){//except hidden file
                                lstat(dir->d_name,&file_info);//get file info
                                Head=sort_list(Head,dir,&file_info,Sflag);//make node
                        }
                }
	}		
	else if(lflag ==0 && aflag ==1){
		while((dir = readdir(drip))!=NULL){
			lstat(dir->d_name,&file_info);//get file info
			Head = sort_list(Head,dir,&file_info,Sflag);//makde node
			}
	}
	else if(lflag ==1 && aflag ==1){
		while((dir = readdir(drip))!=NULL){
			lstat(dir->d_name,&file_info);//get file info
			Head = sort_list(Head,dir,&file_info,Sflag);//make node
			}
	}
	else if(lflag ==0 && aflag ==0){
		while((dir = readdir(drip))!=NULL){
			if(dir->d_name[0] !='.'){//except hidden file
				lstat(dir->d_name,&file_info);//get file info
				Head=sort_list(Head,dir,&file_info,Sflag);//make node
			}
		}	
	}
        if(rflag==1){//case rflag on
		Head=reverse_node_swap(Head);	
	}
	Cur = Head;
//////////////////////////////////////////////////////////////////////////////////////////////////////	

	Cur = Head;
	rewinddir(drip);
	b_count =0;
	while(Cur!=NULL){//counting 512mb blocks and make 1024 blocks
		b_count =b_count+ Cur->buf.st_blocks;	
		Cur = Cur ->next;
	}
	Cur = Head;
///////////////////////if casing by flag to print information/////////////////////
	if(aflag==0 && lflag == 1){
		if(count ==0){//if there is no input
			getcwd(cwd,255);
			printf("Directory path: %s\n",cwd);
		}
		else{//if there is directory input
			printf("Directory path: %s\n",wd[i]);
		}
		printf("total: %ld\n",b_count/2);//printf total information
		while(Cur!=NULL){//print each information
			print_type(Cur->buf);
			print_permit(Cur->buf);
			printf("\t%u\t",(unsigned int)Cur->buf.st_nlink);
			print_userID(Cur->buf);
			print_groupID(Cur->buf);
		//	printf("%u\t",(unsigned int)Cur->buf.st_size);
			print_size(Cur ->buf,hflag);
			print_time(Cur->buf);		
			printf("%s\n",Cur->name);
			Cur = Cur -> next;
			}								
	}
	else if(aflag ==1 && lflag ==1){
		 if(count ==0){//if there is no input
                        getcwd(cwd,255);
                        printf("Directory path: %s\n",cwd);
                }
                else{//if there is directory input
                printf("Directory path: %s\n",wd[i]);
		}
		printf("total: %ld\n",b_count/2);//printf total information
		while(Cur!=NULL){//print each information
			print_type(Cur->buf);
			print_permit(Cur->buf);
			printf("\t%u\t",(unsigned int)Cur->buf.st_nlink);
			print_userID(Cur->buf);
			print_groupID(Cur->buf);
		//	printf("%u\t",(unsigned int)Cur->buf.st_size);
			print_size(Cur->buf,hflag);
			print_time(Cur->buf);
			printf("%s\n",Cur->name);
			Cur = Cur->next;
		}
	}
	else{//there is no lfalg 
	if(count>1){
                 printf("Directory path: %s\n",wd[i]);
                }
	while(Cur!=NULL){
		printf("%s\n",Cur->name);
		Cur = Cur->next;
	}
	}
//////////////////////end of casing ////////////////////////////
	chdir(fwd);//chage direcotry to first directory ==>reset directory
	}
//////////////////////////////////////////////End of loop///////////////////////////////
	closedir(drip);
	dir = NULL;
///////////////////////////////////////////////case for wild card option ///////////////////////////////////////////////////////////////
	if(w_card_count!=0){//if there is wild card
		for(int i=0; i<w_card_count;i++){//loop until wild card input over
			if((fnmatch("*/*",w_card[i],0)) ==0){//case for absolute path
				ab_path_tok = w_card[i];//keep w_card[i] address
				strcpy(keep_ap_path,ab_path_tok);//copy the absolute path to keep_ap_path
				ab_path_tok = strtok(ab_path_tok,"/");//tokenized ab_path_tok by /
				while(ab_path_tok !=NULL){//loop for find pattern
					pattern = inspecting_wild(ab_path_tok);
					if(pattern!=NULL) break;
					ab_path_tok = strtok(NULL,"/");
				}//end of loop that while(ab_path_tok !=NULL)
				ab_path_tok = strtok(keep_ap_path,pattern);//keep the absolute path except wild cart optin
				drip = opendir(ab_path_tok);//open absolute path diretory 
				while(( dir = readdir(drip))!=NULL){//loop for store file info
					if( (fnmatch(pattern,dir->d_name,0)) ==0 ){ //save file which is matching pattern
                                        lstat(dir->d_name,&file_info);
                                        w_c_Head = sort_list(w_c_Head, dir,&file_info,Sflag);
					}
                                }//end of loop that while((dir = readdir(drip))!=NULL)
				closedir(drip);//close diretory
                		is_star_last =1;//is_star_last = 1 	
			}//end of case for absolute path
			else if(strcmp(w_card[i],"*") ==0) {//case for * only
				drip = opendir("."); // open current diretory
				while(( dir = readdir(drip))!=NULL){//loop for store file info
					lstat(dir->d_name,&file_info);
					w_c_Head = sort_list(w_c_Head, dir,&file_info,Sflag);
				}//end of loop that while((dir = readdir(drip))!=NULL)
				ab_path_tok = ".";//ab_path_tok keep current directory
				is_star_last=1;
				closedir(drip);
			}//end of case for *	
			else{//case for ? or sequence
				drip = opendir(".");//open current directory
                                while((dir = readdir(drip))!=NULL){//loop for store file
                                lstat(dir->d_name,&file_info);
                                w_c_Head = sort_list(w_c_Head,dir,&file_info,Sflag);
                                }
				Cur = w_c_Head;
				while(Cur!=NULL){
					if((fnmatch(w_card[i],Cur->name,0))!=0){//for exception which input is not matching wild card
						w_c_Head=except_node(w_c_Head,Cur);//exception node
						Cur = w_c_Head;
					}
					else {
					Cur = Cur ->next;
					}
				}
				closedir(drip);
			 }//end of case for ? 
	if(is_star_last!=1){//if there is no absolute path wild card input
	if(rflag ==1){//if rflag on
		w_c_Head = reverse_node_swap(w_c_Head);//reverse the node	
	}
	Cur = w_c_Head;
	dir = NULL;
	while(Cur !=NULL){//loop for counting block count
		if(aflag == 1){
		b_count = b_count + Cur ->buf.st_blocks;
		Cur = Cur ->next;
		}
		else if(aflag ==0){
			if(strcmp(Cur -> name, ".") ==0 || strcmp(Cur ->name, "..") ==0){
				Cur = Cur ->next;
			}
			else{
			b_count = b_count + Cur ->buf.st_blocks;
              		Cur = Cur ->next;
			}
		}
	}
	Cur = w_c_Head;
	if(aflag ==0 && lflag ==1){//if aflag=0 lflag =1
		printf("total: %ld\n",b_count/2);//printf total information
		while(Cur!=NULL){//print each information
			if(strcmp(Cur -> name, ".") ==0 || strcmp(Cur ->name, "..") ==0){Cur = Cur->next; continue;}
			print_type(Cur->buf);
			print_permit(Cur->buf);
			printf("\t%u\t",(unsigned int)Cur->buf.st_nlink);
			print_userID(Cur->buf);
			print_groupID(Cur->buf);
		//	printf("%u\t",(unsigned int)Cur->buf.st_size);
			print_size(Cur ->buf,hflag);
			print_time(Cur->buf);		
			printf("%s\n",Cur->name);
			Cur = Cur -> next;
			}
	}
	else if(aflag ==1 && lflag ==1){//if aflag =1 lflag =1
		printf("total: %ld\n",b_count/2);//printf total information
		while(Cur!=NULL){//print each information
			print_type(Cur->buf);
			print_permit(Cur->buf);
			printf("\t%u\t",(unsigned int)Cur->buf.st_nlink);
			print_userID(Cur->buf);
			print_groupID(Cur->buf);
		//	printf("%u\t",(unsigned int)Cur->buf.st_size);
			print_size(Cur->buf,hflag);
			print_time(Cur->buf);
			printf("%s\n",Cur->name);
			Cur = Cur->next;
		}
	}
	else{//aflag = 0 , lflag =0
		while(Cur!=NULL){
		if((fnmatch(".*",Cur->name,0))==0 ){Cur = Cur ->next; continue;}//hidden file exception
		printf("%s\n",Cur->name);
		Cur = Cur->next;
		}
	}
	}//is_star_last !=1
	else if(is_star_last ==1){is_star_last =0;//if wild card is in absolute path
		if(rflag ==1){//rflag on
            	    w_c_Head = reverse_node_swap(w_c_Head);//reverse the node
        	}
		 Cur = w_c_Head;
      	         dir = NULL;
		chdir(ab_path_tok);
		while(Cur!=NULL){//first of all file print
			drip = opendir(Cur->name);//open func for inspecting file or directory
			if(drip==NULL){	//if file
				if(aflag ==0 && lflag ==1){
					if(strcmp(Cur -> name, ".") ==0 || strcmp(Cur ->name, "..") ==0){Cur = Cur->next; continue;}
                       				 print_type(Cur->buf);
                 			       print_permit(Cur->buf);
                      				  printf("\t%u\t",(unsigned int)Cur->buf.st_nlink);
                        			print_userID(Cur->buf);
                        			print_groupID(Cur->buf);
               					 //      printf("%u\t",(unsigned int)Cur->buf.st_size);
                       				 print_size(Cur ->buf,hflag);
                        			print_time(Cur->buf);
                        			printf("%s%s\n",ab_path_tok,Cur->name);
                        
				}
				else if(aflag ==1 && lflag ==1){
						print_type(Cur->buf);
                			        print_permit(Cur->buf);
               				         printf("\t%u\t",(unsigned int)Cur->buf.st_nlink);
                      				  print_userID(Cur->buf);
                   				     print_groupID(Cur->buf);
						 //      printf("%u\t",(unsigned int)Cur->buf.st_size);
                      				  print_size(Cur->buf,hflag);
                     				   print_time(Cur->buf);
                       				 printf("%s%s\n",ab_path_tok,Cur->name);	
				
				}
				else{
					 if((fnmatch(".*",Cur->name,0))==0 ){Cur = Cur ->next; continue;}//exception hidden file
            				    printf("%s%s\n",ab_path_tok,Cur->name);
				}
			}
		closedir(drip);
		Cur = Cur->next;
		}
		printf("\n\n\n");
		Cur = w_c_Head;
		dir = NULL;
		while(Cur!=NULL){ //Loop for directory print 
			if(fnmatch(".*",Cur->name,0)==0){Cur = Cur ->next; continue;}
			if((strcmp(Cur->name,".")==0)||(strcmp(Cur->name,"..")==0)){Cur = Cur ->next; continue;}
			drip = opendir(Cur->name);//inspecting that file is directory or real file
			if(drip!=NULL){//if directory
				while((dir = readdir(drip))!=NULL){//store that directory inner file information
					lstat(dir->d_name,&file_info);
					Head_ab_print = sort_list(Head_ab_print,dir,&file_info,Sflag);	//Keeping
				}
				abCur = Head_ab_print;
				while(abCur!=NULL){//if that directory has fille
					if(aflag == 1){
				                b_count = b_count + abCur ->buf.st_blocks;
						abCur = abCur ->next;
                			}
              			       else if(aflag ==0){
                     				   if(fnmatch(".*",abCur->name,0)==0){
                        			   	abCur = abCur ->next;
						   }
                        			else{
                        				b_count = b_count + abCur ->buf.st_blocks;
                        				abCur = abCur ->next;
                        			 }
                			}
				}//while(abCur!=NULL)
				printf("Directory path: %s%s\n",ab_path_tok,Cur->name);//print directory path
				abCur = Head_ab_print;
			 	if(aflag ==0 && lflag ==1){
              				  printf("total: %ld\n",b_count/2);//printf total information
               				 while(abCur!=NULL){//print each information
                        			if(strcmp(Cur -> name, ".") ==0 || strcmp(Cur ->name, "..") ==0){abCur = abCur->next; continue;}
                        			print_type(abCur->buf);
                       				 print_permit(abCur->buf);
                        			printf("\t%u\t",(unsigned int)abCur->buf.st_nlink);
                        			print_userID(abCur->buf);
                        			print_groupID(abCur->buf);
                				//      printf("%u\t",(unsigned int)Cur->buf.st_size);
                        			print_size(abCur ->buf,hflag);
                        			print_time(abCur->buf);
                        			printf("%s\n",abCur->name);
                        			abCur = abCur -> next;
                        		}
        		       }
			       else if(aflag ==1 && lflag ==1){
               				 printf("total: %ld\n",b_count/2);//printf total information

              				  while(abCur!=NULL){//print each information
                      				  print_type(abCur->buf);
                   				    print_permit(abCur->buf);
                 				     printf("\t%u\t",(unsigned int)abCur->buf.st_nlink);
                    				    print_userID(abCur->buf);
        	     		                print_groupID(abCur->buf);
						 //      printf("%u\t",(unsigned int)Cur->buf.st_size);
                      				  print_size(abCur->buf,hflag);
                       				 print_time(abCur->buf);
                        			printf("%s\n",abCur->name);
                      				  abCur = abCur->next;
                				}
        				}
        		      else{
           			     while(abCur!=NULL){
                				if(strcmp(abCur->name,".")==0 || strcmp(abCur->name,"..")==0 ){abCur = abCur ->next; continue;}
               					 printf("%s\n",abCur->name);
                				abCur = abCur->next;
                			}
				}
			     printf("\n");
			     free_node(Head_ab_print);//free node
			     Head_ab_print = NULL;
			     closedir(drip);
			     dir = NULL;
			}//if(drip!=NULL)
			Cur = Cur ->next;
		}
	}//if_star_last ==1
	chdir(fwd);
	}//for(;;)
	}//if(w_card_count)
//////////////////////////////////////////////////////////////End of case for wild card ////////////////////////////////////////////////////////////////////////////////////
	if(Head!=NULL){//case for free
	free_node(Head);
	}
	if(w_c_Head!=NULL){
	 free_node(w_c_Head);
	}
	free(wd);//free wd
	free(fi_info);//free fi_info
			


	return 0; }


