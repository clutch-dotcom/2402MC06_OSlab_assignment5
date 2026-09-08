#include "types.h"
#include "stat.h"
#include "user.h"

#define N 5

struct shm {
    int dinsem[N];
    int p_mutex;
};

void philosopher(int i, struct shm *shm) {
    for (int cycle = 0; cycle < 3; cycle++) {              //simulate 3 cycles instead of infinite
        sem_wait(shm->p_mutex);
        printf(1, "Philosopher %d: THINKING\n", i+1);
        sem_post(shm->p_mutex);
        sleep(2);

        sem_wait(shm->p_mutex);
        printf(1, "Philosopher %d: HUNGRY\n", i+1);
        sem_post(shm->p_mutex);

        //asym fork pickup
        if (i == N - 1) {
            sem_wait(shm->dinsem[(i + 1) % N]); // Right
            sleep(2);                           // so that no philosopher can pick up both forks at the same time
            sem_wait(shm->dinsem[i]);           // Left
        } else {
            sem_wait(shm->dinsem[i]);           // Left
            sleep(2);
            sem_wait(shm->dinsem[(i + 1) % N]); // Right
        }

        sem_wait(shm->p_mutex);
        printf(1, "Philosopher %d: EATING\n", i+1);
        sem_post(shm->p_mutex);
        sleep(5);

        // Put down forks
        sem_post(shm->dinsem[i]);
        sem_post(shm->dinsem[(i + 1) % N]);
    }
    sem_wait(shm->p_mutex);
    printf(1, "Philosopher %d: FINISHED\n", i+1);
    sem_post(shm->p_mutex);
}

int main() {
    // Fixed: Use shmget() instead of malloc()
    struct shm *shm = (struct shm *) shmget(); 
    
    for (int i = 0; i < N; i++) {
        shm->dinsem[i] = sem_init(1); 
    }
    shm->p_mutex = sem_init(1); // print mutex

    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            philosopher(i, shm);
            exit();
        }
    }

    for (int i = 0; i < N; i++) {
        wait();
    }

    printf(1, "All philosophers finished cleanly.\n");
    exit();
}