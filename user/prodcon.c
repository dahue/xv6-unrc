#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "syscall.h"

#define PRODUCERS 5
#define CONSUMERS 5
#define BUFF_SIZE 50
#define UPPER 1000000
#define MAX_IT 5
#define FILE "buff_file"

int buffer;
int semprod;
int semcom;
int sembuff;

uint
random(void)
{
  // Take from http://stackoverflow.com/questions/1167253/implementation-of-rand
  static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
  unsigned int b;
  b  = ((z1 << 6) ^ z1) >> 13;
  z1 = ((z1 & 4294967294U) << 18) ^ b;
  b  = ((z2 << 2) ^ z2) >> 27;
  z2 = ((z2 & 4294967288U) << 2) ^ b;
  b  = ((z3 << 13) ^ z3) >> 21;
  z3 = ((z3 & 4294967280U) << 7) ^ b;
  b  = ((z4 << 3) ^ z4) >> 12;
  z4 = ((z4 & 4294967168U) << 13) ^ b;

  return (z1 ^ z2 ^ z3 ^ z4) / 2;
}

void
initbuffer(int init_val)
{
  // create buffer file
  buffer = open(FILE, O_CREATE|O_RDWR);
  if(buffer < 0)
    printf(1,"cant create buffer file\n");

  // write init value
  int size = write(buffer, &init_val, sizeof(init_val));
  if(size != sizeof(init_val))
    printf(1,"cant init buffer file\n");

  // release buffer
  close(buffer);
}

void
readbuffer(int *val)
{
  // open buffer
  buffer = open(FILE,O_RDONLY);

  // read value  
  int size = read(buffer, val, sizeof(&val));
  if(size != sizeof(&val))
    printf(1,"cant read buffer file\n");
  
  // release buffer
  close(buffer);
}

void
writebuffer(int *val)
{
  // open buffer
  buffer = open(FILE,O_WRONLY);

  // write value
  int size = write(buffer, val, sizeof(&val));
  if(size != sizeof(&val))
    printf(1,"cant write buffer file\n");

  // release buffer
  close(buffer);
}

void
releasebuffer()
{
  // eliminar archivo
}

void
produce()
{
  printf(1,">> Start Producer\n");
  int i, aux;
  for(i = 0; i < MAX_IT * CONSUMERS; i++){
    semdown(semprod); // empty
    semdown(sembuff); // mutex
    printf(1,"producer obtiene\n");
    readbuffer(&aux);
    aux++;
    writebuffer(&aux);
    readbuffer(&aux);
    printf(1,">> buffer after produce: %d\n",aux);
    printf(1,"producer libera\n");
    semup(sembuff); //mutex
    semup(semcom); // full
    printf(1,">> Termina producer\n");
  }
}

void
consume()
{
  printf(1,">> Start Consumer\n");
  int i,aux, w, j;
  for(i = 0; i < MAX_IT * PRODUCERS; i++){
    printf(1,"consumer obtiene\n");
    semdown(semcom);
    semdown(sembuff);
    readbuffer(&aux);
    aux--;

    w = (random() % UPPER) + 1;
    printf(1,"## waiting %d\n",w);
    j = 0;
    while (j < w)
      j++;
    writebuffer(&aux);
    readbuffer(&aux);
    printf(1,"<< buffer after consume: %d\n",aux);
    printf(1,"consumer libera\n");
    semup(sembuff);
    semup(semprod);
    printf(1,">> Termina consumer\n");
  }
}

// print process list running in the system 
// calling system procstat
int
main(void)
{
  int pid_prod, pid_com, i;

  printf(1,"Number of Producers: %d\n", PRODUCERS);
  printf(1,"Total messages sent by each producer: %d\n", MAX_IT * CONSUMERS);
  printf(1,"Number of Consumers: %d\n", CONSUMERS);
  printf(1,"Total messages received by each consumer: %d\n", MAX_IT * PRODUCERS);
  printf(1,"Buffer size: %d\n", BUFF_SIZE);
  // init buffer file
  initbuffer(0);

  // create producer semaphore
  semprod = semget(0,BUFF_SIZE); // empty
  if(semprod < 0){
    printf(1,"invalid semprod \n");
    exit();
  }

  // create consumer semaphore
  semcom = semget(1,0); // full
  if(semcom < 0){
    printf(1,"invalid semcom\n");
    exit();
  }

  // create buffer semaphore
  sembuff = semget(2,1); // mutex
  if(sembuff < 0){
    printf(1,"invalid sembuff\n");
    exit();
  }

  for (i = 0; i < PRODUCERS; i++) {
    // create producer process
    pid_prod = fork();
    if(pid_prod < 0){
      printf(1,"can't create producer process\n");
      exit(); 
    }
    // launch producer process
    if(pid_prod == 0){
      semget(semprod,0);
      semget(semcom,0);
      semget(sembuff,0);
      produce();
      exit();
    }
  }

  for (i = 0; i < CONSUMERS; i++) {
    // create consumer process
    pid_com = fork();
    if(pid_com < 0){
      printf(1,"can't create consumer process\n");
      exit(); 
    }
    // launch consumer process
    if(pid_com == 0){
      semget(semprod,0);
      semget(semcom,0);
      semget(sembuff,0);
      consume();
      exit();
    }
  }

  for (i = 0; i < PRODUCERS + CONSUMERS; i++) {
    wait();
  }

  releasebuffer();
  exit();
}
