#include "header.h"

typedef struct {
	int num_aluno;
	char nome[50];
	char turma[10];
	char telemovel[15];
	char email[40];
	char c_credito[20];
	int rating;
} Tpassageiro;

typedef struct {
	long tipo;
	struct {
		int pid_passageiro;
		int pid_condutor;
		char local_encontro[100];
		long data;
		float valor;
	} dados;
} MsgCorrida;

struct sembuf UP = {0, 1, 0};
struct sembuf DOWN = {0, -1, 0};

void validar_num(); //ponto 1
void envia_msg(); //ponto 2
void recebe_msg(); //ponto 3

int i;
char numero[50];
Tpassageiro *p=NULL;
MsgCorrida enviar_msg;
MsgCorrida receber_msg;
int msg_id;
int status;
int sem_id;
int status_s;

int main() {
	int id=shmget(7345, 500*sizeof(Tpassageiro), 0);
	exit_on_error(id, "Ligação");
	printf("Ligado a memoria\n", id);
	p = shmat (id, 0, 0 );
	exit_on_null(p, "shmat");

	validar_num();
	envia_msg();
	recebe_msg();
	
	exit(0);
}

void validar_num(){
	sem_id = semget(7345, 1, 0);
	exit_on_error(sem_id, "Ligação");
	int encontrou=0;
		printf("Número do aluno:\n");
		fgets(numero, 50, stdin);
		numero[strlen(numero)-1]='\0';
		
		status_s = semop(sem_id, &DOWN, 1);
		exit_on_error(status_s, "DOWN");
		
		for (i=0; i<500; i++) {
			if (p[i].num_aluno == atoi(numero)) {
				printf("O nome do aluno é %s\n", p[i].nome);
				encontrou=1;
				break;
			}
		}
		
		status_s=semop(sem_id, &UP, 1);
		exit_on_error(status_s, "UP");
		
	if (encontrou==0) {
		printf("Esse número não existe na memória.\n");
		validar_num();
	}
}

void recebe_msg(){
	msg_id=msgget(96975, 0);
	exit_on_error(msg_id, "Ligação");
	
	printf("À espera de uma mensagem...\n");
	status = msgrcv(msg_id, &receber_msg, sizeof(receber_msg.dados), enviar_msg.dados.pid_passageiro, 0);
	exit_on_error (status, "Recepção");
	
	printf("O pid do condutor é %d\n", receber_msg.dados.pid_condutor);
}

void envia_msg(){
	char l[100];
	time_t t=time(NULL);
	
	printf("Indique o local de encontro.\n");
	fgets(l, 100, stdin);
	l[strlen(l)-1] = '\0';
	
	msg_id = msgget (96975, 0);
	exit_on_error (msg_id, "Criação/Ligação");
	
	strcpy(enviar_msg.dados.local_encontro, l);
	enviar_msg.tipo = 1;
	enviar_msg.dados.pid_passageiro = getpid();
	enviar_msg.dados.data = t;
	
	status = msgsnd( msg_id, &enviar_msg, sizeof(enviar_msg.dados), 0);
	exit_on_error (status, "Erro no envio\n");
	
	printf("Mensagem enviada!\n");
}
