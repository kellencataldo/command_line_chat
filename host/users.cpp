#include "header.h"

struct user {
  int active;  
  char name[15];
  char password[15];
  int online;
  int fd;
} ;

user userlist[17];
int i;

user returnuser(int ad){
  for(i = 0; i < 16; i++) {
    if(ad == userlist[i].fd && userlist[i].active != 0) {
      return userlist[i];
    }
  }
  return userlist[17]; 
}

int returnindex(int ad){
  for(i = 0; i < 16; i++) {
    if(ad == userlist[i].fd && userlist[i].active != 0) {
      return i;
    }
  }
  return 16;
}


int checkuser(char * uname) {
  for(i = 0; i < 16; i++) {
    int eq = strcmp(uname, userlist[i].name);
    if(eq == 0 && userlist[i].active){
      return 1;
    }
  }
  return 0;
}

int adduser(char * uname, char * pword, int fd) {  
  if(checkuser(uname)) {
    return 0;
  }

  for(i = 0; i < 16; i++){
    if(userlist[i].active == 0) {
      userlist[i].active = 1;
      strcpy(userlist[i].name, uname);
      strcpy(userlist[i].password, pword);
      userlist[i].online = 1;
      userlist[i].fd = fd;
      return 1;
    }
  }
  return 2;
} 

int logon(char * uname, char * pword, int ad) {
  for(i = 0; i < 16; i++) {
    if(strcmp(uname, userlist[i].name) == 0 && userlist[i].active == 1) {
      if(strcmp(pword, userlist[i].password) == 0) {
	userlist[i].online = 1;
	userlist[i].fd = ad;
	return 1;
      }
      else {
	return 0;
      }
    }
  }
}

int sender(char * uname) {
  for(i = 0; i < 16; i++) {
    if (strcmp(userlist[i].name, uname) == 0 && userlist[i].active == 1) {
      if(userlist[i].online == 1) {
	return 1;
      } else {
	return 0;
      }
    }
  }
  return 2;
}

int find(char * uname) {
  for(i = 0; i < 16; i++) {
    if(strcmp(userlist[i].name, uname) == 0 && userlist[i].active == 1) {
      return i;
    }
  }
  return 16;
}







