int printf(const char *, ...);

int main(void) {
  int sum = 0;
  int i;
  
  i = 0;
  for (; i <= 100; ++i) {
    sum += i;
  }
  printf("Sum is %d\n", sum);
  return 0;
}
