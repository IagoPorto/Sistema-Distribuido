#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define N 5 // --> nodos
#define P 5 // --> procesos
#define MAX_ESPERA 10
#define SLEEP 3

struct msgbuf_mensaje{
  long msg_type;
  int id;
  int peticion;
  int atendidas[N];
};

struct {
    //VARIABLES GLOBALES
    bool testigo = false;
    bool permiso_para_S_C_E_M = false;
    bool tengo_que_pedir_testigo = true;
    int mi_id;
    int atendidas[N] = {}, peticiones[N] = {};
    int buzones_nodos[N];

    //SEMÁFOROS GLOBALES
    sem_t sem_testigo, sem_mi_id, sem_atendidas, sem_buzones_nodos,
        sem_peticiones, sem_tengo_que_pedir_testigo, sem_permiso_para_S_C_E_M;

    //VARIABLES PROCESOS
    int mi_peticion = 0;
    struct msgbuf_mensaje solicitud;
    int contador_procesos = 0, contador_espera = 0;
    int contador_procesos_SC = 0;

    //SEMÁFOROS PROCESOS
    sem_t sem_mi_peticion, sem_espera_procesos, 
            sem_contador_procesos, sem_contador_espera,
            sem_contador_procesos_SC;

    sem_t sem_S_C_E_M;
}memoria;