#include "header.h"

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

typedef struct {
	int num_aluno;
	char nome[50];
	char turma[10];
	char telemovel[15];
	char email[40];
	char marca[20];
	char matricula[15];
	int rating;
	float saldo;
	int activo;
	long disponivel_desde;
	int PID;
} Tcondutor;

typedef struct {
	char data[20];
	int na_condutor;
	int na_passageiro;
	float valor;

} Tcorrida;

struct sembuf UP = { 0, 1, 0 } ;
struct sembuf DOWN = { 0, -1, 0 } ;


typedef struct {
	int num_aluno;
	char nome[50];
	char turma[10];
	char telemovel[15];
	char email[40];
	char c_credito[20];
	int rating;
} Tpassageiro;


void read_passageiros();

void read_condutores();
void obter_substring(char linha[], char resultado[], char separador, int indice);
void read_passageiros();
void makePID();
void printmem();
void mudaPassageiro();
void mudaCondutor();
void recebeSIGHUP();
void recebeSIGTERM();
void menu();
int handleMsg();
void criaFila();

int lastP;
int lastC;


void obter_substring(char linha[], char resultado[], char separador, int indice) {

	int i, j=0, meu_indice = 0;
	for (i=0; linha[i] != '\0'; i++) {
		if ( linha[i] == separador ) {
			meu_indice++;
		} else if (meu_indice == indice) {
			resultado[j++]=linha[i];
		}
	}
	resultado[j]='\0';

}


readmem() {

	int sem_id = semget(73452,1, 0666 | IPC_CREAT);
        exit_on_error(sem_id,"Criar semaforo");



	int id = shmget (7345, 500*sizeof(Tpassageiro), 0);
	exit_on_error (id, "Criação/Ligação");
	Tpassageiro *p = shmat ( id, 0, 0 );
	exit_on_null (p, "Attachw");
	int i;


	int status =semop(sem_id,&DOWN,1);
	exit_on_error(status,"read");



	for ( i = 0; i < lastP; i++) {
		printf("%d : %s - %s -  %s -  %s -  %s -  %d \n",p[i].num_aluno
				,p[i].nome,p[i].turma
				,p[i].telemovel
				,p[i].email 
				,p[i].c_credito
				,p[i].rating);
	}
	
	 status = semop(sem_id,&UP,1);
	exit_on_error(status,"read");


}


readmem2(){


	int sem_id = semget(73452,1, 0666 | IPC_CREAT);
        exit_on_error(sem_id,"Criar semaforo");

	int idcondutor = shmget(73452, 500*sizeof(Tcondutor),0);
	exit_on_error(idcondutor,"Ligacao");
	Tcondutor *c = shmat(idcondutor,0,0);
	exit_on_null(c,"Attach");
	int j;
	 int status = semop(sem_id,&DOWN,1);
	exit_on_error(status,"read2");

	for(j = 0; j < lastC; j++){
		printf("%d : %s - %s - %s - %s - %s - %s - %d - %0.2f - %d - %ld - %d\n",
				c[j].num_aluno,c[j].nome,c[j].turma,c[j].telemovel,c[j].email,
				c[j].marca,c[j].matricula,c[j].rating,c[j].saldo,c[j].activo,
				c[j].disponivel_desde,c[j].PID);
	}

	 status = semop(sem_id,&UP,1);
	exit_on_error(status,"read2");
}





main(){


int sem_idC = semget(73452,1, 0666 | IPC_CREAT);
int sem_idP = semget(7345,1, 0666 | IPC_CREAT);

int semcond = semctl (sem_idC, 0, SETVAL, 1);
int semPass = semctl (sem_idP, 0, SETVAL, 1);

	
	criaFila();
	read_condutores();
	read_passageiros();

	menu();

}



void read_condutores(){

	int sem_id = semget(73452,1, 0666 | IPC_CREAT);
	exit_on_error(sem_id,"Criar semaforo");	

	Tcondutor condutores[500];
	char line[500];
	char resultado[40];

	FILE *ficheiro_condutores;
	ficheiro_condutores = fopen("condutores.txt","r");

	int idC = shmget(73452, 500*sizeof(Tcondutor),IPC_CREAT | 0666);
	exit_on_error(idC,"Criar IPC");

	Tcondutor *condutor = shmat(idC,0,0);

	int status = semop(sem_id, &DOWN,1);
	exit_on_error(status,"DOWN readCond");
	while(fgets(line,500,ficheiro_condutores) != NULL){

		Tcondutor a;

		obter_substring(line,resultado,';',1);
		strcpy(a.nome,resultado);
		

		obter_substring(line,resultado,';',2);
		strcpy(a.turma,resultado);

		obter_substring(line,resultado,';',3);
		strcpy(a.telemovel,resultado);

		obter_substring(line,resultado,';',4);
		strcpy(a.email,resultado);

		obter_substring(line,resultado,';',5);
		strcpy(a.marca,resultado);

		obter_substring(line,resultado,';',6);
		strcpy(a.matricula,resultado);

		obter_substring(line,resultado,';',0);
		a.num_aluno = atoi(resultado);

		obter_substring(line,resultado,';',8);
		a.saldo = atof(resultado);

		obter_substring(line,resultado,';',7);
		a.rating = atoi(resultado);        

		obter_substring(line,resultado,';',9);
		a.activo = atoi(resultado);

		obter_substring(line,resultado,';',10);
		a.disponivel_desde = atol(resultado);

		obter_substring(line,resultado,';',11);
		a.PID = atoi(resultado);

		condutor[lastC] = a;
		lastC++;

	}
	status = semop(sem_id,&UP,1);
	exit_on_error(status,"criacond");
	fclose(ficheiro_condutores);
}




void read_passageiros(){

	int sem_id = semget(7345,1, 0666 | IPC_CREAT);
	exit_on_error(sem_id,"Cria sem readPAS");

	Tpassageiro passageiros[500];
	char resultado[40];
	char line[500];
	FILE *file_passageiros;
	file_passageiros = fopen("passageiros.txt","r");

	int idP = shmget(7345,500*sizeof(Tpassageiro),IPC_CREAT | 0666);
	exit_on_error(idP,"Criar passageiros");


	Tpassageiro *passageiro = shmat(idP,0,0);
	exit_on_null(passageiro,"erro pass");

	int status = semop(sem_id, &DOWN,1);
	exit_on_error(status,"semop pass");
	int i;

	for(i = 0; i < 500; i++){
		strcpy(passageiro[i].nome,"coiso");
		strcpy(passageiro[i].turma,"coiso");
		strcpy(passageiro[i].telemovel,"coiso");;
		strcpy(passageiro[i].email,"coiso");
		strcpy(passageiro[i].c_credito,"coiso");
		passageiro[i].num_aluno = 0;
		passageiro[i].rating = 0;
	}

	while(fgets(line,500,file_passageiros) != NULL){
		Tpassageiro a;

		obter_substring(line,resultado,';',1);
		resultado[strlen(resultado)] = '\0';
		strcpy(a.nome,resultado);
		obter_substring(line,resultado,';',2);
		resultado[strlen(resultado)] = '\0';
		strcpy(a.turma,resultado);

		obter_substring(line,resultado,';',3);
		resultado[strlen(resultado)] = '\0';
		strcpy(a.telemovel,resultado);

		obter_substring(line,resultado,';',4);
		resultado[strlen(resultado)] = '\0';
		strcpy(a.email,resultado);

		obter_substring(line,resultado,';',5);
		resultado[strlen(resultado)] = '\0';
		strcpy(a.c_credito,resultado);

		obter_substring(line,resultado,';',0);
		resultado[strlen(resultado)] = '\0';
		a.num_aluno = atoi(resultado);

		obter_substring(line,resultado,';',6);
		resultado[strlen(resultado)] = '\0';
		a.rating = atof(resultado);

		passageiro[lastP] = a;
		lastP++;
	}
	
	status = semop(sem_id,&UP,1);
	exit_on_error(status,"criapass");

	fclose(file_passageiros);
}

void menu(){

	char op[2];
	int opcao;
	printf("\nEscolha o numero da opcao\n");
	printf(" 1 - Imprimir memoria\n 2 - Alterar Passageiro\n 3 - Alterar Condutor\n");
	printf("Opcao: ");
	fgets(op,3,stdin);
	opcao = atoi(op);
	switch(opcao){

		case 1:
			printf("Imprime Memoria\n");
			readmem();
			readmem2();
			menu();
			break;
		case 2:
			printf("Altera Passageiro\n");
			mudaPassageiro();
			menu();
			break;
		case 3:
			printf("Altera Condutor\n");
			mudaCondutor();
			menu();
			break;

		case 4:
			printf("msg\n");
			handleMsg();
			menu();
			break;

		default:
			printf("Nao e uma opcao valida\n");
			menu();
	}

}


void mudaPassageiro(){

	char num[50];
	int nr;
	char resultado[50];
	printf("Insira o numero do Passageiro: ");
	fgets(num,50,stdin);
	sscanf(num,"%d",&nr);


	 int sem_id = semget(7345,1, 0666 | IPC_CREAT);
        exit_on_error(sem_id,"Cria sem readPAS");
		

	int idP = shmget(7345,500*sizeof(Tpassageiro),0);

	Tpassageiro *passageiros = shmat (idP,0,0);
	exit_on_null(passageiros,"mudapassageiro");

	int status = semop(sem_id,&DOWN,1);
	exit_on_error(status,"mudapass");



	int i;
	for(i = 0; i < lastP; i++){
		if(passageiros[i].num_aluno == nr){
			printf("Insira o novo nome: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(passageiros[i].nome,resultado);

			printf("Insira a nova turma: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(passageiros[i].turma,resultado);

			printf("Insira o novo telemovel: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(passageiros[i].telemovel,resultado);

			printf("Insira o novo email: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(passageiros[i].email,resultado);

			printf("Insira o novo cartao de credito: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(passageiros[i].c_credito,resultado);

		}
	}
	status = semop(sem_id,&UP,1);
	exit_on_error(status,"mudapas");



}

void mudaCondutor(){


 	int sem_id = semget(7345,1, 0666 | IPC_CREAT);
        exit_on_error(sem_id,"mudac");


	char num[50];
	int nr;
	char resultado[50];
	printf("Insira o numero do Condutor: ");
	fgets(num,50,stdin);
	sscanf(num,"%d",&nr);

	int idC = shmget(73452,500*sizeof(Tcondutor),0);

	Tcondutor *condutores = shmat(idC,0,0);
	exit_on_null(condutores,"mudacondutor");


	int status = semop(sem_id,&DOWN,1);
	exit_on_error(status,"mudacond");


	int i;
	for(i = 0; i < lastC; i++){
		if(condutores[i].num_aluno == nr){
			printf("Insira o novo nome: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(condutores[i].nome,resultado);

			printf("Insira a nova turma: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(condutores[i].turma,resultado);

			printf("Insira o novo telemovel: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(condutores[i].telemovel,resultado);

			printf("Insira o novo email: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(condutores[i].email,resultado);

			printf("Insira a nova marca do automovel: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(condutores[i].marca,resultado);

			printf("Insira a nova matricula do automovel: ");
			fgets(resultado,50,stdin);
			resultado[strlen(resultado) - 1] = '\0';
			strcpy(condutores[i].matricula,resultado);

		}
	}
	status = semop(sem_id,&UP,1);
	exit_on_error(status,"mudacond");


}

int handleMsg(){

	int idC = shmget(73452,500*sizeof(Tcondutor),0);



	Tcondutor *c = shmat(idC,0,0);
	exit_on_null(c,"mudacondutor");


	int i;
	int menor = 0;		
	



	for(i = 0; i < 500; i++){
		if(c[i].activo == 1){
			if(c[i].disponivel_desde > c[menor].disponivel_desde){
				menor = i;
			}
		}		
	}	
	c[menor].activo = 0;
	printf("Escolhido - %s\n",c[menor].nome);

	return c[menor].PID;
}


void criaFila(){
	int id = msgget(96975, IPC_CREAT | 0666);
	exit_on_error(id,"msg");
	printf("Fila de Mensagens criada com sucesso\n");


	int idfilho = fork();
	MsgCorrida msg;

	 int idC = shmget(73452,500*sizeof(Tcondutor),0);

        Tcondutor *c = shmat(idC,0,0);
        exit_on_null(c,"recibo");

	if(idfilho == 0){
		 int sem_id = semget(7345,1, 0666 | IPC_CREAT);
	        exit_on_error(sem_id,"Cria sem readPAS");


		printf("Criou o filho\n");
		while(1){
			printf("Chegou ao filho\n");
			int status = msgrcv(id, &msg, sizeof(msg.dados),1,0);
			exit_on_error(status,"recebermsg");
			


			if(msg.dados.pid_condutor == 0){
				printf("Mensagem 1\n");
				int statushand = semop(sem_id,&DOWN,1);
				int pid = handleMsg();
				statushand = semop(sem_id,&UP,1);
				msg.tipo = pid;
				int sent = msgsnd(id,&msg,sizeof(msg.dados),0);
				exit_on_error(sent,"erro a enviar");	

			} else {
				int i;
				printf("Recebeu recibo\n");
				printf("%d",lastC);
				int statusem = semop(sem_id,&DOWN,1);
				exit_on_error(statusem,"filho");
				for(i = 0; i<500; i++){
					if(c[i].PID == msg.dados.pid_condutor){
						printf("encontrou\n");
						c[i].saldo += msg.dados.valor;
						c[i].disponivel_desde = time();
						c[i].activo = 1;
						printf("Alterou\n");
					}
				}
				statusem = semop(sem_id,&UP,1);
				exit_on_error(statusem,"filho");
			}
	
		}
		}
		
}



