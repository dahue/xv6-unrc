#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sem.h"

struct {
  struct spinlock lock;
  struct sem sem[NSEM];
} stable;

struct sem* *checkosem(){
	struct sem* *r;
	for (r = myproc()->osem; r < myproc()->osem + NOSEM; r++) {
		if (*r == 0)
			return r;
	}
	return 0;
}

struct sem* getstable(){
	return stable.sem;
}

int semget(int key, int init_value){
	struct sem *s;
	struct sem **r;
	static int first_time = 1;

	if (first_time) {
		initlock(&stable.lock, "stable");
		first_time = 0;
	}

	acquire(&stable.lock);

  s = stable.sem + key;
  if (s->refcount == 0){
    r = checkosem();
    if (r != 0){
      s->value = init_value;
      s->refcount = 1;
      *r = s;
      release(&stable.lock);
      return s - stable.sem;
    }
    release(&stable.lock);
    return -1;
	
  }else if (*(r = checkosem()) == 0){
    *r = s;
    s->refcount++;
    release(&stable.lock);
    return key;
  }	else {
    release(&stable.lock);
    return -1;
  }
}


int semclose(int semid){
	struct sem *s;
	struct sem **r;

	s = stable.sem + semid;
	if (s->refcount == 0)
		return -1;

	for (r = myproc()->osem; r < myproc()->osem + NOSEM; r++) {
		if (*r == s) {
			*r = 0;
			acquire(&stable.lock);
			s->refcount--;
			release(&stable.lock);
			return 0;
		}
	}
	return -1;
}

int semdown(int semid){
	struct sem *s;
	s = stable.sem + semid;
	acquire(&stable.lock);
	if (s->refcount <= 0) {
		release(&stable.lock);
		return -1;
	}
	while (s->value == 0)
		sleep(s, &stable.lock);

	s->value--;
	release(&stable.lock);
	return 0;
}

int semup(int semid){
	struct sem *s;
	s = stable.sem + semid;
	acquire(&stable.lock);
	if (s->refcount <= 0) {
		release(&stable.lock);
		return -1;
	}
	if (s->value >= 0) {
		if (s->value == 0){
			s->value++;
			wakeup(s);
		}else
			s->value++;
		release(&stable.lock);
	}
	return 0;
}
