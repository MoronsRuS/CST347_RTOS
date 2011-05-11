#ifndef	RWLOCKOBJECT_H
#define	RWLOCKOBJECT_H

#include "../SROS/source/rtos.h"

#define	ENTRY_WAIT_TIME		-1
#define	EXIT_WAIT_TIME		-1
#define	WRITE_WAIT_TIME		-1

typedef struct rwLockObject {
	mutexObject_t	entryLock;
	mutexObject_t	exitLock;
	mutexObject_t	writeLock;
	
	int32		count;
} rwLockObject_t;

void	rwLockObjectInit(rwLockObject_t* lock);
void	rwLockObjectLockReader(rwLockObject_t* lock);
void	rwLockObjectLockWriter(rwLockObject_t* lock);
void	rwLockObjectRelease(rwLockObject_t* lock);

#endif
