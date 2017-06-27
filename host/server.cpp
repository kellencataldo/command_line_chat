#include "header.h"
#include "users.cpp"
#define size 4096

fd_set masterlist;
fd_set listenlist;
int sockFD;
int max;
struct sockaddr_in myAddr;

void debug(char const * loc){
  printf("got to: %s\n", loc);
}

void sendtoall(char *recv, int k, int a) {
  char msg[size];
  char user[16];
  struct user from;
  from = returnuser(k);

  strcpy(msg, "Public message from: ");
  strcat(msg, from.name);

  for(i = 0; i < 16; i++) {
    if((userlist[i].fd != k) && (userlist[i].active == 1) && (userlist[i].online == 1)) {
      if(a == 1) {
	send(userlist[i].fd, msg, strlen(msg), 0);
	send(userlist[i].fd, recv + 5, strlen(recv), 0);
      }
      if(a == 0) {
	send(userlist[i].fd, "Public message from anonymous user", 40, 0);
	send(userlist[i].fd, recv + 6, strlen(recv), 0);
      }
    }
  }
}


void lister(int k) {  
  int i;
  char msg[size];
  strcpy(msg, "Users online: ");
  for(i = 0; i < 16; i++) {
    if(userlist[i].online == 1) {
      strcat(msg, userlist[i].name);
      strcat(msg, " ");
    }
  }
  send(k, msg, strlen(msg), 0);
}


  void processmsg(int i, int nrecv, char * recv){  
    char msg[size];  
    char error[40];  
    int val;  
    int msgsize; 
    char username[16];
    char password[16];
    int to;
    struct user address;
    int index = returnindex(i);
    int status = userlist[index].online;    

    //register
    if(strncmp(recv, "REGISTER", 8) == 0) {
      if(status == 1) {
	send(i, "You must log out to register another user", 44, 0);
      } 
      else { 
	sscanf(recv, "REGISTER %s %s", username, password);
	if(strlen(username) < 4 || strlen(username) > 16 || strlen(password) < 4 || strlen(password) > 16) {
	  send(i, "Username and password  must each be between 8 and 16 characters", 40, 0);
	} else {
	  val = adduser(username, password, i);    
	  if(val == 0) {
	    send(i, "Username in use", 20, 0);
	  
	  } else if (val == 1) {
	    
	    send(i, "User creation success", 21, 0);
	    strcpy(msg, "Username is: ");
	    strcat(msg, username);
	    strcat(msg, ", password is: ");
	    strcat(msg, password);
	    send(i, msg, strlen(msg), 0);
	    printf("User %s registered\n", username);
	    
	  } else if (val == 2) {
	    send(i, "User limit reached", 20, 0);
	  }
	}
      }    
    } 
    
    //delete account
    else if(strncmp(recv, "DELACCT", 7) == 0) {
      if(status == 0) {
        send(i, "You must log in to an account to delete it", 45, 0);
      }
      else {
	userlist[i].active = 0;
	userlist[i].fd = -1;
	userlist[i].online = 0;
        send(i, "Account deleted", 15, 0);
      }
    }

    //send anonymous
    else if(strncmp(recv, "SENDA", 5) == 0) {
      if(status == 0) {
	send(i, "You must log in first to send messages", 50, 0);
      }
      else {
	sscanf(recv, "SENDA %s", username);
	val = find(username);
	
        if(val == 16) {
          sendtoall(recv, i, 0);
        }
	else {
	  if(userlist[val].online == 1) {
	    send(userlist[val].fd, "Message from anonymous user:", 40, 0);
	    send(userlist[val].fd, recv + 7 + strlen(username), nrecv, 0);
	  }
	  if(userlist[val].online == 0) {
	    send(i, "User is offine", 40, 0);
	  }
	}
      }
    }
    
    //send ecrypted
    else if(strncmp(recv, "SENDE", 5) == 0) {
      if(status == 0) {
	send(i, "You must log on first to send messages", 50, 0);
      }
      else {
	sscanf(recv, "SENDE %s", username);
	val = find(username);

	if(val == 16) {
	  send(i, "Error, please check spelling of username", 50, 0);
	}
	else {
	  if(userlist[val].online == 1) {	  
	    strcpy(msg, "Encrypted message from: ");	  
	    strcat(msg, userlist[index].name);
	    send(userlist[val].fd, msg, 50, 0);
	    strcpy(msg, "SENDE ");
	    strcat(msg, recv + 7 + strlen(userlist[val].name));
	    send(userlist[val].fd, msg, nrecv, 0);
	  }
	}
      }
    }

    //send
    else if (strncmp(recv, "SEND", 4) == 0) {
      if(status == 0) {
	send(i, "You must log in first to send messages", 50, 0);
      }
      else {
	sscanf(recv, "SEND %s", username);
	val = find(username);
	if(val == 16) {
	  sendtoall(recv, i, 1);
	}
	else {
	  if(userlist[val].online == 1) {
	    strcpy(msg, "Message from: ");
	    strcat(msg, userlist[index].name);
	    send(userlist[val].fd, msg, 40, 0);
	    send(userlist[val].fd, recv + 6 + strlen(username), nrecv, 0);
	  }
	  else {
	    send(i, "User is offline", 40, 0);
	  }
	}
      }
    }
    //log on

    else if (strncmp(recv, "LOGIN", 5) == 0) {
      if(status == 1) {
	send(i, "You must log out to log in to a different account", 50, 0);
      }
      else {
	sscanf(recv, "LOGIN %s %s", username, password);
	val = find(username);
	
	if(val == 16) {
	  send(i, "Username not registered", 30, 0);
	}
	else if(userlist[val].online == 1) {
	  send(i, "User with that username is already online", 40, 0);
	}
	else {
	  val = logon(username, password, i);
	  if(val == 1) {
	    send(i, "Log on successful", 25, 0);
	  }
	  else if(val == 0) {
	    send(i, "Incorrect password", 25, 0);
	  }
	}
      }
    }

    //log off
    else if (strncmp(recv, "LOGOUT", 6) == 0) {
      if(status == 0) {
	send(i, "You are not logged in", 25, 0);
      }
      else {
	userlist[index].online = 0;
	userlist[index].fd = -1;
	send(i, "You are now logged out", 25, 0);
      }
    }

    else if(strncmp(recv, "LIST", 4) == 0) {
      lister(i);
    }
    
    else {
      send(i, "Unknown command, type HELP for information", 50, 0);
    } 	  
      
  }


void datarecv(int i) {
  int nrecv;
  char data[size];
  int index;
  index = returnindex(i);
  if ((nrecv = recv(i, data, size, 0)) <= 0) {
    // index = returnindex(i);
      if(index != 16) {
	userlist[index].online = 0;
	userlist[index].fd = -1;
	printf("User %s has disconnected off\n", userlist[index].name);
      } else {
	printf("User on socket %d has disconnected\n", i);
      } 	
      close(i);
      FD_CLR(i, &masterlist);
    }
    else {
      if(strlen(data) > 1) {
	if(index != 16) { 
	  printf("Command from user %s\n", userlist[index].name);
	  printf("%s\n", data);
	}
	else {
	  printf("Command from FD %d\n", i);
	}
      }
      processmsg(i, nrecv, data);
      memset(data, '\0', size); 
    }
}

void connection_accept()
{

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);

  int newsock;
  sockaddr_in * loc = &clientAddr;

  newsock = accept(sockFD, (struct sockaddr *)loc, &clientAddrLen);

  if(newsock == -1) {
    printf("Can't accept new connecttion\n");
    exit(0);

  } else {
    FD_SET(newsock, &masterlist);

    if(newsock > max){
      max = newsock;
    }
    printf("New connection from this IP: %s, socket addrress is %d\n",inet_ntoa(clientAddr.sin_addr), newsock);
  }
}

void listensocket() {

 
  sockFD = socket(AF_INET, SOCK_STREAM, 0);
  if(sockFD == -1) {
    printf("Could not create listening socket\n");
    exit(0);
  }
  
  myAddr.sin_family = AF_INET;
  myAddr.sin_port = htons(6000);
  myAddr.sin_addr.s_addr = INADDR_ANY;

  int optval = 1;

  if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
    printf("Could not set socket\n");
  }
  
  sockaddr_in * loc = &myAddr;

  if (bind(sockFD, (struct sockaddr *)loc, sizeof(struct sockaddr)) == -1) {
    printf("Could not bind socket\n");
    exit(0);
  }
  if (listen(sockFD, 10) == -1) {
    printf("Could not set socket state to listen\n");
    exit(0);
  }
  printf("Server online\n");
  fflush(stdout);
}


void doserver() {
  int i; 
  while(1){
    listenlist = masterlist;
    select(max+1, &listenlist, NULL, NULL, NULL);
    for (i = 0; i <= max; i++){
      if (FD_ISSET(i, &listenlist)){
        if (i == sockFD) {
          printf("New connection incoming\n");
          connection_accept();
        }
        else
          datarecv(i);
      }
    }
  }
}


int main()
{
 
  memset(&userlist, 0, sizeof(userlist));
  FD_ZERO(&masterlist);
  FD_ZERO(&listenlist);
  listensocket();
  FD_SET(sockFD, &masterlist);
  max = sockFD;
  
  doserver();

  return 0;
}
