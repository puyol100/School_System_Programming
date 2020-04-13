# include <stdio.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fnmatch.h>
# include <unistd.h>
# include "web_serv.h"///////////////////////////////////////////////////////////////
# define URL_LEN 255
# define BUFSIZE 1024
# define PORTNO  40320
#define	FNM_CASEFOLD	(1 << 4)
int count_file_size(FILE *file){
	int size;
	fseek(file,0,SEEK_END);
	size = ftell(file);
	fseek(file,0,SEEK_SET);
	return size;
}
int main(){
	struct sockaddr_in server_addr, client_addr;
	int socket_fd, client_fd;
	int len, len_out;
	int opt = 1;
	char home[255];
	char cwd[255];
	char precwd[255];
	getcwd(home,255);
	strcpy(cwd,home);
	if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server : Can't open stream socket \n");
		return 0;
	}
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt));
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNO);
	if(bind(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) <0){
		printf("Server : Can't bind local address \n");
		return 0;
	}
	listen(socket_fd,5);
	while(1)
	{
		struct in_addr inet_client_address;
		FILE* file;
		char  buf[BUFSIZE]={0,};
		char  tmp[BUFSIZE]={0,};
		char  response_header[BUFSIZE]={0,};
		char* response_message;
		char url[URL_LEN] = {0,};
		char method[20] = {0,};
		char* tok = NULL;
		int t_BUFSIZE=0;
		char decision[URL_LEN] = {0,};
		DIR *drip1;
		len = sizeof(client_addr);
		client_fd = accept(socket_fd, (struct sockaddr*)&client_addr,&len);
		if( client_fd <0)
		{
			printf("Server : accept failed\n");
			return 0;
		}
		inet_client_address.s_addr = client_addr.sin_addr.s_addr;
		printf("[%s : %d] client was connected\n",inet_ntoa(inet_client_address),client_addr.sin_port);
//////////////////////////////////////////////////////////////
		if(read(client_fd,buf,BUFSIZE)<=0)
		{
			printf("abnormal connection");
			close(client_fd);
			continue;
		}
		printf("buf : %s\n",buf);
                strcpy(tmp,buf);
                puts("=============================================");
                printf("Request from [%s : %d]\n",inet_ntoa(inet_client_address),client_addr.sin_port);
                puts(buf);
                puts("=============================================");

                tok = strtok(tmp," ");
                strcpy(method,tok);
                if(strcmp(method,"GET") ==0)
                {
                        tok = strtok(NULL," ");
                        strcpy(url,tok);//url has absolute path
                }
		if(strcmp(url,"/favicon.ico")==0)
		{
			printf("inputting favicon.ico\n");
			close(client_fd);
			continue;
		}
/////////////////////////////////////////////////////////////////////////
		printf("first url : %s\n",url);
		strcpy(cwd,home);
		if(strcmp(url,"/")==0)
		{
			   strcpy(url,home);
		}
		else
		{
			   printf("cwd : %s\n",cwd);
	                   printf("url : %s\n",url);
			   strcat(cwd,url);
	                   strcpy(url,cwd);  
		  	   printf("final : %s\n",url);
		}
///////////////////////////////////////////////////////////////////////////////
		if(((drip1=opendir(url)) == NULL) || (fnmatch("*.html",url,0) ==0))
		{	
		   if((fnmatch("*.html",url,0)==0))
		   {
		   	strcpy(decision,"text/html");
			file = fopen(url,"r");
		   }
		   else if((fnmatch("*.jpg",url,FNM_CASEFOLD)==0)||
                    	  (fnmatch("*.png",url, FNM_CASEFOLD)==0)||
                    	  (fnmatch("*.jpeg",url, FNM_CASEFOLD)==0))
                   {
                        strcpy(decision,"image/*");
			file = fopen(url,"rb");
		   }
		   else
		   {
                        file = fopen(url,"r");
			if(file == NULL)
			{
			   web_serv_info_print(url,home);
			   file = fopen("html_ls.html","r");
			}
			strcpy(decision,"text/plain");
		   }
		}
		else
		{
		  if(drip1!=NULL)
		  {
		     closedir(drip1);	
		  }
		strcpy(decision,"text/html");
		web_serv_info_print(url,home);
		strcpy(url,"html_ls.html");
	        file = fopen(url,"r");
                }
//////////////////////////////////////////////////////////////////////////////////////////////////
		if(file == NULL)
                {
                        printf("file open error\n");
                        return -1;
                }
                t_BUFSIZE = count_file_size(file);
                printf("%d\n",t_BUFSIZE);
                response_message = malloc(t_BUFSIZE+1);
		
///////////////////////////////////////////////////////////////////////
		for(int i=0; i<t_BUFSIZE; i++)
		{
			response_message[i] = fgetc(file);

		}
		printf("testing wtg : %s\n",decision);
		sprintf(response_header,
			 "HTTP/1.0.200 OK\r\n"
			 "Server:2019 simple web server\r\n"
			 "Content-length:%d\r\n"
			 "Content-type: %s\r\n\r\n",t_BUFSIZE,decision);
		write(client_fd, response_header, strlen(response_header));
		write(client_fd, response_message,t_BUFSIZE);
		
		printf("[%s: %d] client was disconnected\n",inet_ntoa(inet_client_address),client_addr.sin_port);
		close(client_fd);
		fclose(file);
		free(response_message);
	     }
		close(socket_fd);
	return 0;
}
