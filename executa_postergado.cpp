#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
	msgflag = 0666;
	msgsize = sizeof(struct msgtype) - sizeof(long);

	struct msgtype msg = { 1, "MENSAGEM DE TESTE" };

	if((msgqid = msgget(msgkey, msgflag)) < 0){
        perror("msgget");
        exit(1);
    }else{
    	cout << "Me conectei a fila de mensagens" << endl;
    }

    if(msgsnd(msgqid, &msg, msgsize, 0) == -1){
    	perror("msgsnd");
    }else{
    	cout << "Enviei a mensagem" << endl;
    }

	return 0;
}