#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>
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

int main(int argc, char *argv[]){

	msgkey 	= 100126481;
	msgflag = 0666;
	msgsize = sizeof(struct msgtype) - sizeof(long);

	struct msgtype msg = { 1, " " };

	strcpy(msg.pname, argv[1]);

	// Se conecta a fila de mensagens
	if((msgqid = msgget(msgkey, msgflag)) < 0){
        perror("msgget");
        exit(1);
    }

    // Envia a mensagem
    if(msgsnd(msgqid, &msg, msgsize, 0) < 0){
    	perror("msgsnd");
    }

	return 0;
}