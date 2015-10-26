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
#include <sstream>

using namespace std;

enum pstatus { SCHEDULED, RUNNING, SLEEPING, DEAD };

ostringstream exec_log;

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
	int id;
	string path;

	Process(string path, int time, int id){
		this->status = SCHEDULED;
		this->path = path;
		this->runtime = time;
		this->id = id;
	}

	void run(){
		this->status = RUNNING;
		//this->runtime = 0;

		pid_t pid;

		if((pid = fork()) < 0){
			perror("fork");
		}else if(pid == 0){
			execv(path.c_str(), NULL);
		}else{
			wait(NULL);
			exec_log << "\n";
			exec_log << this->id;
			exec_log << "\t";
			exec_log << this->path;
			exec_log << "\t\t\t";
			exec_log << (int)(this->runtime / 60);
			exec_log << ":";
			exec_log << (int)(this->runtime % 60);
			exec_log << "\n"; 
			this->status = DEAD;
		}
	}

	bool isJobId(int id){
		if(this->id == id)
			return true;
		else
			return false;
	}

};

key_t	msgkey;
int		msgqid;
int 	msgflag;
int		msgsize;
int 	job_id;
vector<Process> proc_scheduled;
queue<Process> proc_running;

void dummy(){}

void listaProcessos(){
	cout << endl << "job\tarq_exec\t\t\thhmm" << endl;

	vector<Process>::iterator it = proc_scheduled.begin();
	while(it != proc_scheduled.end()){
		cout << it->id << "\t" << it->path << "\t\t" << (int)(it->runtime / 60) << ":" << (int)(it->runtime % 60) << endl;
		++it;
	}
}

void desligaServidor(int sig){
	cout << endl << "DESLIGANDO SERVIDOR..." << endl << "ESSES PROCESSOS NAO SERAO EXECUTADOS";
	listaProcessos();
	cout << endl << "ESSES PROCESSOS FORAM EXECUTADOS" << endl << exec_log.str();
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
	cout << endl << "JOB ID" << job_id << ": PROCESSO " << path << " COM OFFSET DE " << offset << " MINUTOS E EXECUTANDO " << times << " VEZES" << endl;
	int current_time = getCurrentTime();
	for(int i = 1; i <= times; ++i){
		proc_scheduled.push_back(Process(path, current_time + i*offset, job_id));
	}
	job_id++;
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

void removeProcesso(int id){
	vector<Process>::iterator it = proc_scheduled.begin();
	while(it != proc_scheduled.end()){
		if(it->id == id){
			it = proc_scheduled.erase(it);
		}else{
			++it;
		}
	}
}

int main(){

	signal(SIGTERM, desligaServidor);
	//signal(SIGALRM, dummy);

	exec_log << "job\tarq_exec\thhmm";
	job_id 	= 0;
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

		if(-1 != msgrcv(msgqid, &msg, msgsize, -4, IPC_NOWAIT)){

			// DESLIGAR SERVIDOR
			if(msg.mtype == 1)
				raise(SIGTERM);
			// LISTAR PROCESSOS
			else if(msg.mtype == 2)
				listaProcessos();
			// REMOVER PROCESSO
			else if(msg.mtype == 3)
				removeProcesso(msg.proc.offset);
			// ADICIONAR PROCESSO
			else if(msg.mtype == 4)
				addToSchedule(msg.proc.path, msg.proc.offset, msg.proc.times);
			
		}

		checkSchedule();

		runProcesses();

		sleep(1);

	}

	return 0;
}