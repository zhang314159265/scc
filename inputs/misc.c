int printf(const char *, ...);

int main(void) {
  int i, j, a;

  i = 10 - 1; // m==10
  j = 20; // n == 20
  a = 30; // u1 == 30

  for (; 0 == 1;) {
    i = i + 1;
    j = j - 1;
    if (1 == 1) {
      a = 40; // u2 = 40
    }
    i = 50; // u3 == 50
  }
  printf("bye\n");
  return 0;
}
