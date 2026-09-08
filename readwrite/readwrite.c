#include "types.h"
#include "stat.h"
#include "user.h"

struct shared_data {
    int data;
    int read_count;
};

void reader(struct shared_data *shared, int id, int mutex, int rw_mutex, int turnstile) {
    for (int i = 0; i < 2; i++) {
        sleep(id * 2); // Small delay to interleave execution
        
        // 1. Wait at the turnstile (Fairness mechanism)
        sem_wait(turnstile);
        
        // 2. Lock read_count to update it safely
        sem_wait(mutex);
        shared->read_count++;
        if (shared->read_count == 1) {
            // First reader locks the shared data from writers
            sem_wait(rw_mutex);
        }
        
        // 3. Release turnstile and read_count lock
        sem_post(turnstile);
        sem_post(mutex);
        
        // --- CRITICAL SECTION (Reading) ---
        printf(1, "[Reader %d] READING | Data: %d\n", id, shared->data);
        sleep(5); // Simulate reading time
        
        // 4. Lock read_count to decrement it safely
        sem_wait(mutex);
        shared->read_count--;
        if (shared->read_count == 0) {
            // Last reader unlocks the shared data, allowing writers in
            sem_post(rw_mutex);
        }
        sem_post(mutex);
    }
}

void writer(struct shared_data *shared, int id, int mutex, int rw_mutex, int turnstile) {
    for (int i = 0; i < 2; i++) {
        sleep(id * 5); // Delay

        // 1. Wait at the turnstile, then lock the data exclusively
        sem_wait(turnstile);
        sem_wait(rw_mutex);

        //cs
        shared->data++;
        printf(1, "[Writer %d] WRITING | Data: %d\n", id, shared->data);
        sleep(10); // Simulate writing time
        
        // 2. Release data lock and turnstile
        sem_post(turnstile);
        sem_post(rw_mutex);
    }
}

int main(int argc, char *argv[]) {

    struct shared_data *shared = (struct shared_data *) shmget();
    shared->data = 0;
    shared->read_count = 0;

   
    int mutex     = sem_init(1); // Protects read_count
    int rw_mutex  = sem_init(1); // Ensures exclusive data access for writers
    int turnstile = sem_init(1); // Prevents writer starvation

    // 3. Spawn 3 Readers
    for (int i = 1; i <= 3; i++) {
        if (fork() == 0) { 
            reader(shared, i, mutex, rw_mutex, turnstile); 
            exit(); 
        }
    }
    
    // 4. Spawn 2 Writers
    for (int i = 1; i <= 2; i++) {
        if (fork() == 0) { 
            writer(shared, i, mutex, rw_mutex, turnstile); 
            exit(); 
        }
    }

    // 5. Wait for all 5 children to complete
    for (int i = 0; i < 5; i++) {
        wait();
    }

    printf(1, "Readers and Writers finished successfully.\n");
    exit();
}