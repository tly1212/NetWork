#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<time.h>

#define MYPORT 3991
#define BACKLOG 10
#define MAXDATASIZE 2048

int main(){
  int sockfd, new_fd;
  struct sockaddr_in myaddr;
  struct sockaddr_in cliaddr;
  socklen_t len;
  char buff[80];
  char buffread[MAXDATASIZE];
  int numberbytes;

  //---------------create socket--------------
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("socket error!\n");
    exit(1);
  }

  bzero(&myaddr, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons(MYPORT);
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //----------bind the scoket and listen-----------------
  if(bind(sockfd, (struct sockaddr*) &myaddr, sizeof(myaddr)) == -1){
    printf("bind error\n");
    exit(1);
  }

  if(listen(sockfd, BACKLOG) == -1){
    printf("listen error\n");
    exit(1);
  }
  
  //-------------create a ifinit loop and listen-----------------
  while(1){
    len = sizeof(cliaddr);
    if((new_fd = accept(sockfd, (struct sockaddr *)&cliaddr, &len)) == -1){//accept new request
      printf("accept error\n");
      exit(1);
    }

    //print the host name and IP address
    printf("Connection from %s, Port: %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

    if((numberbytes = read(new_fd, buffread, MAXDATASIZE)) < 0){
      printf("read error\n");
      exit(1);
    }

    //printf(buffread);

    //-------------parse the request message--------------
    char method[100]="";
    char path1[100]; 
    char path[100] = " ";
    char useless[100];
    sscanf(buffread,"%99s %99s %99s", method, path1, useless);// split the message
    // printf("%s\n",method);
    //printf("path = %s\n", path1);
    sscanf(path1,"/%99s",path);
    //printf("path = %s\n",path);


    char message[MAXDATASIZE]= "";
    FILE *fptr;
    fptr = fopen(path,"r");
    struct stat buf;
    char content[MAXDATASIZE];
    char content2[MAXDATASIZE];
    time_t t;
    time(&t);
    char bufsize[100];

    //-------check the status of the path and create response message-----------
    if(stat(path,&buf)!=0){
      strcat(message,"HTTP/1.0 404 Not Found");      
    }else{
      if(!strcmp(method, "GET")){ // find out if the request is GET
	strcat(message,"HTTP/1.1 200 OK\n"); //create the header
	strcat(message,"Content-Length:");
	sprintf(bufsize, "%d",  buf.st_size);
	strcat(message,bufsize);
	strcat(message,"\r\n\r\n");
	char ch;
	while(ch != EOF){//read the whole file and store it to message
	  ch = fgetc(fptr);
	  strncat(message, &ch,1); // attach the content to return message
	}
	//	printf("content = %s\n", content);
	
      }
      else{
	strcat(message,"HTTP/1.0 200 OK\n");
	strcat(message,"Last-Modified:");
	strcat(message, ctime(&t));
	strcat(message,"\r\n\r\n");
      }
    }


    if(write(new_fd, message,strlen(message)) < 0){
      printf("send error\n");
      exit(1);
    }

    close(new_fd);
  }  
}
