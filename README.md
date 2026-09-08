# 2402MC06_OSlab_assignment5


To build and execute the programs, first replace all the modified files(I've included only the modified files here) in the xv6 source tree, then run make qemu, and then in the terminal run the program name as a command.

NOTE: These files are compatible with the MacOS port of xv6 by iitb, and the compiler has been changed from 1386-gcc to 1686-gcc for convenience, so these implementations might not work on the x86 version of xv6.



Q1.
Used a custom systemcall shmget() for shared memory which allocates a page and maps it into the processors page table and returns a pointer to it, and made sure the child process inherits the shared memory address from the parent when fork is used.
For the implementation of Peterson's algo, a parent process forks a child process and each of them increment a shared counter alternatively and then busy waits(sleep) when flag and turn variable of the other process is set for 10 iterations. This creates mutual exclusion by ensuring both of them cannot enter the critical section and the process that modifies the turn variable loses control and enters into a busy wait cycle.



Q2.
Used the same implementation as above for the shared memory. Implemented semaphores in the kernel level(using sleep and wakeup calls), to make use of the xv6 spinlocks and defined three syscalls init, wait, and post to handle the semaphores. If the semaphore value dips below zero when a process calls wait, it is put to sleep, and when the process running releases the semaphore, i.e, increases the semaphore(post), it wakes up the process put to sleep.
The producer consumer problem is handled by using a circular shared buffer queue, and three semaphores, 2 counting semaphores full initialised to 0; and empty initialised to the size of the buffer. The producer decrements the empty semaphore before writing and increments the full semaphore after writing. The consumer decrements full before reading and empty after reading.(Full and Empty keep track of the filled and empty positions in the buffer).



Q3.
Use the shared memory and the semaphore implementation used above.
To address the readers writers problem, we need:
1. multiple readers can read simultaneously
2. only one writer can modify at a time
3. readers cannot read while data is being modified
To ensure this, we use a read count(keeps track of readers, as the read count becomes zero, a writer can acquire the mutex for the data and no other reader or writer can access the data.
But this leads to reader priority/wirter starvation; which means when writer is waiting for a reader to finish, other readers can also access the data leading the writer to never get the chance.
To address this problem, we use a gate/turnstile mutex which keeps track of who came first in the queue and only that process can enter the critical section next; acting like a queue of sorts.



Q4.
Use the shared memory and semaphore implementation above.
In the dining philosophers problem the philosophers need both forks kept aside them to start eating, but if each of them take the fork on their right at the same time then nobody would have forks on their left and nobody can start eating, i.e, deadlock. To address this, asymmetric pickup is used where one of the philosophers picks up the left fork first, leading to him eating first and then everybody else after that at some point.
The implementation is done by mutexes as forks, and a parent process forks 5 child processes simulating philosophers, and the last numbered philosopher picks up the mutex in a different order
