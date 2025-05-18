int printf(const char *, ...);

int main(void) {
  int a = 1, b = 2, c = 3, d;
  if (a > 0) {
    d = b + c;
  } else {
    d = b - c;
  }
  printf("d is %d\n", d);
  return 0;
}
