///////////////////////////////////////////////////////////
// File Name       : Log file and Semaphore              //  
// Date            : 2019/06/14                          //                                           
// Os              : Ubuntu 16.04 LTS 64bits             //                                            
// Author          : Han Sung Min                        //                                           
// Student ID      : 2015722088                          //
// ------------------------------------------------------//    
// Title : System Programing Assignment #4-3	         //
// Decription: ...                                       //
///////////////////////////////////////////////////////////
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
# include <sys/ipc.h>
# include <sys/shm.h>
# include <pthread.h>
# include <time.h> 
# include <semaphore.h>
# include <fcntl.h>
# include "web_serv.h"
# define URL_LEN 255//define for URL length
# define BUFSIZE 1024//define BUFSIZE
# define PORTNO  40320//define my portno
# define FNM_CASEFOLD	(1 << 4)//for FNM_CASEFOLD
# define KEY_NUM 40320//define KEY_NUM
# define MEM_SIZE 70000 // define shared Memory size

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

pid_t child_make(int socketfd,int num_index);
void insert_history(struct sockaddr_in* New, pid_t id);//for insert histroy to shared memory
void* print_to_log(void* s);//for print to log.txt

static sem_t* mysem;//for keep semephore
static pid_t *pids;//keep child's pid
static pid_t pid_p;//Keep parent's pid
static char* buf;//for buffer
char home[255];//for all the prcess's home directory
static int MaxChilds;//for Save MaxChild
static int MaxIdleNum;//for Save MaxIdleNum
static int MinIdleNum;//for Save MinIdleNum
static int StartProcess;//for Save StartProcess
static int MaxHistory;//for Save MaxHistory
static int IdleProcessCount;//for Save IdleProcess count
static int shm_id;//for keep shared memory id
static int socket_fd;//for save server socket_fd
static int total_forked;//for save total forked count
/////////////////////////////////////////////////////////
// count_file_size                                     //
// ====================================================//
// file : To inspect file			       //
// output: file size;                                  //
// purpose: for counting file size   		       //                       
/////////////////////////////////////////////////////////
int count_file_size(FILE *file){
	int size;
	fseek(file,0,SEEK_END);
	size = ftell(file);
	fseek(file,0,SEEK_SET);
	return size;
}
/////////////////////////////////////////////////////////
//  get_time					       //
// ====================================================//
// purpose: for makeing time for print		       //                       
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
// signalHandler                                       //
// ====================================================//
// sig : To get signal  	                       //
// purpose: to Handle SIGINT and SIGALRM	       // 
// SIGUSR1 , SIGUSR2     			       //                 
/////////////////////////////////////////////////////////
void signalHandler(int sig)
{
	char k_time[60];//for keeping terminate process time
	char log_info[1024];//for log print character size
	int log_size =0;//for log print size
	pthread_t tid;//for call thread 
	if(sig == SIGINT)//if signal is siginit
	{
		printf("\n");
                log_size =0;
                sprintf(log_info,"^c\n");//get print info
                pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread and call print info
                pthread_join(tid,NULL);//thread exit
		memset(log_info,0,1024*sizeof(char));//initialize memory
		int temp = 0;
		for(int i=0;i<MaxChilds;i++)
		{
			if(pids[i] ==0)
			{
				continue;
			}
			temp = temp +1;
		}
		temp = temp -1;
		for(int i=0;i<MaxChilds;i++)//total_forked
		{	
			if(pids[i] ==0)
			{
				 continue;
			}
			get_time(k_time);
			printf("[%s] %d process is terminated.\n",k_time,pids[i]);//print terminate process ID and terminated time
			sprintf(log_info,"[%s] %d process is terminated.\n",k_time,pids[i]);//get print info
			log_size = strlen(log_info);//check that size
			printf("[%s] IdleProcessCount : %d\n",k_time,temp);
			sprintf(&log_info[log_size],"[%s] IdleProcessCount : %d\n",k_time,temp);//get print info
			pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread and call print info 
        		pthread_join(tid,NULL);	//thread exit
			temp = temp -1;
			kill(pids[i],SIGTERM);//kill child process
			memset(log_info,0,1024*sizeof(char));//initialize memory
			log_size =0;
		}
		while(wait(NULL) >0)//for complete terminated  all child process 
			;
		get_time(k_time);
		memset(log_info,0,1024*sizeof(char));//initialize memory
		log_size =0;
		printf("[%s] server is terminated.\n",k_time);
		sprintf(log_info,"[%s] server is terminated.\n",k_time);//get print info
		pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread and call print info
                pthread_join(tid,NULL);//thread exit
	        shmctl(shm_id,IPC_RMID,0);//clean shared memory
   		exit(0);
	}
	else if(sig == SIGALRM)//if sigal is SIGALRM print history
	{
		printf("===================== Connection History =======================\n");//print history title
		sprintf(log_info,"===================== Connection History =======================\n");//get print info
		log_size = strlen(log_info);//char that size
 		printf("%s\t%s\t\t%s\t%s\t%s\n","No.","IP","PID","PORT","TIME");//print history  title
		sprintf(&log_info[log_size],"%s\t%s\t\t%s\t%s\t%s\n","No.","IP","PID","PORT","TIME");//get print info additionalli
		pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread and call print info
                pthread_join(tid,NULL);//thread exit
                memset(log_info,0,1024*sizeof(char));//memory initialize
                log_size =0;
		void *shm_addr = shmat(shm_id,NULL,0);
		char buffer[120] ={0,};
		int no =1;
		pthread_mutex_lock(&counter_mutex);
		int temp = ((int*)shm_addr)[3];
		for(int i=temp-1; i>=1; i--)
		{
			strcpy(buffer,((char*)shm_addr) +i* 15 *sizeof(buffer));	
			printf("%d\t%s\n",no,buffer);
			sprintf(log_info,"%d\t%s\n",no,buffer);//get print info about  histroy info
			pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread and call print info
                	pthread_join(tid,NULL);//thread exit
               		memset(log_info,0,1024*sizeof(char));
			no  = no +1;
		}
		pthread_mutex_unlock(&counter_mutex);
		printf("================================================================\n\n");//print history title
                sprintf(log_info,"================================================================\n\n");//get print info
		pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread and call print info
                pthread_join(tid,NULL);//thread exit
                memset(log_info,0,1024*sizeof(char));
		alarm(10);			
	}
	else if(sig == SIGUSR1)//if signal is SIGUSR1 manage idle process
	{
		pid_t pid;
		void *shm_addr = shmat(shm_id,NULL,0);//attach shared memory
		pthread_mutex_lock(&counter_mutex);//thread locking
		int count_idle = *(int*)shm_addr;//get idle count info
		pthread_mutex_unlock(&counter_mutex);//thread unlocking
		if(count_idle <MinIdleNum)//if it needs forked
		{   
		 	for(;count_idle < MinIdleNum;)//loop for fork
			{
				if(total_forked >= MaxChilds)
				{		
					break;
				} 
				IdleProcessCount = count_idle;//get Present idle count
				count_idle = count_idle +1;//increa idle count because for fork
				pthread_mutex_lock(&counter_mutex);//thred locking
				*(int*)shm_addr = *(int*)shm_addr +1;//increase share memory idle count
				pthread_mutex_unlock(&counter_mutex);//thread unlockinig
				pids[total_forked] = child_make(socket_fd,total_forked);//foking
				total_forked = total_forked +1;//increase total forking
			}
		 }
		 else if(count_idle > MaxIdleNum)//if it needs terminate
		 {
			for(int i= StartProcess; i<=MaxIdleNum; i++)
			{
				int status;//variable for get state of terminated child
                        	get_time(k_time);
                        	printf("[%s] %d process is terminated.\n",k_time,pids[i]);//print terminate process ID and terminated time
				sprintf(log_info,"[%s] %d process is terminated.\n",k_time,pids[i]);//get print info
				log_size = strlen(log_info);//check that size
				kill(pids[i],SIGTERM);//kill child process
				wait(&status);//kill compelet
				pids[i] = 0;//init pids info that terminater
				pthread_mutex_lock(&counter_mutex);
				*(int*)shm_addr = *(int*)shm_addr -1;
				printf("[%s] IdleProcessCount : %d\n",k_time,*((int*)shm_addr));
				sprintf(&log_info[log_size],"[%s] IdleProcessCount : %d\n",k_time,*((int*)shm_addr));//get print info
				pthread_mutex_unlock(&counter_mutex);
				pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread and call func
                        	pthread_join(tid,NULL);//thread exit
				log_size = 0;
       				memset(log_info,0,1024*sizeof(char));
			}
	
			if(total_forked > MaxIdleNum)
			{
				total_forked = total_forked - MaxIdleNum;
			}
			for(int i= 1; i<total_forked; i++)
			{
				pids[StartProcess+ i-1] = pids[MaxIdleNum + i];
				pids[MaxIdleNum+ i] = 0;
			}	 
		 }
	}
	else if(sig == SIGUSR2)//if IdleProcess print 
	{
		
		void *shm_addr = shmat(shm_id,NULL,0);//attach memory
		pthread_mutex_lock(&counter_mutex);
		get_time(k_time);			
		printf("[%s] IdleProcessCount : %d\n",k_time,*((int*)shm_addr));//print	
		sprintf(log_info,"[%s] IdleProcessCount : %d\n",k_time,*((int*)shm_addr));//get print info send to log
		pthread_mutex_unlock(&counter_mutex);
		pthread_create(&tid,NULL,&print_to_log,(void*)log_info);//make thread 
                pthread_join(tid,NULL);//thread exit
                memset(log_info,0,1024*sizeof(char));//memory initialize
	}
}
/////////////////////////////////////////////////////////
// access_inspecting                                   //
// ====================================================//
// struct in_addr address : client address	       //
// purpose: for inspecting accessible ip               //                       
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
// purpose: for printing access deny info	       //                       
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
// IDLE_ADD                                            //
// ====================================================//
// purpose:for increase Idle count                     //                       
/////////////////////////////////////////////////////////
void *IDLE_ADD(void *vptr)
{
	 void *shm_addr = 0;
         while((shm_addr = shmat(shm_id,NULL,0)) == (void*)-1)//attach shared memory
         {
                printf("shmat fail\n");
                return NULL;
         }
         *(int*)shm_addr = *(int*)shm_addr +1;//increase Idle count in shared memory
	 kill(pid_p,SIGUSR2);//signal to parent for printing state 
	return NULL;
}
/////////////////////////////////////////////////////////
// IDLE_SUB                                            //
// ====================================================//
// purpose:for decrease Idle count                     //                       
/////////////////////////////////////////////////////////
void *IDLE_SUB(void *vptr)
{
         void *shm_addr = 0;
         while((shm_addr = shmat(shm_id,NULL,0)) == (void*)-1)//attach shared memory
         {
                printf("shmat fail\n");
                return NULL;
         }
         pthread_mutex_lock(&counter_mutex);
         *(int*)shm_addr = *(int*)shm_addr -1;//decrease Idle count in shared memory
	 pthread_mutex_unlock(&counter_mutex); 
	kill(pid_p,SIGUSR2);//signal to parent for printing state 
         return NULL;
}
/////////////////////////////////////////////////////////
// IDLE_MANAGE                                         //
// ====================================================//
// purpose:managing Idle by forking or terminating     //                       
/////////////////////////////////////////////////////////
void *IDLE_MANAGE(void *vptr)
{
         void *shm_addr = 0;
         while((shm_addr = shmat(shm_id,NULL,0)) == (void*)-1)//attach shared memory
         {
                printf("shmat fail\n");
                return NULL;
         }
         pthread_mutex_lock(&counter_mutex);
	 int num = *(int*)shm_addr;//get idle count in shared memory
         pthread_mutex_unlock(&counter_mutex);
         if(num > MaxIdleNum  || num < MinIdleNum)//compare idlecount because for fork or terminate
         {
		kill(pid_p,SIGUSR1);//signal to parent process
	 }
         return NULL;
}
/////////////////////////////////////////////////////////
// child_main	                                       //
// ====================================================//
// socket_id:socket_id that given by parent  	       //
// num_index: that process id hisory head number       //
// purpose: function of action for child process       //                       
/////////////////////////////////////////////////////////
void child_main(int socket_fd,int num_index)
{
	struct sockaddr_in client_addr;//for client
        int client_fd, len_out;
        char buf[BUFSIZE];
	int len;
	char cwd[255];
	char precwd[255];
        socklen_t clien;
	pid_t proces_id;//for keep forked process id
	strcpy(cwd,home);
	proces_id = getpid();//for print process id that forked by parent
	signal(SIGINT,SIG_IGN);//it ignored SIGINT
	signal(SIGALRM,SIG_IGN);//it ignored SIGALRM
	while(1){
		struct in_addr inet_client_address;//for inetoa client address
		FILE* file;//for file open
		char  buf[BUFSIZE]={0,};
		char  tmp[BUFSIZE]={0,};
		char  response_header[BUFSIZE]={0,};
		char* response_message;
		char url[URL_LEN] = {0,};
		char url_disconnect[URL_LEN] = {0,};
		char method[20] = {0,};
		char* tok = NULL;
		int t_BUFSIZE=0;
		char decision[URL_LEN] = {0,};
		char co_time[60], dis_time[60];//for keep connection time and disconnected time
		DIR *drip1;
		pid_t pid;//for get client pid
		int status;//for keep child process status
		clock_t start,end; //start,end for time checking
		int result;//for keeping connecting time
		char log_data[1024] = {0,};
		int temp_length =0;
/////////////////////////////////variable for pthread/////////////////////////////////////////		
		pthread_t tid_A, tid_B, tid_manage, tid_log;
////////////////////////////////variable for pthread///////////////////////////////////////
///////////////////////////////decide prcess count///////////////////////////////////////
                pthread_create(&tid_manage,NULL,&IDLE_MANAGE,NULL);
                pthread_join(tid_manage,NULL);
//////////////////////////////decide process count///////////////////////////////////// 	
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
/////////////////////////////////////////////////////////insert history///////////////////////
		insert_history(&client_addr,getpid());
//////////////////////////////////////////////////////////////////////////////////////////////
		strcpy(url_disconnect,url);//strcpy for url path copy for diconnect
		get_time(co_time);//get time for print connected time
		printf("==========New Client=================\n");
		sprintf(log_data,"==========New Client=================\n");
		temp_length = strlen(log_data);
		printf("TIME : [%s]\n",co_time);//print connected time
		sprintf(&log_data[temp_length],"TIME : [%s]\n",co_time);
		printf("URL : %s\n",url);
		temp_length = strlen(log_data);
		sprintf(&log_data[temp_length],"URL : %s\n",url);
       		printf("IP : %s\n",inet_ntoa(inet_client_address));
		temp_length = strlen(log_data);
		sprintf(&log_data[temp_length],"IP : %s\n",inet_ntoa(inet_client_address));
        	printf("Port : %d\n",client_addr.sin_port);
		temp_length = strlen(log_data);
		sprintf(&log_data[temp_length],"Port : %d\n",client_addr.sin_port);
		printf("PID : %d\n",getpid());
		temp_length = strlen(log_data);
		sprintf(&log_data[temp_length],"PID : %d\n",getpid());
		printf("=====================================\n\n");
		temp_length = strlen(log_data);
		sprintf(&log_data[temp_length],"=====================================\n\n");
		start = clock();//start point of checking clock_time
////////////////////////print New client info/////////////////////////////////////////////////

///////////////////////call print_to_log function/////////////////////////////////////////////
		pthread_create(&tid_log,NULL,&print_to_log,(void*)log_data);
		pthread_join(tid_log,NULL);
		temp_length = 0;
		memset(log_data,0,1024*sizeof(char));
//////////////////////////////////////////////////////////////////////////////////////////////		

////////////////////////////call thread function /////////////////////////////
                pthread_create(&tid_A,NULL,&IDLE_SUB,NULL);
                pthread_join(tid_A,NULL);
///////////////////////////call thread function /////////////////////////////
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
///////////////////////////////decide prcess count///////////////////////////////////////
                pthread_create(&tid_manage,NULL,&IDLE_MANAGE,NULL);
                pthread_join(tid_manage,NULL);
//////////////////////////////decide process count///////////////////////////////////// 
/////////////////////////////calculate connecting time////////////////////////////////// 
		end = clock();
		result = (int)(end - start);
		//result = result * 1000;
////////////////////////////end of calculate//////////////////////////////////////////
//////////////////////sleep for delay disconnection/////////////////////////////
		sleep(5);
//////////////////////sleep for delay disconnection//////////////////////////////
		get_time(dis_time);//for make disconnected time
		printf("======= Disconnected Client ===\n");//print diconnected client info
		sprintf(log_data,"======= Disconnected Client ===\n");
		temp_length = strlen(log_data);
		printf("TIME : [%s]\n",dis_time);//print disconnected time
		sprintf(&log_data[temp_length],"TIME : [%s]\n",dis_time);
		printf("URL : %s\n",url_disconnect);
		temp_length = strlen(log_data);
		sprintf(&log_data[temp_length],"URL : %s\n",url_disconnect);
		printf("IP : %s\n",inet_ntoa(inet_client_address));
		temp_length = strlen(log_data);
		sprintf(&log_data[temp_length],"IP : %s\n",inet_ntoa(inet_client_address));
	        printf("Port : %d\n",client_addr.sin_port);
		temp_length = strlen(log_data);
                sprintf(&log_data[temp_length],"Port : %d\n",client_addr.sin_port);
		printf("PID : %d\n",getpid());
		temp_length = strlen(log_data);
                sprintf(&log_data[temp_length],"PID : %d\n",getpid());
		printf("CONNECTING TIME: %d (us)\n",result);
		temp_length = strlen(log_data);
                sprintf(&log_data[temp_length],"CONNECTING TIME: %d (us)\n",result);
		printf("===============================\n\n");
		temp_length = strlen(log_data);
                sprintf(&log_data[temp_length],"===============================\n\n");
///////////////////////call print_to_log function/////////////////////////////////////////////
                pthread_create(&tid_log,NULL,&print_to_log,(void*)log_data);
                pthread_join(tid_log,NULL);
		temp_length = 0;
                memset(log_data,0,1024*sizeof(char));
//////////////////////////////////////////////////////////////////////////////////////////////
		close(client_fd);
		fclose(file);
		free(response_message);
////////////////////////////call thread function /////////////////////////////
                pthread_create(&tid_B,NULL,&IDLE_ADD,NULL);
                pthread_join(tid_B,NULL);
///////////////////////////call thread function /////////////////////////////
	}
}
/////////////////////////////////////////////////////////
// child_make                                          //
// ====================================================//
// socketfd: for get socket id                         //
// num_index: for keep history node number_index       //
// purpose:make child porcess by forked		       //                       
/////////////////////////////////////////////////////////
pid_t child_make(int socketfd,int num_index)
{
        pid_t pid;
	int status;
	char log_info[1024];
	pthread_t tid;
	int temp = 0;
	if((pid=fork()) >0)//forking
	{
		 char f_time[60];
		 get_time(f_time);//for get forked time
       		 printf("[%s] %d process is forked.\n",f_time,pid);//print that forked sentence
		 sprintf(log_info,"[%s] %d process is forked.\n",f_time,pid);
		 temp = strlen(log_info);
	         IdleProcessCount = IdleProcessCount+1;
		 printf("[%s] IdleProcessCount : %d\n",f_time,IdleProcessCount);
		 sprintf(&log_info[temp],"[%s] IdleProcessCount : %d\n",f_time,IdleProcessCount);
		 pthread_create(&tid,NULL,&print_to_log,(void*)log_info);
    		 pthread_join(tid,NULL);
		 return(pid);//get child process id
	}
	else
	{		
        	child_main(socketfd,num_index);//give sokcet_fd and index to child process
	}
}
int main(){
	struct sockaddr_in server_addr;//, client_addr;
	int  client_fd;//for client and sever socket
	int len, len_out;
	int opt =1;
	char p_sever_time[60];//for keep  parent server open time
	char log_info[1024];//for keep log info
	pthread_t tid_l;//for call thread
//////////////////for get httpd.conf info to global variable////////////////////////
	FILE *file = fopen("httpd.conf","r");
	char *temp_keep_ptr;
	char temp[20];
	int temp_httpd_count=0;
////////////////////////Make semaphore/////////////////////////////////////////////	
	mysem = sem_open("40320",O_CREAT | O_EXCL, 0700,1);	
	sem_close(mysem);
///////////////////////Make semaphore//////////////////////////////////////////////
	while(!feof(file))
	{
		fgets(temp,20,file);
		temp_keep_ptr=strstr(temp," ");
		switch(temp_httpd_count){
			case 0: MaxChilds    = atoi(temp_keep_ptr); break;
			case 1: MaxIdleNum   = atoi(temp_keep_ptr); break;
			case 2: MinIdleNum   = atoi(temp_keep_ptr); break;
			case 3: StartProcess = atoi(temp_keep_ptr); break;
			case 4: MaxHistory   = atoi(temp_keep_ptr); break;
		}
		temp_httpd_count++;
	}
	fclose(file);
///////////////////for get httpd.conf info to global variable/////////////////////////
//////////////////get parent pid//////////////////////////////////////////////
	pid_p =getpid();
//////////////////get parent pid//////////////////////////////////////////////	
//////////////////Make shared memory/////////////////////////////////////////////////
	shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT|0666);
	void *shm_addr = shmat(shm_id,NULL,0);
	*((int*)shm_addr) = StartProcess;
	((int*)shm_addr)[3] = 1;
	((int*)shm_addr)[2] = 1;
//////////////////Make shared memory////////////////////////////////////////////////
	
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
	sprintf(log_info,"\n[%s] Server is started.\n",p_sever_time);//get server start string to log_info
        pthread_create(&tid_l,NULL,&print_to_log,(void*)log_info);//send to print to log
        pthread_join(tid_l,NULL);//thread exit
	listen(socket_fd,5);//for sockt queue
	
	pids = (pid_t *)malloc(MaxChilds * sizeof(pid_t));//make pids memory by malloc for keep preforked process pid
	total_forked = StartProcess;
	memset(pids,0,sizeof(pid_t) *(MaxChilds));
	for(int i=0; i<StartProcess;i++)
	{
		pids[i] = child_make(socket_fd,i);//make child process and get pid info to pids[i]
	}
	for(;;)
	{
		signal(SIGUSR2,signalHandler);
		signal(SIGUSR1,signalHandler);
		signal(SIGINT,signalHandler);//for SIGINT signal handling
  	     	signal(SIGALRM,signalHandler);//for SIGALRM signal Handling
		alarm(10);//for alarming 10 seocnds
		pause();
	}
/////////////////////////detach semaphore////////////////////////////////
	sem_unlink("40320");
//////////////////////////////////////////////////////////////////////////
	return 0;
}
//////////////////////////////////////////////////////////////
//insert_history      	                                    //
//----------------------------------------------------------//
//purpose : for save inserted history                       //
//////////////////////////////////////////////////////////////
void insert_history(struct sockaddr_in* New, pid_t id)
{
   pthread_mutex_lock(&counter_mutex); 
   void *shm_addr = shmat(shm_id,NULL,0);//attach share memory 
   char buffer[120] = {0,};//for save history
   char pr_time[60] ={0,};//for save time info of history
   int temp = ((int*)shm_addr)[3];
   int no_temp = ((int*)shm_addr)[2];
   get_time(pr_time);//make time info
   sprintf(buffer,"%s\t%d\t%d\t%s\n",inet_ntoa(New->sin_addr),id,New->sin_port,pr_time);//save history
   if(no_temp > MaxHistory)//case for history number is over 10
   {
	char temp_buffer[120] = {0,};
	for(int i=1; i<MaxHistory; i++)//loop for sort history by saved time info
	{
		strcpy(temp_buffer,((char*)shm_addr) + (i+1) * 15 * sizeof(buffer));
		strcpy(((char*)shm_addr) + i * 15 * sizeof(buffer),temp_buffer);
	}
	strcpy(((char*)shm_addr) + MaxHistory * 15 * sizeof(buffer),buffer);//save new info	
   }
   else//case for history number is under 10
   {
	   ((int*)shm_addr)[2] = no_temp +1;
 	  strcpy(((char*)shm_addr) + temp * 15 * sizeof(buffer),buffer);
	   ((int*)shm_addr)[3] = temp +1;
   }
   pthread_mutex_unlock(&counter_mutex);
}
//////////////////////////////////////////////////////////////
//print_to_log()                                            //
//----------------------------------------------------------//
// s : string that have info about exe file history         //
//purpose : for printing to log.txt                         //
//////////////////////////////////////////////////////////////
void* print_to_log(void* s)
{
	char info_buf[1024];//variable for get s
	strncpy(info_buf,(char*)s,1024);//copy s data to info_buf
	mysem = sem_open("40320",O_RDWR);//get semephore
	sem_wait(mysem);//for p area
	FILE *log = fopen("server_log.txt","a");//file for write
	fprintf(log,"%s",info_buf);//wirte info to log
	fclose(log);
	sem_post(mysem);//for v area
	sem_close(mysem);//semephore return
}

