#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

using namespace std;

struct msgtype{
	long mtype;
	char pname[100];
};

key_t	msgkey;
int		msgqid;
int 	msgflag;
int		msgsize;

void executa(char* pname){

	pid_t pid;
	char new_pname[102];

	strcat(new_pname, "./");
	strcat(new_pname, pname);

	if((pid = fork()) < 0){
		perror("fork");
	}else if(pid == 0){
		execv(pname, NULL);
	}else{
		wait(NULL);
	}

}


int main(){

	msgkey 	= 100126481;
	msgflag = IPC_CREAT | 0666;
	msgsize = sizeof(struct msgtype) - sizeof(long);

	struct msgtype msg;

	// Cria a fila de mensagens
	if((msgqid = msgget(msgkey, msgflag)) < 0){
        perror("msgget");
        exit(1);
    }

	while(true){

		msgrcv(msgqid, &msg, msgsize, 0, 0);

		executa(msg.pname);

		//cout << msg.pname << endl;

	}

	return 0;
}