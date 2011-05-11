#include "rwLockObject.h"
#include "assert.h"

void	rwLockObjectInit(rwLockObject_t* lock) {
	mutexObjectInit(&(lock->entryLock),1);//Initialize mutex to unlocked.
	mutexObjectInit(&(lock->exitLock),1);//Initialize mutex to unlocked.
	mutexObjectInit(&(lock->writeLock),1);//Initialize mutex to unlocked.
	lock->count = 0;//Initialize count to 0;
}
void	rwLockObjectLockReader(rwLockObject_t* lock) {
	while (! mutexObjectLock(&(lock->entryLock), ENTRY_WAIT_TIME));
	while (! mutexObjectLock(&(lock->exitLock), EXIT_WAIT_TIME));
	if (lock->count == 0) {
		while (! mutexObjectLock(&(lock->writeLock), WRITE_WAIT_TIME));
	}
	++(lock->count);
	mutexObjectRelease(&(lock->exitLock));
	mutexObjectRelease(&(lock->entryLock));
}
void	rwLockObjectLockWriter(rwLockObject_t* lock) {
	while (! mutexObjectLock(&(lock->entryLock), ENTRY_WAIT_TIME));
	while (! mutexObjectLock(&(lock->writeLock), WRITE_WAIT_TIME));
	assert(lock->count == 0);//There should be no readers if we made it this far.
}
void	rwLockObjectRelease(rwLockObject_t* lock) {
	while (! mutexObjectLock(&(lock->exitLock), EXIT_WAIT_TIME));
	if (lock->count == 0) {
		mutexObjectRelease(&(lock->exitLock));
		mutexObjectRelease(&(lock->writeLock));
		mutexObjectRelease(&(lock->entryLock));
	} else {
		--(lock->count);
		if (lock->count == 0) {
			mutexObjectRelease(&(lock->writeLock));
		}
		mutexObjectRelease(&(lock->exitLock));
	}
}

