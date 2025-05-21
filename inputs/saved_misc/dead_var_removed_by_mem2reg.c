int printf(const char *, ...);

// test for DCE
// mem2reg pass will remove the dead variable y
int main(void) {
  int x = 3;
  int y = 5;
  printf("%d\n", x);
  return 0;
}
