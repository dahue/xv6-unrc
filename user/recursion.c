#include "types.h"
#include "user.h"

int foo(int count) {
  int arr[10];
  memset(arr, 0, sizeof(arr));
  if (count == 0){
    arr[0] = 0;
  }
  else{
    arr[0] = foo(count - 1) + 1;
  }
  return arr[0];
}

int main(void)
{
  int array[1024];
  memset(array, 0, sizeof(array));

  // array[0] has the number of recursive calls.
  // it works for 200 calls, it fails for 300.
  array[0] = 200;
  printf(1, "Recursive calls: %d\n", foo(array[0]));
  exit();
}
