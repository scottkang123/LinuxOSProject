#define _GNU_SOURCE
#include "simple_mutex.h"
#include <linux/futex.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

void mutex_lock(int *mutex)
{
    int v;
    //Bit 31 was clear, lock the mutex
    if(__atomic_fetch_or(mutex, 0x80000000, __ATOMIC_SEQ_CST) >= 0){
        return;
    }
    //Add 1 to mutex as it is already locked (show that a thread is waiting)
    __atomic_fetch_add(mutex, 1, __ATOMIC_SEQ_CST);
    while(1){
        //if mutex was unlocked state, lock it and 
        //subtract 1 to show a decrease in waiting threads
        if(__atomic_fetch_or(mutex, 0x80000000, __ATOMIC_SEQ_CST) >= 0){
            __atomic_fetch_sub(mutex, 1, __ATOMIC_SEQ_CST);
            return;
        }
        v = *mutex;
        if(v >= 0)
            continue;
        //wait for the thread to finish (until v changes) since it is locked
        syscall(__NR_futex, &mutex, FUTEX_WAIT, v, 0, 0, 0);
    } 
}

void mutex_unlock(int *mutex)
{
    //Add 0x80000000 to mutex to see if it is 0. If 0, then there are 
    //no other interested threads
    if(__atomic_add_fetch(mutex, 0x80000000, __ATOMIC_SEQ_CST) == 0){
        return;
    }
    //There are other threads waiting for this mutex, wake one of them
    syscall(__NR_futex, &mutex, FUTEX_WAKE, 1, 0, 0, 0);
}

//TRY FOR EXTRA CREDIT - UNABLE TO FINISH IMPLEMENTING/TESTING
/*
#include <stdbool.h>

typedef struct mcs_spinlock {
    mcs_qnode* tail;
} mcs_spinlock;

typedef struct mcs_qnode {
    mcs_qnode* next;
    bool locked;
} mcs_qnode;

void mcs_spinlock_lock(mcs_spinlock *lock, mcs_qnode *node)
{
    node->locked = true;
    node->next = NULL;  // initially no successor
    mcs_qnode* prev = __atomic_exchange_n(&lock->tail, node, __ATOMIC_SEQ_CST); // Queue for lock
    if (prev != NULL) {
        node->locked = true; // Prepare to spin
        prev->next = node; // Link behind predecessor
        while (node->locked) {
            syscall(__NR_futex, &node->locked, FUTEX_WAIT_PRIVATE, 1, 0, 0, 0);
        }
    }
}

void mcs_spinlock_unlock(mcs_spinlock *lock, mcs_qnode *node)
{
    if (node->next == NULL) {
        if (__atomic_compare_exchange_n(&lock->tail, &node, NULL, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            return;
        }

        while (node->next == NULL) {
            continue;
        }
    }
    node->next->locked = false;
    syscall(__NR_futex, &node->next->locked, FUTEX_WAKE_PRIVATE, 1, 0, 0, 0);
}
*/

