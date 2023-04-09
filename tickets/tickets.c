#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>

#define N 5

int ticket = 0;
int max_ticket = 0;
int mi_id;

int id_nodos_pend[N];
int buzones_nodos[N];

bool quiero = false;

sem_t sem_ticket, sem_max_ticket, sem_mi_id,
        sem_id_nodos_pend, sem_buzones_nodos, 
        sem_quiero; 

struct msgbuf_solicitud{
  int id;
  int ticket;
};
struct msgbuf_respuesta{
    int id;
};

void *receptor(void *n);
int max(int n1, int n2);

int main(int argc, char *argv[]){

    if(argc != 2){
        printf("La forma de ejecución es: %s \"id_nodo\"\n", argv[0]);
        return -1;
    }

    //INICIALIZACIÓN VARIABLES
    mi_id = atoi(argv[1]);
    int yo = mi_id;
    int i;
    int id_mi_buzon;

    struct msgbuf_solicitud msg_solicitud;
    msg_solicitud.id = mi_id;
    struct msgbuf_respuesta msg_recivido, msg_enviar_ok;
    msg_enviar_ok.id = mi_id;


    //INICIALIZACIÓN BUZONES DE LOS NODOS
    for (i = 0; i < N; i++){

        buzones_nodos[i] = msgget(i + 1, IPC_CREAT | 0777);
        printf("El id del buzón del nodo %i es: %i.\n", i + 1, buzones_nodos[i]);

        if(buzones_nodos[i] == -1){

            perror("No se creó el mensaje\n");
        }
    }

    id_mi_buzon = buzones_nodos[yo - 1];

    //INICIALIZACIÓN SEMÁFOROS
    sem_init(&sem_ticket, 0, 1);
    sem_init(&sem_max_ticket, 0, 1);
    sem_init(&sem_mi_id, 0, 1);
    sem_init(&sem_buzones_nodos, 0, 1);
    sem_init(&sem_id_nodos_pend, 0, 1);
    sem_init(&sem_quiero, 0, 1);

    //INICIALIZACIÓN HILO RECEPTOR
    pthread_t hilo;
    int id_hilo = pthread_create(&hilo, NULL, receptor, NULL);//Hilo destinado a la recepción de mensajes.
    if(id_hilo != 0){
        printf("No se ha podido crear el hilo.\n");
        return -1;
    }

    while(1){

        printf("Esperando para entrar en la sección crítica\n");

        getchar();

        sem_wait(&sem_quiero);
        quiero = true;
        sem_post(&sem_quiero);
        sem_wait(&sem_ticket);
        sem_wait(&sem_max_ticket);
        ticket = max_ticket++;
        sem_post(&sem_max_ticket);
        msg_solicitud.ticket = ticket;
        sem_post(&sem_ticket);

        //ENVIO PETICIONES
        for(i = 0; i < N; i++){
            
            if(yo - 1 == i){

                printf("No me voy a enviar un mensaje a mi mismo\n");
                continue;

            }else{

                printf("Enviando solicitud al nodo: %d\n", i + 1);
                sem_wait(&sem_buzones_nodos);
                if(msgsnd(buzones_nodos[i], &msg_solicitud, sizeof(msg_solicitud), 0) == -1){

                    sem_post(&sem_buzones_nodos);
                    printf("\n\tERROR: Hubo un error enviando el mensaje al nodo: %i.\n", i);
                }else{
                    
                    sem_post(&sem_buzones_nodos);
                    printf("Mensaje enviado con EXITO. \n");
                }
            }
        }
        printf("Hemos enviado todos los tickets\n");

        //ENVIO PETICIONES
        for(i = 0; i < N; i++){
            
            if(yo - 1 == i){

                printf("No me voy a recibir un mensaje de mi mismo\n");
                continue;

            }else{

                printf("Enviando solicitud al nodo: %d\n", i + 1);
                if(msgrcv(id_mi_buzon, &msg_recivido, sizeof(msg_recivido), 0, 0) == -1){

                    printf("\n\tERROR: Hubo un error enviando el mensaje al nodo: %i.\n", i);
                }else{
                    
                    printf("Mensaje enviado con EXITO. \n");
                }
            }
        }
        printf("Hemos recibido todas las respuestas\n");

        printf("\n\t\tESTAMOS DENTRO DE LA SECCIÓN CRÍTICA\n\n");

        getchar();

        sem_wait(&sem_quiero);
        quiero = false;
        sem_post(&sem_quiero);

        for(i = 0; i < N; i++){

            sem_wait(&sem_id_nodos_pend);
            if(id_nodos_pend[i] > 0){

                sem_post(&sem_id_nodos_pend);
                sem_wait(&sem_buzones_nodos);
                if(msgsnd(buzones_nodos[i], &msg_enviar_ok, sizeof(msg_enviar_ok), 0) == -1){

                    sem_post(&sem_buzones_nodos);
                    printf("\n\tERROR: Hubo un error enviando el mensaje al nodo: %i.\n", i);
                }else{
                    
                    sem_post(&sem_buzones_nodos);
                    printf("Mensaje enviado con EXITO. \n");
                }
                id_nodos_pend[i] = 0;

            }else{

                sem_post(&sem_id_nodos_pend);
            }
        }

    }

    return 0;

}

void *receptor(void *n){

    struct msgbuf_solicitud msg_peticion;
    struct msgbuf_respuesta msg_respuesta;
    sem_wait(&sem_mi_id);
    int mi_id_receptor = mi_id - 1;
    sem_post(&sem_mi_id);
    msg_respuesta.id = mi_id_receptor + 1;
    sem_wait(&sem_buzones_nodos);
    int id_de_mi_buzon = buzones_nodos[mi_id_receptor];
    sem_post(&sem_buzones_nodos);

    while(true){

        //RECIBIMOS PETICIÓN
        if(msgrcv(id_de_mi_buzon, &msg_peticion, sizeof(msg_peticion), 0, 0) == -1){
            printf("ERROR: Hubo un error al recibir un mensaje en el RECEPTOR.\n");
        }
        printf("He recibido un mensaje del nodo: %d\n", msg_peticion.id);
        
        sem_wait(&sem_max_ticket);
        max_ticket = max(max_ticket, msg_peticion.ticket);
        sem_post(&sem_max_ticket);

        sem_wait(&quiero);
        sem_wait(&sem_ticket);
        if(!quiero || msg_peticion.ticket < ticket || (msg_peticion.ticket == ticket  && msg_peticion.id < mi_id_receptor + 1)){
            sem_post(&quiero);
            sem_post(&sem_ticket);
            sem_wait(&sem_buzones_nodos);
            if(msgsnd(buzones_nodos[msg_peticion.id - 1], &msg_respuesta, sizeof(msg_respuesta), 0) == -1){

                sem_post(&sem_buzones_nodos);
                printf("\n\tERROR: Hubo un error enviando el mensaje al nodo: %i.\n", msg_peticion.id);
            }else{
                
                sem_post(&sem_buzones_nodos);
                printf("Mensaje enviado con EXITO. \n");
            }
        }else{
            sem_post(&quiero);
            sem_post(&sem_ticket);
            sem_wait(&sem_id_nodos_pend);
            id_nodos_pend[msg_peticion.id - 1]++;
            sem_post(&id_nodos_pend);
        }

    }

}

int max(int n1, int n2){
  if(n1 > n2) return n1;
  else return n2;
}