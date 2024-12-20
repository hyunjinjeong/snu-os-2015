#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h"

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

/* made by Taeho - this struct represents user program's state.
	therefore, it should have the elements : list-elem for being managed by child_list in parent thread
																					 tid_t for identifying its parent
																					 enum child_status for showing its state
																					 int exit_status for showing its exit state */



/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */



struct file_elem {
	int fd; 								/* file descriptor */
	unsigned pos;						/* position */
	struct file *file;			/* file pointer */
	struct dir *dir;				/* directory pointer */
	struct list_elem elem;	/* list element */
};


struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    int old_priority;                   /* Old priority. */
    int set_priority;                   /* When thread_set_priority called, If there's donated lock of current thread, then the thread has to wait until lock_release. */
                                        /* This variable is for remembering set-value. */
    struct list_elem allelem;           /* List element for all threads list. */
		
	int64_t wait_start;
	int64_t wait_length;
	bool wait_flag;	


    struct lock *wait_lock;             /* Lock that the thread waits to acquire */
    struct list lock_list;              /* List of donated locks by other thread */
    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. && userprog/syscall.c */
    uint32_t *pagedir;                  /* Page directory. */
		int exit_status;										/* exit_status */
		struct semaphore sema_wait;				/* Semaphore for wait */
		struct semaphore sema_exit;				/* Smeaphore for exit */
		struct list child_list;								/* List of children of thread */
		struct list_elem child_elem;				/* List element for 'children' list */
		struct thread *parent;							/* Parent of thread */
		struct list open_files;							/* List of opened files of thread */
		struct file *open_file;							/* Opened flle of thread */
#endif

		struct dir *dir;										/* current directory */

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

#ifdef USERPROG
struct thread *find_thread_tid(struct thread *, tid_t);
void process_create(struct thread *);
void set_parent_init_thread(void);
#endif

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);
void thread_sleep (int64_t, int64_t);
bool is_less_time (const struct list_elem*, const struct list_elem*, void *aux);
bool is_higher_priority (const struct list_elem *, const struct list_elem *, void *aux);
void wake_thread (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

#endif /* threads/thread.h */
