#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define size 4096

char msg[size];
char esend[size];
char * emsg;
char username[16];
int sockFD;
fd_set masterlist;
fd_set listenlist;


char* cipher (char * test, int k) {
  int i = 0;
  while(test[i] != '\0' && test[i] != '\n') {
    if(test[i] >= 33 && test[i] <= 125) {
      if (k == 1) {
	test[i]++;
      }
      else {
	test[i]--;
      }
    }
    i++;
  }
  return test;
}

void send(int sockfd){
  memset(msg, '\0', sizeof(msg));
  
  fgets(msg, size, stdin);
  
  if (strncmp(msg, "QUIT", 4) == 0) {
    printf("Client closed\n");
    exit(0);
  }
  
  if(strncmp(msg, "HELP", 4) == 0) {
    printf("List of commands:\nREGISTER [username] [password] | Registers a new account\nLOGIN [username] [password] | Log into an existing account\nLOGOUT | Log out of account\nSEND [message] | Send a public message to all online use\
rs\nSENDA [message] | Send an anonymous public message to all online users\nSEND [username] [message] | Send a private message to a user\nSENDA [username] [message] | Send a private anonymous message to a user\nSENDE [username] [message \
] | Send a private encrypted message to a user\nLIST | Lists all online users\n");
  }
  else if (strncmp(msg, "SENDE", 5) == 0) {
    memset(esend, '\0', sizeof(esend));
    sscanf(msg, "SENDE %s", username);
    emsg = cipher(msg + 7 + strlen(username), 1);
    strcpy(esend, "SENDE ");
    strcat(esend, username);
    strcat(esend, " ");
    strcat(esend, emsg);
    send(sockfd, msg, strlen(msg) - 1, 0);
  }

  else {
    send(sockfd, msg, strlen(msg) - 1, 0);
  }
}

void receive(int sockfd){
  memset(msg, '\0', sizeof(msg));
  int nrecv; 
  nrecv = recv(sockfd, msg, size, 0);
  if(nrecv <= 0) {
    printf("Server has closed\n");
    exit(0);
  }
  if(strncmp(msg, "SENDE", 5) == 0) {
    memset(esend, '\0', sizeof(esend));      
    strcpy(msg, cipher(msg + 6, 0));
    printf("%s\n", msg);
  }
  else {
    printf("%s\n", msg);
    memset(msg, '\0', sizeof(msg));
    fflush(stdout);
  }
}

void connecter(int *sockFD){

  *sockFD = socket(AF_INET, SOCK_STREAM, 0);
  
  if(*sockFD == -1) {
    printf("Cannot create socket\n");
    exit(0);
  }

  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(6000);
  serverAddr.sin_addr.s_addr = inet_addr("54.68.61.201");

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  
  if(connect(*sockFD, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
    printf("Error connecting to server\n");
    exit(0);
  }
  printf("You are now online, type HELP for commands and QUIT to quit\n");
}

void runserver() {
  while(1){
    listenlist = masterlist;
    select(4, &listenlist, NULL, NULL, NULL);
    if(FD_ISSET(0, &listenlist)) {
      send(sockFD);
    }
    if(FD_ISSET(sockFD, &listenlist)) {
      receive(sockFD);
    }
  }
}


int main()
{
  memset(msg, '\0', sizeof(msg));
  connecter(&sockFD);
  FD_ZERO(&masterlist);
  FD_ZERO(&listenlist);
  FD_SET(0, &masterlist);
  FD_SET(sockFD, &masterlist);
  runserver();
  return 0;
}
