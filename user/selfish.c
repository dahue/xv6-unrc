#include "types.h"
#include "user.h"

int 
main(void){
  printf(1, "I'm a selfish process...\n");
	for (;;){
		set_priority(0);
	}
	exit();
}
