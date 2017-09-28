#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>


#define MAXDATASIZE 2048



int main(int argc, char *argv[]){
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in servaddr;

	//check the number if the arguments
	if(argc != 2 && argc != 3){
	  printf("usage: invalid input!\n");
	}

	// --------------parse url-----------------
	char *url;
	int PORT = 3991;
	if(argc == 2){
	  url =argv[1];
	}else{
	  url = argv[2];
	}
	char identifier1[100] = " ";
	char identifier2[100]; 
	char identifier[100] = {"/"};
	char hostname[100];
	char hostname2[100];

	sscanf(url, "http://%99[^:]:%99d/%99[^\n]", hostname2, &PORT, identifier1);//split the argument
	if(strstr(hostname2, "/") == NULL){     //check if port included or not
	  strcpy(hostname,hostname2);
	  //printf("hostname2 = \"%s\"\n", hostname);
	  // printf("port = \"%d\"\n", PORT);
	  // printf("identifier1 = \"%s\"\n", identifier1);
	  if(strstr(identifier1, " " )) {  
	    strcat(identifier, identifier1);//append the path to "/"
	  }
	  // printf("identifier = %s\n", identifier);
	}else{     //split the hostname and identifier
	  sscanf(hostname2, "%99[^/]/%99[^\n]", hostname, identifier2);
	  printf("hostname = \"%s\"\n", hostname);
	  printf("port = \"%d\"\n", PORT);
	  printf("identifier2 = \"%s\"\n", identifier2);
	  strcat(identifier, identifier2);
	  printf("identifier = %s\n", identifier);
	}


	//---------------construc message------------
	char msg[100] = ""; // request message to be sent to server
	int onlyhead; // check whether -h is icluded in the arguments or not.
	if (argc == 3){
	  onlyhead = 1;
	}else{
	  onlyhead = 0;
	}

	if(!onlyhead){
	  strcat(msg, "GET ");
	  //printf("%s\n",identifier);
	  //printf("%s\n",msg);
	  //printf("%s\n",identifier);
	  strcat(msg,identifier);
	  //printf("%s\n",msg);
	  strcat(msg, " HTTP/1.0\r\n\r\n");
	}else{
	  strcat(msg, "HEAD ");
	  strcat(msg,identifier);
	  strcat(msg, " HTTP/1.0\r\n\r\n");
	}
       	printf("Message: %s",msg);
	
	

	//----------------create socket and connect----------------------
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	  printf("socket error!\n");
	  exit(1);
	}

	if ((he=gethostbyname(hostname)) == NULL) { // get the host info of server
	  printf("gethostbyname error!\n");
	  exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);

	memcpy(&servaddr.sin_addr, he->h_addr_list[0], he->h_length); 

        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
	  printf("connect error!\n");
	  exit(1);
	}

	// send GET or HEAD request msg to server
	if (write(sockfd,msg,strlen(msg)) < 0) {
	  printf("send error!\n");
	  exit(1);
	}

	// credits to He Jin for the help of this part
	int in,index = 0,limit = MAXDATASIZE;
	char message[MAXDATASIZE+1];
	FILE *file = fopen("reponse.txt", "w+");  

	do {
	  bzero(message, MAXDATASIZE+1);
	  in = recv(sockfd, message, MAXDATASIZE, 0);
	  if ( in > 0 ) { //ckeck if there is more information
	    int results = fputs(message, file); // write to file
	    if (results == EOF) { // ckeck whether reaches the end of file
	      printf("usage: store error!\n");
	    }
	  }
	}
	while ( in > 0 );

	close(sockfd);
}
