#include "types.h"
#include "stat.h"
#include "user.h"

struct shared_data {
    int buffer[50]; 
    int head;
    int tail;
};

int main(int argc, char *argv[]) {
    int buffer_size = (argc > 1) ? atoi(argv[1]) : 5;
    
    // 1. Get shared memory and initialize
    struct shared_data *shared = (struct shared_data *) shmget();
    shared->head = 0;
    shared->tail = 0;

    // 2. Initialize kernel semaphores
    int empty = sem_init(buffer_size); 
    int full  = sem_init(0);            
    int mutex = sem_init(1);           

    int pid = fork();

    if (pid == 0) {
        // CONSUMER
        for (int i = 1; i <= 20; i++) {
            sem_wait(full);   
            sem_wait(mutex);  

            int item = shared->buffer[shared->tail];
            shared->tail = (shared->tail + 1) % buffer_size;
            printf(1, "[Consumer] Consumed: %d\n", item);

            sem_post(mutex);  
            sem_post(empty);  
            sleep(20); 
        }
        exit();
    } else {
        // PRODUCER
        for (int i = 1; i <= 20; i++) {
            sem_wait(empty);  
            sem_wait(mutex);  

            shared->buffer[shared->head] = i;
            shared->head = (shared->head + 1) % buffer_size;
            printf(1, "[Producer] Inserted: %d\n", i);

            sem_post(mutex);  
            sem_post(full);   
            sleep(5); 
        }
        wait();
        printf(1, "Producer-Consumer finished successfully.\n");
        exit();
    }
    return 0;
}