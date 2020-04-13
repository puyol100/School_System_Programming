# include "web_serv.h"

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
void print_type(struct stat  a, FILE* file){
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
	
	//printf("%c\t",c);
	if(S_ISLNK(a.st_mode)) fprintf(file,"<td style = \"color:green\"> %c\t",c);
	else if(S_ISDIR(a.st_mode)) fprintf(file,"<td style = \"color:blue\"> %c\t",c);
	else{
	fprintf(file,"<td style= \"color:red\">%c\t",c);
	}
}
/////////////////////////////////////////////////////////
// print_permit                                        //
// ====================================================//
// stat a --> argument that has file information       //
// output: printf(print permit)                        //
// purpose: print file permit                          //                       
/////////////////////////////////////////////////////////
void print_permit(struct stat a, FILE* file){
	mode_t tmp;
	tmp = a.st_mode;
	for(int i=0; i<3; i++){
		if(tmp & S_IRUSR)
			fprintf(file,"r");
		else 
			fprintf(file,"-");

		if(tmp & S_IWUSR)
			fprintf(file,"w");
		else 
			fprintf(file,"-");
		
		if(tmp & S_IXUSR)
			fprintf(file,"x");
		else
			fprintf(file,"-");

		tmp <<=3;
	}
	fprintf(file,"</td>");
}	
/////////////////////////////////////////////////////////
// print_ID(user.group)                                //
// ====================================================//
// stat a --> argument that has file information       //
// output: printf(print ID) 	                       //
// purpose: print file user or group ID                //                       
/////////////////////////////////////////////////////////
void print_userID(struct stat a,FILE* file){
	struct passwd *pwd;
	pwd=getpwuid(a.st_uid);//get ID information
	if(S_ISLNK(a.st_mode)) fprintf(file,"<td style = \"color:green\">");
        else if(S_ISDIR(a.st_mode)) fprintf(file,"<td style = \"color:blue\">");
        else{
        fprintf(file,"<td style= \"color:red\">");
        }
	
	if((pwd=getpwuid(a.st_uid))!=NULL){
	fprintf(file,"%s\t",pwd->pw_name);
	}
	else 
	printf("Wrong input\n");

	fprintf(file,"</td>");
}
void print_groupID(struct stat a,FILE* file){
	struct group *grp;
	if(S_ISLNK(a.st_mode)) fprintf(file,"<td style = \"color:green\">");
        else if(S_ISDIR(a.st_mode)) fprintf(file,"<td style = \"color:blue\">");
        else{
        fprintf(file,"<td style= \"color:red\">");
        }

	if((grp =getgrgid(a.st_uid))!=NULL){//get ID Information
	fprintf(file,"%s\t",grp->gr_name);
	}
	else
	printf("Wrong input\n");

	fprintf(file,"</td>");
}
/////////////////////////////////////////////////////////
// print_time 	                                       //
// ====================================================//
// stat a --> argument that has file information       //
// output: printf(print time)                          //
// purpose: print file time information                //                       
/////////////////////////////////////////////////////////
void print_time(struct stat a,FILE* file){
	struct tm * ptm;
	ptm = localtime(&(a.st_mtime));//get time information
	if(S_ISLNK(a.st_mode)) fprintf(file,"<td style = \"color:green\">");
        else if(S_ISDIR(a.st_mode)) fprintf(file,"<td style = \"color:blue\">");
        else{
        fprintf(file,"<td style= \"color:red\">");
        }
	switch(ptm->tm_mon){
	       case 0: fprintf(file,"Jan");
                      break;
               case 1: fprintf(file,"Feb");
                      break;
               case 2: fprintf(file,"Mar");
                      break;
               case 3: fprintf(file,"Apr");
                      break;
               case 4: fprintf(file,"May");
                      break;
               case 5: fprintf(file,"Jun");
                      break;
               case 6: fprintf(file,"Jul");
                      break;
               case 7: fprintf(file,"Aug");
                      break;
               case 8: fprintf(file,"Sep");
                      break;
               case 9: fprintf(file,"Oct");
                      break;
               case 10: fprintf(file,"Nov");
                      break;
               case 11: fprintf(file,"Dec");
                      break;
	}
	fprintf(file," %02d %02d %02d\t</td>",ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
}	
/////////////////////////////////////////////////////////
// print_size 	                                       //
// ====================================================//
// stat a -->argument that has file information        //
// int hflg-->argument that has hflag info	       //
// output: return;                                     //
// purpose: for printing file size                     //                       
/////////////////////////////////////////////////////////
void print_size(struct stat a,int hflg,FILE* file){
	if(S_ISLNK(a.st_mode)) fprintf(file,"<td style = \"color:green\">");
        else if(S_ISDIR(a.st_mode)) fprintf(file,"<td style = \"color:blue\">");
        else{
        fprintf(file,"<td style= \"color:red\">");
        }
	if(hflg==1){
		if( (unsigned int)(a.st_size)/(1024 * 1024 * 1024) !=0){
			fprintf(file,"%.1fG\t",(float)a.st_size/(1024*1024*1024));	
		}
		else if( (unsigned int)(a.st_size)/(1024 * 1024) !=0){
                        fprintf(file,"%.1fM\t",(float)a.st_size/(1024*1024));
                }
		else if( (unsigned int)(a.st_size)/(1024) !=0){
                        fprintf(file,"%.1fK\t",(float)a.st_size/(1024));
                }
		else {
			fprintf(file,"%u\t",(unsigned int)a.st_size);
		}

	}
	else{
		fprintf(file,"%u\t",(unsigned int)a.st_size);
	}
	
	fprintf(file,"</td>");
	return ;
}


/////////////////////////////////////////////////////////
// two_Array_free                                      //
// ====================================================//
// Node * remove_node --> node  for free               //
// output: return;                                     //
// purpose: for node free   			               //                       
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
/////////////////////////////////////////////////////////
// test_404                                            //
// ====================================================//
// path for inspecting							       //
// return 0: right return 1: incorrect path	           //
// purpose: insepecting path 						   //
/////////////////////////////////////////////////////////
int test_404(char* path)
{
    char tmp[255];
    DIR* tmp_drip;//for open directory
    struct dirent* tmp_dir;//tmp for keep file info
    int count = 0;
    char inspect[124];//for inspecting url
    tmp_drip = opendir(path);//open path directory
    if(tmp_drip !=NULL)//if path is right
    {
	closedir(tmp_drip);
	return 0;
    }
    strcpy(tmp,path);//tmp has path
    count = strlen(path);//counting path length
    for(int i=0;;i++)//loop for inspecting url
    {
	if(tmp[count-i] =='/')
	{
		tmp[count-i] = '\0';//get last / before url
		break;	
	}
	inspect[i] = tmp[count-i];
    } 
    count =0; 
    tmp_drip = opendir(tmp); //open last / before url
    if(tmp_drip ==NULL)//if that url incorrect
    {
	closedir(tmp_drip);
	return 1;//return 1
    }
    else//if that url can open 
    {
	while((tmp_dir = readdir(tmp_drip))!=NULL)
	{
		if(strcmp(tmp_dir->d_name,inspect) ==0)//inspecting is there file in directory
		{
			count = count +1;
			break;
		}
	}
    }
    closedir(tmp_drip);
    if(count ==0)//if there is no matching file
    {
	return 1;//return 1
    }
	return 0;//else return 0
}
int web_serv_info_print(char* path, char* home){
	FILE *file = fopen("html_ls.html","w");
	DIR *drip;
	int hflag=0;
	int lflag=0, aflag=0;
	struct dirent *dir;//for directory
	char cwd[255];//for print current directory
	Node *Head=NULL;//for node list Head addiress
	Node *Cur = NULL;//for iterate node list
	long int b_count=0;//for block counting
	char fwd[255];//first working directory
	struct stat file_info;//for getting file info
	int count=0;
	char tmp_url[255];
	char tmp_url2[255];
	char tmp_url3[255];
	char copy_path[255];
///////////////////////////////////////////
	
	if(test_404(path)==1)//inpecting path and if that path is not correct path
	{
		for(int i=0;;i++)
		{
			if(path[i] != home[i])
			{
				count = i;//for get path that after home url
				break;
			}
		}
		fprintf(file,"<html>\n");
        fprintf(file,"<title>%s</title>\n",path);
        fprintf(file,"<h1>");
        fprintf(file,"Not Found");
        fprintf(file,"</h1>");
        fprintf(file,"<br>");
		fprintf(file,"<body>\n");
		fprintf(file,"The request URL %s was not found on this server",&path[count]);
		fprintf(file,"<br>");
		fprintf(file,"HTTP 404 - Not Page Found");
		fprintf(file,"</body>");
		fprintf(file,"</html>");
		fclose(file);//print 404 error message to html file
		return 0;
	
	}
	else//that path is correct
	{
		strcpy(copy_path,path);
		getcwd(fwd,255);
		if(file ==0)
		{
			printf("error\n");
			return -1;
		}
		if(strcmp(path,fwd) ==0){ lflag =1;}//l flag for home
		else{aflag =1; lflag=1;}//a and l flag for not home 
	}
	if(lflag ==1 && aflag ==0)//for home print
	{
			
		fprintf(file,"<html>\n");
		fprintf(file,"<title>%s</title>\n",fwd);
		fprintf(file,"<h1>");
		fprintf(file,"Welcome to System Programming Http");
		fprintf(file,"</h1>");
		fprintf(file,"<br>");
		fprintf(file,"<br>");
		drip = opendir(path);
		while((dir = readdir(drip))!=NULL)//read directory and get file info
		{
			if(dir ->d_name[0] != '.')//if that file is not hidden file
			{
				if(strcmp(dir->d_name,"html_ls.html")==0)
				{
					continue;
				}
				lstat(dir->d_name,&file_info);//get file info
				Head = sort_list(Head,dir,&file_info,0);//make list
			}
		}
		Cur = Head;
		b_count =0;
		while(Cur!=NULL)
		{//counting 512mb blocks and make 1024 blocks
			b_count =b_count+ Cur->buf.st_blocks;	
			Cur = Cur ->next;
		}
		Cur = Head;
		fprintf(file,"<body>\n");
     	fprintf(file,"<table border=\"1\">\n");
		fprintf(file,"Directory path: %s\n",fwd);
		fprintf(file,"<br>");
		fprintf(file,"total: %ld\n",b_count/2);//printf total information
		fprintf(file,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");//make board
		while(Cur!=NULL)
		{//print each information
		   fprintf(file,"<tr>");
			if(S_ISLNK(Cur->buf.st_mode))//if symbolic link
			{
                   fprintf(file,"<td><a href =\"%s\">%s</a></td>",Cur->name,Cur->name);
			}
			else if(S_ISDIR(Cur->buf.st_mode))//if directory
			{
				fprintf(file,"<td><a href =\"%s\">%s</a></td>",Cur->name,Cur->name);
            }
            else //others
			{
				fprintf(file,"<td><a href =\"%s\">%s</a></td>",Cur->name,Cur->name);
			}
			print_type(Cur->buf,file);
			print_permit(Cur->buf,file);
			if(S_ISLNK(Cur->buf.st_mode))//if symbolic link make coloer
			{
               fprintf(file,"<td style = \"color: green\" >\t%u\t</td>",(unsigned int)Cur->buf.st_nlink);
            }
            else if(S_ISDIR(Cur->buf.st_mode))//if directory 
			{
              fprintf(file,"<td style = \"color: blue\" >\t%u\t</td>",(unsigned int)Cur->buf.st_nlink);
             }
            else//if other
			{
               fprintf(file,"<td style= \"color: red\" >\t%u\t</td>",(unsigned int)Cur->buf.st_nlink);
            }
			print_userID(Cur->buf,file);
			print_groupID(Cur->buf,file);
			print_size(Cur ->buf,hflag,file);
			print_time(Cur->buf,file);		
			Cur = Cur -> next;
			fprintf(file,"</tr>\n");
		}
		fprintf(file,"</table>\n");
		fprintf(file,"</body>\n");
		fprintf(file,"<br>");
		fprintf(file,"<br>");	
				
	}
	else if(lflag ==1 && aflag ==1)
	{//for in home directory
		drip = opendir(path);
		chdir(path);
		while((dir = readdir(drip))!=NULL)//read directory and get file info
        {
			if(strcmp(dir->d_name,"html_ls.html")==0)
            {
                 continue;
            }
			lstat(dir->d_name,&file_info);//get file info
            Head = sort_list(Head,dir,&file_info,0);//make list
        }
        Cur = Head;
        b_count =0;
        while(Cur!=NULL)
        {//counting 512mb blocks and make 1024 blocks
           b_count =b_count+ Cur->buf.st_blocks;
           Cur = Cur ->next;
        }
		fprintf(file,"<html>\n");
        fprintf(file,"<title> %s </title>\n",fwd);
        fprintf(file,"<h1>");
        fprintf(file,"System Programming Http ");
        fprintf(file,"</h1>");
        fprintf(file,"<br>");
        fprintf(file,"<br>");
                
		while(Cur!=NULL)
        {//counting 512mb blocks and make 1024 blocks
          b_count =b_count+ Cur->buf.st_blocks;
          Cur = Cur ->next;
        }
        Cur = Head;
        fprintf(file,"<body>\n");
        fprintf(file,"<table border=\"1\">\n");
        fprintf(file,"Directory path: %s\n",path);
        fprintf(file,"<br>");
        fprintf(file,"total: %ld\n",b_count/2);//printf total information
        fprintf(file,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");

////////////////////////////////////////////part for get relative url hyper//////////////////////////////////////////////////////////////////////////		
		count = strlen(home);
		for(int i=0;;i++)
		{
		  if(path[count+i] =='\0')
		  {
			tmp_url[i] ='\0';
			break;
		  }
		  tmp_url[i] = path[count+i];
		}
		count = strlen(path);
		for(int i=0;;i++)
		{
			if(path[count-i-1] =='/')
			{	
				count = count-i-1;
				break;
			}
		}
		for(int i=0;;i++)
		{
			if(i == count)
			{
			  tmp_url2[i] = '\0';	
		          break;
			}
			tmp_url2[i] = path[i];
		}
		count =0;
		for(int i=0;home[i]!='\0';i++)
		{
			count++;
		}
		for(int i=0;;i++)
		{
			if(tmp_url2[i+count] =='\0'){
				tmp_url3[i] ='\0';
				break;
			}
			tmp_url3[i] = tmp_url2[i+count];
		}
///////////////////////////////////////////////End of part/////////////////////////////////////////////////////////////////
		while(Cur!=NULL)
        {//print each information
             fprintf(file,"<tr>");
             if(S_ISLNK(Cur->buf.st_mode))//if symbolic link
             {
				fprintf(file,"<td><a href =\"%s/%s\">%s</a></td>",tmp_url,Cur->name,Cur->name);
             }
             else if(S_ISDIR(Cur->buf.st_mode))//if directory
             {
				if((strcmp(Cur->name,".") ==0))//if url is .
				{
					fprintf(file,"<td><a href =\"%s\">%s</a></td>",tmp_url,Cur->name);//url is current directory
				}
				else if((strcmp(Cur ->name,"..") ==0))//if url is ..
				{
					if(tmp_url3[0] =='\0') 
					{
					 fprintf(file,"<td><a href =\"%s\">%s</a></td>",Cur->name,Cur->name);//if current directory is home
					}
					else
					{
					fprintf(file,"<td><a href =\"%s\">%s</a></td>",tmp_url3,Cur->name);//if current directory is not home 
					}
				}
				else
				{
					fprintf(file,"<td><a href =\"%s/%s\">%s</a></td>",tmp_url,Cur->name,Cur->name);
                }
			}
            else
            {	
             fprintf(file,"<td><a href =\"%s/%s\">%s</a></td>",tmp_url,Cur->name,Cur->name);
			}
            print_type(Cur->buf,file);
            print_permit(Cur->buf,file);
            if(S_ISLNK(Cur->buf.st_mode))
			{
              fprintf(file,"<td style = \"color: green\" >\t%u\t</td>",(unsigned int)Cur->buf.st_nlink);
            }
            else if(S_ISDIR(Cur->buf.st_mode))
			{
              fprintf(file,"<td style = \"color: blue\" >\t%u\t</td>",(unsigned int)Cur->buf.st_nlink);
            }
            else
			{
                fprintf(file,"<td style= \"color: red\" >\t%u\t</td>",(unsigned int)Cur->buf.st_nlink);
            }
            print_userID(Cur->buf,file);
            print_groupID(Cur->buf,file);
            print_size(Cur ->buf,hflag,file);
            print_time(Cur->buf,file);
            Cur = Cur -> next;
            fprintf(file,"</tr>\n");
        }
        fprintf(file,"</table>\n");
        fprintf(file,"</body>\n");
        fprintf(file,"<br>");
        fprintf(file,"<br>");
	}
	fclose(file);
	chdir(fwd);
	closedir(drip);	
	free_node(Head);	
   return 0;
}





