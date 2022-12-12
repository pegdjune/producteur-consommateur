#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

struct shared_memory {
  int buffer[BUFFER_SIZE];
  int head;
  int tail;
  int count;
};
sem_t semaphore , max;
int main(int argc, char const *argv[]) {

    sem_init (& semaphore, 0, 1);
    static int it;

    if(argc != 4){
		fprintf(stderr, "USAGE: %S <Nombre de producteurs> <NOMBRE DE CONSOMMATEURS> <Nombre maximum d'elements qui peuvent etre produire> \n", argv[0]);
		return 1;
	}
	
	int P=atoi(argv[1]);
	int C=atoi(argv[2]);
	int max_data=atoi(argv[3]);

    sem_init(&max, 0, max_data);

  // Créer la mémoire partagée
  int shmid = shmget(IPC_PRIVATE, sizeof(struct shared_memory), 0666 | IPC_CREAT);
  if (shmid < 0) {
    perror("shmget");
    exit(1);
  }

  // Attacher la mémoire partagée à l'espace de travail du processus
  struct shared_memory* shm = shmat(shmid, NULL, 0);
  if (shm == (void*) -1) {
    perror("shmat");
    exit(1);
  }

  // Initialiser la mémoire partagée
  shm->head = 0;
  shm->tail = 0;
  shm->count = 0;
//--------------------------------------------------------------------------------------------

pid_t pidp[P];
  for(int i = 0; i < P; i++){
    pidp[i] = fork();
    if(pidp[i] < 0){
      perror("fork");
      exit(1);
    }
    if(pidp[i] == 0){
      int item = 0;

    while (1) {
        printf("Producteur %d: début\n", getpid());
      // Attendre que le buffer ne soit pas plein
      //sem_wait(&max);
      sem_wait(& semaphore) ;
      while (shm->count >= BUFFER_SIZE){}

        // Ajouter un nouvel item au buffer
        printf("Producteur %d : Ajout d'item %d au buffer\n", getpid(), item);
        shm->buffer[shm->tail] = item;
        shm->tail = (shm->tail + 1) % BUFFER_SIZE;
        shm->count++;
        item++;
        // Endormir le processus pour simuler un temps de traitement
        sleep(rand() % 5);
        //shmdt(shm);
        sem_post(&semaphore);
        printf("Producteur %d: fin\n", getpid());

        sem_post(&semaphore);
        it++;
        if(it>=max_data){exit(1);}
        
       
    }
    return 0;
    
    }else{
      fprintf(stderr, "Je suis le processus (%d) qui vient de créer le producteur (%d)\n", getpid(), pidp[i]);
    }
  }

    int pstatus;
        for(int i = 0; i < P; i++){
            pidp[i] = wait(&pstatus);
            //fprintf(stderr, "Mon fils (%d) vient de se terminer (%d)\n", pid[i], status);
        }

  pid_t pidc[C];
  for(int i = 0; i < C; i++){
    pidc[i] = fork();
    if(pidc[i] < 0){
        perror("fork");
        exit(1);
    }
    if(pidc[i] == 0){
      int item;
      while (1) {
        printf("Consommateur %d : début\n", getpid());
        sem_wait(&semaphore);
        // Attendre que le producteur produise un élément
        //printf("Conxxxx : début\n");
        while (shm->count <= 0){}

        // Récupérer l'élément en tête du buffer
        item = shm->buffer[shm->head];
        // Retirer un item du buffer
        shm->head = (shm->head + 1) % BUFFER_SIZE;
        shm->count--;

        printf("Consommateur %d consomme item: %d\n",getpid(), item);
        //printf("Consommateur: Suppression d'item %d du buffer\n", shm->buffer[shm->head]);

        // Endormir le processus pour simuler un temps de traitement
        sleep(rand() % 5);
        //shmdt(shm);
        printf("Consommateur %d : fin\n", getpid());
        sem_post(&semaphore);
        //sem_post(&max);
        
        it++;
        if(it>=max_data){exit(1);}
        
        
    }
    
    return 0;
    
    }else{
      fprintf(stderr, "Je suis le processus (%d) qui vient de créer le consommateur (%d)\n", getpid(), pidc[i]);
    }
  }
 

 int cstatus;
  for(int i = 0; i < C; i++){
    pidc[i] = wait(&cstatus);
   //fprintf(stderr, "Mon fils (%d) vient de se terminer (%d)\n", pid[i], status);
  }
    return 0; 
}







/*
//-------------------------------------------------------------------------------------------
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(1);
  }


  // Processus enfant (consommateur)
  if (pid == 0) {
    
    int item;
    while (1) {
      printf("Consommateur %d : début\n", getpid());
      // Attendre que le producteur produise un élément
      while (shm->count <= 0){}

        // Récupérer l'élément en tête du buffer
        item = shm->buffer[shm->head];
        // Retirer un item du buffer
        shm->head = (shm->head + 1) % BUFFER_SIZE;
        shm->count--;

        printf("Consommateur %d consomme item: %d\n",getpid(), item);
        //printf("Consommateur: Suppression d'item %d du buffer\n", shm->buffer[shm->head]);

        // Endormir le processus pour simuler un temps de traitement
        sleep(rand() % 5);
      
        printf("Consommateur %d : fin\n", getpid());
        if(item>=max_data){exit(1);}
    }
    
    return 0;

  }
  // Processus parent (producteur)
  else {
    int item = 0;

    while (1) {
        printf("Producteur %d: début\n", getpid());
      // Attendre que le buffer ne soit pas plein
      while (shm->count >= BUFFER_SIZE){}

        // Ajouter un nouvel item au buffer
        printf("Producteur %d : Ajout d'item %d au buffer\n", getpid(), item);
        shm->buffer[shm->tail] = item;
        shm->tail = (shm->tail + 1) % BUFFER_SIZE;
        shm->count++;
        item++;
        // Endormir le processus pour simuler un temps de traitement
        sleep(rand() % 5);
        
        printf("Producteur %d: fin\n", getpid());

        if(item>=max_data){exit(1);}
      
    }
    wait(NULL);
  }
    return 0;

}
*/

