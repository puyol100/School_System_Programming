# include <stdio.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fnmatch.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/wait.h>
# include <signal.h>
# include "web_serv.h"
# define URL_LEN 255//define for URL length
# define BUFSIZE 1024//define BUFSIZE
# define PORTNO  40320//define my portno
# define FNM_CASEFOLD	(1 << 4)//for FNM_CASEFOLD
static int maxNchildren;//The maximun # of child process
static pid_t *pids;//keep child's pid
static char* buf;//for buffer
char home[255];//for all the prcess's home directory
/////////////////////////////////////////////////////////
// count_file_size                                     //
// ====================================================//
// file : To inspect file			                   //
// output: file size;                                  //
// purpose: for counting file size   			       //                       
/////////////////////////////////////////////////////////
int count_file_size(FILE *file){
	int size;
	fseek(file,0,SEEK_END);
	size = ftell(file);
	fseek(file,0,SEEK_SET);
	return size;
}
/////////////////////////////////////////////////////////
// make_history_node                                   //
// ====================================================//
// purpose: for making node           			       //                       
/////////////////////////////////////////////////////////
typedef struct make_history_node//make struct 
{
	int cl_no;//for keeping client no
	char cl_ip_adrs[12];//for keeping client address
	pid_t cl_p_id;//for keeping client PID
	int cl_prt;//for keeping client port no
	char cl_time[60];//for keeping client access time
	struct make_history_node* Prev;
	struct make_history_node* Next;
	
}History;
static History* pHead[5];//define global variable pHead

/////////////////////////////////////////////////////////
//  get_time							               //
// ====================================================//
// purpose: for makeing time for print			       //                       
/////////////////////////////////////////////////////////
void get_time(char* cl_time)
{
	char tm[60];
	char tmp[10];
	int year;
	time_t current_time;
	struct tm *struct_time;
	time(&current_time);//get current time
	struct_time = localtime(&current_time);
	switch(struct_time->tm_wday){//for week day
               case 0: strcpy(tm,"Sun ");
                      break;
               case 1: strcpy(tm,"Mon ");
                      break;
               case 2: strcpy(tm,"Thue ");
                      break;
               case 3: strcpy(tm,"Wen ");
                      break;
               case 4: strcpy(tm,"Thur ");
                      break;
               case 5: strcpy(tm,"Fri ");
                      break;
               case 6: strcpy(tm,"Sat ");
                      break;
        }
	switch(struct_time->tm_mon){//for make month 
	       case 0: strcat(tm,"Jan ");
                      break;
               case 1: strcat(tm,"Feb ");
                      break;
               case 2: strcat(tm,"Mar ");
                      break;
               case 3: strcat(tm,"Apr ");
                      break;
               case 4: strcat(tm,"May ");
                      break;
               case 5: strcat(tm,"Jun ");
                      break;
               case 6: strcat(tm,"Jul ");
                      break;
               case 7: strcat(tm,"Aug ");
                      break;
               case 8: strcat(tm,"Sep ");
                      break;
               case 9: strcat(tm,"Oct ");
                      break;
               case 10: strcat(tm,"Nov ");
                      break;
               case 11: strcat(tm,"Dec ");
                      break;
	}
	year = struct_time->tm_year + 1900;//make year +1900 because of epoch time
	sprintf(tmp, "%d",struct_time->tm_mday);//tmp get month info
	strcat(tm,tmp);
	strcat(tm," ");//tm get month info
	sprintf(tmp,"%d",struct_time->tm_hour);//tmp get hour info
	strcat(tm,tmp);
	strcat(tm,":");//tm get hour info
	sprintf(tmp,"%d",struct_time->tm_min);//tmp get minute info
	strcat(tm,tmp);
	strcat(tm,":");//tm get minute info
	sprintf(tmp,"%d",struct_time->tm_sec);//tmp get sec info
	strcat(tm,tmp);
	strcat(tm," ");//tm get sec info
	sprintf(tmp,"%d",year);//tmp get year
	strcat(tm,tmp);//final tm has all time info formatting history print format
	strcpy(cl_time,tm);//node get time
}
/////////////////////////////////////////////////////////
// make_list                                           //
// ====================================================//
// HEAD : list Head      			                   //
// struct in_addr i_c_a : has client info			   //
// struct sockaddr_in c_a : has client info            //
// purpose: making linked list about client info	   //
/////////////////////////////////////////////////////////
History* make_list(History* HEAD,struct in_addr i_c_a,struct sockaddr_in c_a,pid_t c)
{
	History* Head;//for keep head address
	History* pNew;//for new node
	History* pCur;//for cur
	int no =1;
	pNew =(History*) malloc(sizeof(History));//make dynamic allocation for new node
	strcpy(pNew->cl_ip_adrs,inet_ntoa(i_c_a));//give new client ip to node
	pNew->cl_prt = c_a.sin_port;//give new client port no
	get_time(pNew->cl_time);//give new client access time
	pNew->cl_p_id = c;
	Head = HEAD;
	if(Head == NULL)//if Head is null
	{
		pNew->cl_no = 1;
		Head = pNew;
		Head->Next = NULL;
		Head->Prev = NULL;
		return Head;//return Head
	}
	else//head is not null
	{
		pNew->Next = Head;
		pNew->Prev = NULL;
		Head->Prev = pNew;
		pCur = pNew;
		while(pCur!=NULL)//cur for make list
		{
			pCur->cl_no = no;
			no = no+1;
			if(pCur->cl_no ==11)//case for over 10 list
			{
				pCur->Prev->Next = NULL;//for free
				free(pCur);//free that node memory
				break;
			}
			pCur = pCur->Next;
		}
		return pNew;	//return New node
	}
}
/////////////////////////////////////////////////////////
// print_history                                       //
// ====================================================//
// Head : has node head address		                   //
// purpose: for printing history     			       //                       
/////////////////////////////////////////////////////////
void print_history(History* Head)
{
	History* pCur = Head;//pCur has Head address
	pCur = Head;
	while(pCur !=NULL)
	{
	printf("%d\t%s\t%d\t%d\t%s\n",pCur->cl_no,pCur->cl_ip_adrs,pCur->cl_p_id,pCur->cl_prt,pCur->cl_time);
	pCur = pCur ->Next;
	}
}
/////////////////////////////////////////////////////////
// signalHandler                                       //
// ====================================================//
// sig : To get signal  			                   //
// purpose: for Handle SIGINT and SIGALRM			   //                       
/////////////////////////////////////////////////////////
void signalHandler(int sig)
{
	char k_time[60];//for keeping terminate process time
	if(sig == SIGINT)//if signal is siginit
	{
		printf("\n");	
		for(int i=0; i<5;i++)
		{
			get_time(k_time);
			printf("[%s] %d process is terminated.\n",k_time,pids[i]);//print terminate process ID and terminated time
			kill(pids[i],SIGTERM);//kill child process
		}
		while(wait(NULL) >0)//for complete terminated  all child process 
			;
		get_time(k_time);
		printf("[%s] server is terminated.\n",k_time);
		exit(0);
	}
	else if(sig == SIGALRM)//if sigal is SIGALRM
	{
		printf("======== Connection History ============================\n");//print history title
 		printf("%s\t%s\t\t%s\t%s\t%s\n","No.","IP","PID","PORT","TIME");//print history  title
		for(int i=0; i<5;i++)
		{
			kill(pids[i],SIGUSR1);//send signal to child process that has pid[i]
		}		
	}
}
/////////////////////////////////////////////////////////
// access_inspecting                                   //
// ====================================================//
// struct in_addr address : client address		       //
// purpose: for inspecting accessible ip		       //                       
/////////////////////////////////////////////////////////
int access_inspecting(struct in_addr address)
{
	FILE *file = fopen("accessible.usr","r");//open acceble.usr file
	char tmp[50];//tmp for keep accessible file info
	if(file !=NULL)
	{	
		while(!feof(file))
		{	
			fgets(tmp,50,file);
			for(int i=0;;i++)
			{
				if((tmp[i]>47&&tmp[i]<58)||tmp[i]==46)//case for buff error
				{
					continue;
				}
				else
				{
					tmp[i] ='\0';//if correct info end 
					break;
				}
			}	
			if(strcmp(tmp,inet_ntoa(address))==0)//inspecting accessible client
			{	
				fclose(file);	
				return 1;//return 1 if accessible client
			}
		}
	fclose(file);
	}
	else
	{
		printf("accessible.usr file has no data\n");//if accessible.usr file has no data
	}
	return 0;//return 0 if not accessible client
}
/////////////////////////////////////////////////////////
//  print_access_deny                                  //
// ====================================================//
// purpose: for printing access deny info		       //                       
/////////////////////////////////////////////////////////
void print_access_deny(struct in_addr inet_address,int c_fd)
{
	char  response_header[BUFSIZE]={0,};//for keep response header message
	char* response_message;//for response message
	int t_BUFSIZE=0;
	FILE *file = fopen("html_ls.html","w");//open html for write
	fprintf(file,"<html>\n");
	fprintf(file,"<title>Access Denied </title>\n");
	fprintf(file,"<h1>");
	fprintf(file,"Access denied!");
	fprintf(file,"<br>\n");
	fprintf(file,"Your IP : %s\n",inet_ntoa(inet_address));
	fprintf(file,"</h1>\n");
	fprintf(file,"<br>");
	fprintf(file,"<body>\n");
	fprintf(file,"You have no permission to access this web server.\n");
	fprintf(file,"<br>");
	fprintf(file,"HTTP 403.6 - Forbidden: IP address reject\n");
	fprintf(file,"</body>");
	fprintf(file,"<br>\n");
	fprintf(file,"</html>");
	fclose(file);
	//write to html
	file = fopen("html_ls.html","r");//open html for read
	t_BUFSIZE = count_file_size(file);	//get file size
	response_message = malloc(t_BUFSIZE+1);//dynamic allocation 
  	for(int i=0; i<t_BUFSIZE; i++)
    	{
    	 response_message[i] = fgetc(file);//make response message
    	}
	sprintf(response_header,
                         "HTTP/1.0.200 OK\r\n"
                         "Server:2019 simple web server\r\n"
                         "Content-length:%d\r\n"
                         "Content-type: %s\r\n\r\n",t_BUFSIZE,"text/html");
    write(c_fd, response_header, strlen(response_header));//send to web
    write(c_fd, response_message,t_BUFSIZE);//send to web
    fclose(file);
    free(response_message);
}
/////////////////////////////////////////////////////////
// signalHandler_child                                 //
// ====================================================//
// sig : To get signal that comes from child  	       //
// purpose: for print histoy of each process by SIGUSR1//  
/////////////////////////////////////////////////////////
void signalHandler_child(int sig)
{
	int count =0;
      	 if(sig == SIGUSR1)
        {
		for(int i=0; i<5; i++)//loop for print all child_process history that has history
		{
			if(pHead[i] ==NULL)//if that chil_process history node is empty skip that
			{
				count  = count+1;
			 	continue;//skip
			}
			else
			{
				print_history(pHead[i]);//print histroy 
			}
		}
        }
}
/////////////////////////////////////////////////////////
// child_main	                                       //
// ====================================================//
// socket_id:socket_id that given by parent  	       //
// num_index: that process id hisory head number	   //
// purpose: function of action for child process 	   //                       
/////////////////////////////////////////////////////////
void child_main(int socket_fd,int num_index)
{
	struct sockaddr_in client_addr;//for client
        int client_fd, len_out;
        char buf[BUFSIZE];
	int len;
	char f_time[60];
	char cwd[255];
	char precwd[255];
        socklen_t clien;
	pid_t proces_id;//for keep forked process id
	strcpy(cwd,home);
	proces_id = getpid();//for print process id that forked by parent
	get_time(f_time);//for get forked time
        printf("[%s] %ld process is forked.\n",f_time,(long)getpid());//print that forked sentence
	signal(SIGUSR1,signalHandler_child); //if SIGUSR1 signal comes to child process signal handler
     while(1){
		struct in_addr inet_client_address;//for inetoa client address
		FILE* file;//for file open
		char  buf[BUFSIZE]={0,};
		char  tmp[BUFSIZE]={0,};
		char  response_header[BUFSIZE]={0,};
		char* response_message;
		char url[URL_LEN] = {0,};
		char method[20] = {0,};
		char* tok = NULL;
		int t_BUFSIZE=0;
		char decision[URL_LEN] = {0,};
		char co_time[60], dis_time[60];//for keep connection time and disconnected time
		DIR *drip1;
		pid_t pid;//for get client pid
		int status;//for keep child process status
		len = sizeof(client_addr);
		client_fd = accept(socket_fd, (struct sockaddr*)&client_addr,&len);
		if( client_fd <0)//if that client_fd is not accepted
		{
			printf("Server : accept failed\n");
			close(client_fd);
			continue;
		}
		
		inet_client_address.s_addr = client_addr.sin_addr.s_addr;
//////////////////////////////////////////////////////////////
		if(read(client_fd,buf,BUFSIZE)<=0)
		{
			close(client_fd);
			continue;
		}
		if(access_inspecting(inet_client_address)==0)//inspecting accessible client
        	{
        	     print_access_deny(inet_client_address,client_fd);//if client is not accessible then print deny
		     close(client_fd);
         	    continue;//keep server on
       	 	}
		strcpy(tmp,buf);
		tok = strtok(tmp," ");
        	strcpy(method,tok);
        	if(strcmp(method,"GET") ==0)//get url
        	{
        	  tok = strtok(NULL," ");
        	  strcpy(url,tok);//url has absolute path
        	}
		if(strcmp(url,"/favicon.ico")==0)//if url is favicon pass that client
		{
			close(client_fd);
			continue;//keep server on
		}
		pHead[num_index] = make_list(pHead[num_index],inet_client_address,client_addr,proces_id);//make new client info node
		get_time(co_time);//get time for print connected time
		printf("==========New Client=================\n");
		printf("[%s]\n",co_time);//print connected time
       		printf("IP : %s\n",inet_ntoa(inet_client_address));
        	printf("Port : %d\n",client_addr.sin_port);
        	printf("=====================================\n\n");
////////////////////////print New client info/////////////////////////////////////////////////
		strcpy(cwd,home);
		if(strcmp(url,"/")==0)//if url is home directory
		{
			   strcpy(url,home);
		}
		else//url is not home directory make absolute path
		{
			   strcat(cwd,url);
	           strcpy(url,cwd);  
		}
/////////////////////////////////for decide file media type//////////////////////////////////////////////
		if(((drip1=opendir(url)) == NULL) || (fnmatch("*.html",url,0) ==0))//if that is not directory
		{	
		   if((fnmatch("*.html",url,0)==0))//if html file
		   {
		   	strcpy(decision,"text/html");//make mediatype
			file = fopen(url,"r");
		   }
		   else if((fnmatch("*.jpg",url,FNM_CASEFOLD)==0)||
                    	  (fnmatch("*.png",url, FNM_CASEFOLD)==0)||
                    	  (fnmatch("*.jpeg",url, FNM_CASEFOLD)==0))//if picture file
           {
            strcpy(decision,"image/*");//make media type
			file = fopen(url,"rb");
		    }
		   else//else file
		   {
            file = fopen(url,"r");
			if(file == NULL)
			{
			   web_serv_info_print(url,home);
			   file = fopen("html_ls.html","r");
			   strcpy(decision,"text/html");//if url is not correct file 
			}
			else
			{
				strcpy(decision,"text/plain");
			}   
		   }
		}
		else//if that is directory file
		{
			  if(drip1!=NULL)
			  {
			     closedir(drip1);	
			  }
			strcpy(decision,"text/html");//decide media type
			web_serv_info_print(url,home);//make html 
			strcpy(url,"html_ls.html");
	        file = fopen(url,"r");
       		 }
//////////////////////////////////////////////////////////////////////////////////////////////////
		if(file == NULL)
 	       {
 	         printf("file open error\n");
 	         continue;
 	       }
 	       t_BUFSIZE = count_file_size(file);//count file size
 	       response_message = malloc(t_BUFSIZE+1);//dynamic allocation
		for(int i=0; i<t_BUFSIZE; i++)
		{
			response_message[i] = fgetc(file);//get file info

		}
		sprintf(response_header,
			 "HTTP/1.0.200 OK\r\n"
			 "Server:2019 simple web server\r\n"
			 "Content-length:%d\r\n"
			 "Content-type: %s\r\n\r\n",t_BUFSIZE,decision);//make response header
		write(client_fd, response_header, strlen(response_header));//give info to web
		write(client_fd, response_message,t_BUFSIZE);//give info to web
		
		get_time(dis_time);//for make disconnected time
		printf("======= Disconnected Client ===\n");//print diconnected client info
		printf("[%s]\n",dis_time);//print disconnected time
		printf("IP : %s\n",inet_ntoa(inet_client_address));
	        printf("Port : %d\n",client_addr.sin_port);
	        printf("=====================================\n\n");	
		close(client_fd);
		fclose(file);
		free(response_message);
	}

}
/////////////////////////////////////////////////////////
// child_make                                          //
// ====================================================//
// socketfd: for get socket id                         //
// num_index: for keep history node number_index	   //
// purpose:make child porcess by forked		 	       //                       
/////////////////////////////////////////////////////////
pid_t child_make(int socketfd,int num_index)
{
        pid_t pid;
	int status;
        if((pid=fork()) >0)//forking
	{
	        return(pid);//get child process id
	}
	else
	{		
        	child_main(socketfd,num_index);//give sokcet_fd and index to child process
	}
}
int main(){
	struct sockaddr_in server_addr;//, client_addr;
	int socket_fd, client_fd;//for client and sever socket
	int len, len_out;
	int opt =1;
	char p_sever_time[60];//for keep  parent server open time
	History* pCur = NULL;
	if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server : Can't open stream socket \n");
		return 0;
	}
	getcwd(home,255);//get parent home info
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt));
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNO);
	if(bind(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) <0){
		printf("Server : Can't bind local address \n");
		return 0;
	}
	get_time(p_sever_time);//for get parent server open time
	printf("\n[%s] Server is started.\n",p_sever_time);
	listen(socket_fd,5);//for sockt queue
	
	maxNchildren = 5;
	pids = (pid_t *)malloc(maxNchildren * sizeof(pid_t));//make pids memory by malloc for keep preforked process pid
	for(int i=0; i<maxNchildren;i++)
	{
		pids[i] = child_make(socket_fd,i);//make child process and get pid info to pids[i]
	}
	signal(SIGINT,signalHandler);//for SIGINT signal handling
	for(;;)
	{
		alarm(10);//for alarming 10 seocnds
        	signal(SIGALRM,signalHandler);//for SIGALRM signal Handling
		pause();
	}
	return 0;
}

