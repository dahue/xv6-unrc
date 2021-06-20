#include "types.h"
#include "user.h"

#define LIMIT 1000
int
main(void)
{	

    int semid, i=0;

    /*checking for non process sema access*/
    int chk = semup(20);
    if(chk<0)
        printf(1,"Testing non adquired semaphore (up): OK!\n");
    else
        printf(1,"Testing non adquired semaphore (up): FAILED.\n");
    
    chk = semdown(20);
    if(chk<0)
        printf(1,"Testing non adquired semaphore (down): OK!\n");
    else
        printf(1,"Testing non adquired semaphore (down): FAILED.\n");

    /*checking for close non requested semaphore*/
    chk=semclose(8);
    if(chk<0)
        printf(1,"Testing semclose: OK!.\n");
    else
        printf(1,"Testing semclose: FAILED!.\n");
    
    /*checking for getting all the resources*/
    int countsem=0,it;
    chk=0;
    for(it=0 ; it<LIMIT && chk>=0; it++,countsem++){
        chk = semget(countsem,4);   
        printf(1,"semget: %d\n",chk);
             
    }    
    printf(1,"Testing get all semaphores availables (%d availables?).\n",countsem-1);
    
    /*checking for close: sem 0 was adquired*/
    chk=semclose(0);
    if(chk==0)
        printf(1,"Testing semclose sem 0: OK!.\n");
    else
        printf(1,"Testing semclose sem 0: FAILED!.\n");

    /*checking for close: but not twice!*/
    chk=semclose(0);
    if(chk<0)
        printf(1,"Testing semclose sem 0 (second time): OK!.\n");
    else
        printf(1,"Testing semclose sem 0 (second time): FAILED!.\n");

   /*cheching n-ary semaphore*/
   semid=semget(-1,2);
   if(fork()==0){
     semget(-1,-1);        

     for(i=0;i<10;i++){
       printf(1,"1st child trying... \n");
       semdown(semid);
       printf(1,"1st child in critical region \n");
       semup(semid);
     }
     exit();
   }
   /*second child*/
   if(fork()==0){
     semget(semid,-1);

     for(i=0;i<10;i++){
       printf(1,"2nd child trying... \n");
       semdown(semid);
       printf(1,"2nd child in critical region \n");
       semup(semid);
     }   
     exit();
   }
   /*Parent code*/
   for(i=0;i<10;i++){
     printf(1,"Parent trying... \n");
     semdown(semid);
     printf(1,"Parent in critical region \n");
     semup(semid);
   }
   wait();
   wait();
   exit();

}
