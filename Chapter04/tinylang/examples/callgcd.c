#include <stdio.h>

extern long _t3Gcd3GCD(long, long);

int main(int argc, char *argv[]) {
  printf("gcd(25, 20) = %ld\n", _t3Gcd3GCD(25, 20));
  printf("gcd(3, 5) = %ld\n", _t3Gcd3GCD(3, 5));
  printf("gcd(21, 28) = %ld\n", _t3Gcd3GCD(21, 28));
  return 0;
}
