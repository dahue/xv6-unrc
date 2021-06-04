struct sem {
  int value;
  int refcount;
};

int semget(int key, int init_value);
int semclose(int semid);
int semdown(int semid);
int semup(int semid);

struct sem* getstable();
