#include "types.h"
#include "user.h"

unsigned int fib(unsigned int n){
	if (n < 2) return 1;
	else return fib(n-1) + fib(n-2);
}

int 
main(void){
	unsigned int n = 0;
	for (;;){
		set_priority(3);
		printf(1, "Fibonacci de %d = %d\n", n, fib(n));
		n++;
	}
	exit();
}
