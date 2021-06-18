#include "types.h"
#include "user.h"

int foo(int count, int array[]) {
  if ((count % 100000) == 0){
    printf(1, "Recursive calls: %d\n",count);
  }
  return foo(++count, array);
}

int main(void)
{
  int array[10000];
  int count = 1;
  memset(array, 999, sizeof(array));			
  printf(1, "Recursive calls: %d\n",foo(count, array));
  exit();
}