#include "types.h"
#include "stat.h"
#include "user.h"

// Define shared structure
struct shm_data {
    volatile int flag[2];
    volatile int turn;
    volatile int shared_counter;
};

int main(void) {
    // 1. Parent gets the shared page
    struct shm_data *shm = (struct shm_data*) shmget();
    if ((int)shm == 0) {
        printf(1,  "shmget failed\n");
        exit();
    }

    // Initialize variables
    shm->flag[0] = 0;
    shm->flag[1] = 0;
    shm->turn = 0;
    shm->shared_counter = 0;

    int pid = fork();
    if (pid < 0) {
        printf(1, "Fork failed\n");
        exit();
    }

    // 2. Child MUST remap the same physical page into its own page directory
    if (pid == 0) {
        shm = (struct shm_data*) shmget();
    }

    int id = (pid == 0) ? 1 : 0; 
    int other = 1 - id;

    for (int k = 0; k < 10; k++) {
        // --- ENTRY SECTION ---
        shm->flag[id] = 1;
        shm->turn = other;
        
        // Memory Barrier: Force assignment to commit before while-loop evaluation
        __sync_synchronize();

        // Busy Wait
        while (shm->flag[other] == 1 && shm->turn == other) {
            sleep(1); // Spin
        }

        // --- CRITICAL SECTION ---
        int temp = shm->shared_counter;
        
        // Force a context switch. If locks fail, this causes a lost update.
        sleep(2); 
        
        shm->shared_counter = temp + 1;
        printf(1, "Process %d in CS, counter = %d\n", id, shm->shared_counter);

        // --- EXIT SECTION ---
        __sync_synchronize(); // Force CS memory writes to commit before dropping flag
        shm->flag[id] = 0;

        // --- REMAINDER SECTION ---
        sleep(1);
    }

    // Parent waits for child and prints final result
    if (pid > 0) {
        wait();
        printf(1, "Final counter value: %d (Expected: 20)\n", shm->shared_counter);
    }

    exit();
}
