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
	char dummy;
};

int main(){

	// Definições da mensagem a ser enviada
	key_t msgkey 	= 0x10012648;
	int msgflag 	= 0666;
	int msgsize 	= sizeof(struct msgtype) - sizeof(long);

	// Monta a mensagem
	struct msgtype msg = { 1, 'a' };

	// Se conecta a fila de mensagens
	int msgqid;
	if((msgqid = msgget(msgkey, msgflag)) < 0){
        perror("msgget");
        exit(1);
    }

    // Envia a mensagem
    if(msgsnd(msgqid, &msg, msgsize, 2) < 0){
    	perror("msgsnd");
    }

	return 0;
}