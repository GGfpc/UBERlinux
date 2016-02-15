#include "header.h"

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
        long tipo;
        struct {
                int pid_passageiro;
                int pid_condutor;
                char local_encontro[100];
                long data;
                float valor;
        } dados;
} MsgCorrida;

int n;

void acabou(int sinal) {
        int id_mem = shmget(73452, 500 * sizeof(Tcondutor), 0);
        exit_on_error(id_mem, "shmget");

        Tcondutor *p = shmat(id_mem, 0, 0);
        exit_on_null(p, "shmat");

        int i;
        for(i = 0; i < 500; i++) {
                if(n == p[i].num_aluno) {
                        p[i].activo = 0;
                        printf("Fora de serviço\n");
                        printf("Saiu\n");
                        exit(0);
                }
        }

}

void up(int id) {
        struct sembuf UP = {0, 1, 0};
        int semStatus = semop(id, &UP, 1);
        exit_on_error(semStatus, "UP");
}

void down(int id) {
        struct sembuf DOWN = {0, -1, 0};
        int semStatus = semop(id, &DOWN, 1);
        exit_on_error(semStatus, "DOWN");
}


main() {

        signal(SIGINT,acabou);

        // ligar ao semáforo
        int semid = semget(73452, 1, 0);
        exit_on_error(semid, "semget");

        // ligar à memória partilhada
        int id_mem = shmget(73452, 500 * sizeof(Tcondutor), 0);
        exit_on_error(id_mem, "shmget");
        printf("Ligado à memória\n");

        // Erro do ponteiro
        Tcondutor *p = shmat(id_mem, 0, 0);
        exit_on_null(p, "shmat");

        // input do número do condutor
        char num_aluno[100];
        printf("Número de condutor: ");
        fgets(num_aluno, 100, stdin);
        n = atoi (num_aluno);

        // percorrer a memória partilhada de 500 pos
        // se o número da memória é igual ao inserido
        int i;
        printf("Número inserido: %d\n", n);

        down(semid); // down do semáforo
        for(i = 0; i < 500; i++) {
                if( n == p[i].num_aluno) {
                        p[i].activo = 1; // ativar o condutor
                        printf("Condutor ativo\n");
                        p[i].disponivel_desde = time(NULL); // data atual
                        p[i].PID = getpid(); // o seu PID
  printf("Já és condutor: %s\nA tua matrícula é: %s\n", p[i].nome,p[i].matricula);
                break;
                }
        }
        up(semid); // up do semáforo


        // ligar à fila de mensagens
        int id_msg = msgget(96975, 0);
        exit_on_error(id_msg, "Erro no msgget");
        printf("Ligado à fila\n");

        // ao receber a mensagem 2
        MsgCorrida m;
        while(1) {

                // recebe mensagem 2 do tipo pid
                int status = msgrcv(id_msg, &m, sizeof(m.dados), getpid(), 0);
                exit_on_error(status, "Erro ao receber mensagem 2");
                printf("Mensagem 2 recebida\n");

                // imprimir o pid do passageiro
                printf("O PID do passageiro é: %d\n", m.dados.pid_passageiro);

                // enviar a mensagem 3
                char local[100];
                m.tipo = m.dados.pid_passageiro; // tipo com o PID do passageiro
                m.dados.pid_condutor = getpid(); // PID do condutor com o meu PID
                printf("Meu PID: %d\n", getpid()); // confirmar o meu pid com o Admin e o Passageiro
                printf("Minha localização: ");
                fgets(local, 100, stdin); // minha localização
                local[ strlen(local) - 1 ] = '\0';

                status = msgsnd(id_msg, &m, sizeof(m.dados), 0);
                exit_on_error(status, "Erro ao enviar mensagem 3");
                printf("Mensagem 3 enviada\n");

                // espera de 1-20s
                srand(time(NULL));
                sleep(rand() %20 + 1);

                // pede o valor da corrida
                char val[10];
                printf("Valor da corrida: ");
                fgets(val, 10, stdin);
                val[ strlen(val) - 1 ] = '\0';

                // enviar mensagem 4 com recibo
                m.tipo = 1;
 		m.dados.pid_passageiro = m.dados.pid_passageiro;
                strcpy(m.dados.local_encontro, local);
                m.dados.valor = atof(val);

                status = msgsnd(id_msg, &m, sizeof(m.dados), 0);
                exit_on_error(status, "Erro ao enviar mensagem 4");
                printf("Mensagem 4 enviada\n");
        }

        acabou(SIGINT); // Sinal do Ctrl C
}




