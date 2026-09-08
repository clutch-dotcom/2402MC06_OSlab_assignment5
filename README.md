# 2402MC06_OSlab_assignment5


To build and execute the programs, first replace all the modified files(I've included only the modified files here) in the xv6 source tree, then run make qemu, and then in the terminal run the program name as a command.

NOTE: These files are compatible with the MacOS port of xv6 by iitb, and the compiler has been changed from 1386-gcc to 1686-gcc for convenience, so these implementations might not work on the x86 version of xv6.



Q1.
Used a custom systemcall shmget() for shared memory which allocates a page and maps it into the processors page table and returns a pointer to it, and made sure the child process inherits the shared memory address from the parent when fork is used.
For the implementation of Peterson's algo, a parent process forks a child process and each of them increment a shared counter alternatively and then busy waits(sleep) when flag and turn variable of the other process is set for 10 iterations. This creates mutual exclusion by ensuring both of them cannot enter the critical section and the process that modifies the turn variable loses control and enters into a busy wait cycle.



Q2.
Used the same implementation as above for the shared memory. Implemented semaphores in the kernel level(using sleep and wakeup calls), to make use of the xv6 spinlocks and defined three syscalls init, wait, and post to handle the semaphores. If the semaphore value dips below zero when a process calls wait, it is put to sleep, and when the process running releases the semaphore, i.e, increases the semaphore(post), it wakes up the process put to sleep.
