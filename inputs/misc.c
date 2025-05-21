int printf(const char *, ...);

// test for CSE
int main(void) {
  int x = 5;
  int y = 8;
  int z = x * y + 2;
  int w = x * y + 2;
  printf("%d %d\n", z, w);
  return 0;
}
