#ifndef _POSIX_THREAD_API_H_
#define _POSIX_THREAD_API_H_

#ifdef WIN32
#include <Windows.h>
#include <process.h>

typedef struct {
    time_t tv_sec;       
    long int tv_nsec;      
}timespec;

typedef struct{
	HANDLE hThread;
	unsigned int  dwThId;
	typedef	void*(*start_routine)(void*arg);
	start_routine posix_thread_proc;
    void* arg;
    void* ret;	
}pthread_t;

unsigned __stdcall win32_thread_proc(void* p)
{
	pthread_t* pth = (pthread_t*)p;
	void* ret = pth->posix_thread_proc(pth->arg);
	pth->ret = ret;
	return 0;
}

#define pthread_create(tht,attr,routine,args)\
									  (*tht).posix_thread_proc = routine;\
									  (*tht).arg = args;\
									  (*tht).hThread=(HANDLE)_beginthreadex(NULL,0,win32_thread_proc, tht,0,&((*tht).dwThId))
#define pthread_exit(ret)             return(ret)

//互斥锁
#define pthread_mutex_t               CRITICAL_SECTION
#define pthread_mutex_init(mtx, attr) InitializeCriticalSection(mtx)
#define pthread_mutex_destroy(mtx)    DeleteCriticalSection(mtx)
#define pthread_mutex_lock(mtx)       EnterCriticalSection(mtx)
#define pthread_mutex_unlock(mtx)     LeaveCriticalSection(mtx)
#define pthread_mutex_trylock(mtx)    (!TryEnterCriticalSection(mtx))

//条件变量
#define pthread_cond_t                CONDITION_VARIABLE
#define pthread_cond_init(cond,attr)  InitializeConditionVariable(cond)
#define pthread_cond_destroy
#define pthread_cond_wait(cond, mtx)  SleepConditionVariableCS(cond, mtx, INFINITE)
#define pthread_cond_timedwait(cond,mtx, ts)\
                                      SleepConditionVariableCS(cond, mtx, (ts->tv_sec*1000+ts->tv_nsec/1000))
#define pthread_cond_signal(cond)     WakeConditionVariable(cond)
#define pthread_cond_broadcast(cond)  WakeAllConditionVariable(cond)

//信号量#include <semaphore.h>
#define sem_t                         HANDLE
#define sem_init(sem, shared, init)   !((*sem)=CreateSemaphore(NULL, init, SEM_VALUE_MAX, NULL))
#define sem_destroy(sem)              CloseHandle(*sem)
#define sem_wait(sem)                 WaitForSingleObject(*sem, INFINITE)
#define sem_trywait(sem)              WaitForSingleObject(*sem, 0)
#define sem_timedwait(sem,ts)         WaitForSingleObject(*sem, (ts->tv_sec*1000+ts->tv_nsec/1000))
#define sem_post(sem)                 ReleaseSemaphore(*sem, 1, NULL)

//读写锁
#define pthread_rwlock_t              SRWLOCK
#define pthread_rwlock_init(rwl)      InitializeSRWLock(rwl)
#define pthread_rwlock_destroy(rwl)       
#define pthread_rwlock_rdlock(rwl)    AcquireSRWLockShared(rwl)
#define pthread_rwlock_wrlock(rwl)    AcquireSRWLockExclusive(rwl)
#define pthread_rwlock_unlock(rwl)    (TryAcquireSRWLockShared(rwl)?ReleaseSRWLockShared(rwl):ReleaseSRWLockExclusive(rwl))

//自旋锁
#define pthread_spinlock_t            CRITICAL_SECTION
#define pthread_spin_init(lock, shared) InitializeCriticalSectionAndSpinCount(lock, shared)
#define pthread_spin_destroy(lock)    DeleteCriticalSection(lock)
#define pthread_spin_lock(lock)       EnterCriticalSection(lock)
#define pthread_spin_trylock(lock)    (!TryEnterCriticalSection(lock))
#define pthread_spin_unlock(lock)     LeaveCriticalSection(lock)

#endif

#endif//_POSIX_THREAD_API_H_
