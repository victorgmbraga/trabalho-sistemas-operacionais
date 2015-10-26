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
#include <map>
#include <queue>
#include <time.h>

using namespace std;

enum pstatus { SCHEDULED, RUNNING, SLEEPING, DEAD };

struct msgtype{
	long mtype;
	struct process{
		char path[100];
		int offset;
		int times;
	} proc;
};

class Process{
public:
	pstatus status;
	int runtime;
	string path;

	Process(string path, int time){
		this->status = SCHEDULED;
		this->path = path;
		this->runtime = time;
	}

	void run(){
		this->status = RUNNING;
		this->runtime = 0;

		pid_t pid;

		if((pid = fork()) < 0){
			perror("fork");
		}else if(pid == 0){
			execv(path.c_str(), NULL);
		}else{
			wait(NULL);
			this->status = DEAD;
		}
	}

};

key_t	msgkey;
int		msgqid;
int 	msgflag;
int		msgsize;
vector<Process> proc_scheduled;
queue<Process> proc_running;

void dummy(){}

void desligaServidor(int sig){
	msgctl(msgqid, IPC_RMID, NULL);
	exit(0);
}

void executa(char* path){

	pid_t pid;

	if((pid = fork()) < 0){
		perror("fork");
	}else if(pid == 0){
		execv(path, NULL);
	}else{
		wait(NULL);
	}

}

int getCurrentTime(){
	time_t theTime = time(NULL);
	struct tm *tm_struct = localtime(&theTime);
	int minute = tm_struct->tm_min;
	int hour = tm_struct->tm_hour;
	int tempo_minutos = (hour*60 + minute) % 1440;
	return tempo_minutos;
}

void addToSchedule (char* path, int offset, int times){
	int current_time = getCurrentTime();
	for(int i = 1; i <= times; ++i){
		proc_scheduled.push_back(Process(path, current_time + i*offset));
	}
}

// Verifica a lista de postergados
// e adiciona a lista de em execucao caso seja hora
void checkSchedule(){
	int current_time = getCurrentTime();
	vector<Process>::iterator it = proc_scheduled.begin();
	while(it != proc_scheduled.end()){
		if(it->runtime <= current_time){
			it->status = SLEEPING;
			proc_running.push(*it);
			it = proc_scheduled.erase(it);
		}else{
			++it;
		}
	}
}

void runProcesses(){
	if(!proc_running.empty()){
		proc_running.front().run();
		proc_running.pop();
	}
}

int main(){

	signal(SIGTERM, desligaServidor);
	//signal(SIGALRM, dummy);

	msgkey 	= 0x10012648;
	msgflag = IPC_CREAT | 0666;
	msgsize = sizeof(struct msgtype) - sizeof(long);

	struct msgtype msg;

	// Cria a fila de mensagens
	if((msgqid = msgget(msgkey, msgflag)) < 0){
        perror("msgget");
        exit(1);
    }

	while(true){

		if(-1 != msgrcv(msgqid, &msg, msgsize, -2, IPC_NOWAIT)){

			if(msg.mtype == 1)
				raise(SIGTERM);

			addToSchedule(msg.proc.path, msg.proc.offset, msg.proc.times);
			
		}

		checkSchedule();

		runProcesses();

		sleep(1);

	}

	return 0;
}