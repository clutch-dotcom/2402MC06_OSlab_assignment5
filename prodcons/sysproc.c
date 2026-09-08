#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

#define MAXSEMS 10

struct semaphore {
  int value;
  int active;
  struct spinlock lock;
};

struct semaphore sems[MAXSEMS];
struct spinlock sems_lock;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


//my syscalls

extern void* get_shared_page(struct proc*);

int sys_shmget(void) {
  return (int)get_shared_page(myproc());
}



int sys_sem_init(void) {
    int init_val;
    if (argint(0, &init_val) < 0)
        return -1;

    acquire(&sems_lock);
    for (int i = 0; i < MAXSEMS; i++) {
        if (sems[i].active == 0) {
            sems[i].active = 1;
            sems[i].value = init_val;
            initlock(&sems[i].lock, "semaphore");
            release(&sems_lock);
            return i; // Return the semaphore ID
        }
    }
    release(&sems_lock);
    return -1; // No free semaphores
}

int sys_sem_wait(void) {
    int sem_id;
    if (argint(0, &sem_id) < 0 || sem_id < 0 || sem_id >= MAXSEMS)
        return -1;

    struct semaphore *s = &sems[sem_id];

    acquire(&s->lock);
    // Use a while loop to handle spurious wakeups
    while (s->value <= 0) {
        // sleep() releases the spinlock and puts the process to sleep.
        // It re-acquires the lock before returning.
        sleep(s, &s->lock); 
    }
    s->value--;
    release(&s->lock);

    return 0;
}

int sys_sem_post(void) {
    int sem_id;
    if (argint(0, &sem_id) < 0 || sem_id < 0 || sem_id >= MAXSEMS)
        return -1;

    struct semaphore *s = &sems[sem_id];

    acquire(&s->lock);
    s->value++;
    // Wake up any processes waiting on this specific semaphore address
    wakeup(s); 
    release(&s->lock);

    return 0;
}
