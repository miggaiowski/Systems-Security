#include <stdio.h>

void function(int a, int b, int c) {
   char buffer1[5];
   char buffer2[10];
   buffer2[0] = 0x32;
   buffer2[1] = 0x32;
   buffer2[2] = 0x32;
   buffer2[3] = 0x32;
   buffer2[4] = 0x32;
   buffer2[5] = 0x32;
   buffer2[6] = 0x32;
   buffer2[7] = 0x32;
   buffer2[8] = 0x32;
   buffer2[9] = 0x32;
   buffer1[0] = 0x33;
   buffer1[1] = 0x33;
   buffer1[2] = 0x33;
   buffer1[3] = 0x33;
   buffer1[4] = 0x33;
   int *ret;

   ret = buffer1 + 25;
   (*ret) += 7;
}

int main() {
  int x;

  x = 0;
  function(1,2,3);
  x = 1;
  printf("%d\n",x);
}
