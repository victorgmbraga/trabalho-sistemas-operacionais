#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

using namespace std;

struct msgtype{
	long mtype;
	char pname[100];
};

key_t	msgkey;
int		msgqid;
int 	msgflag;
int		msgsize;
char 	msgdata[100];


int main(){

	msgkey 	= 100126481;
	msgflag = IPC_CREAT | 0666;
	msgsize = sizeof(struct msgtype) - sizeof(long);

	struct msgtype msg;

	if((msgqid = msgget(msgkey, msgflag)) < 0){
        perror("msgget");
        exit(1);
    }else{
    	cout << "Abri a fila de mensagens" << endl;
    }

	while(true){

		msgrcv(msgqid, &msg, msgsize, 0, 0);

		cout << msg.pname << endl;

	}

	return 0;
}