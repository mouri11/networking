/* echoServer.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> 



#define MAX_MSG 100



int main (int argc, char *argv[]) {
  
  int sd, newSd, cliLen, n;

  struct sockaddr_in cliAddr, servAddr;
  char line[MAX_MSG];

  /***************************/
  /* check command line args */
  /***************************/

  if(argc < 3) {
    printf("usage: %s <server-addr> <server-port>\n",argv[0]);
    exit(1);
  }

  /**********************************/	
  /* build server address structure */
  /**********************************/	

  bzero((char *)&servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(argv[1]);
  servAddr.sin_port = htons(atoi(argv[2]));

  /************************/	
  /* create stream socket */
  /************************/	

  sd = socket(AF_INET, SOCK_STREAM, 0);

  if(sd<0) {
    printf("%s : cannot create stream socket \n", argv[0]);
    exit(-1);
  }
  else
    printf("%s : successfully created stream socket \n", argv[0]);

  /**************************/	
  /* bind local port number */
  /**************************/	
  
  if(bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
    printf("%s : cannot bind port \n", argv[0]);
    exit(-1);
  }
  else
    printf("%s : bound local port successfully\n", argv[0]);


  /********************************/	
  /* specify number of concurrent */ 
  /* clients to listen for        */
  /********************************/	

  listen(sd,5);
 

 
  while(1) {

printf("%s %d: waiting for client connection on port TCP %u\n",argv[0], getpid(), atoi(argv[2]));

    /*****************************/
    /* wait for client connection*/
    /*****************************/    

    newSd = accept(sd, (struct sockaddr *) &cliAddr, &cliLen);

    if(newSd<0) {
      printf("%s : cannot accept connection \n", argv[0]);
      exit(-1);
    }
    else
      printf("%s %d: received connection from host [IP %s ,TCP port %d]\n",argv[0], getpid(),
                 inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
 
    /*****************************/
    /* wait for data from client */
    /*****************************/       
    
    if(fork()==0)
    {
      do{
        memset(line,0x0,MAX_MSG);
    
        n=recv(newSd, line, MAX_MSG, 0);
        line[n]='\n'; 
    
       

 printf("%s %d: received from host [IP %s ,TCP port %d] : %s\n",
         argv[0],getpid(),inet_ntoa(cliAddr.sin_addr), 
         ntohs(cliAddr.sin_port), line);

      printf("\nSENDING TO CLIENT..........\n");
	send(newSd, line, strlen(line) + 1, 0);
      }while(abs(strcmp(line, "quit")));

      /**************************/
      /* close client connection*/
      /**************************/    

//printf("%s %d: closing connection with host [IP %s ,TCP port %d]\n",argv[0],getpid(),inet_ntoa(cliAddr.sin_addr),ntohs(cliAddr.sin_port));

      close(newSd);
      exit(1);
    }
    else
      close(newSd);
  } 
}

