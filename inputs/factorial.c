int printf(const char *, ...);

int fact(int num) {
  if (num <= 1) {
    return 1;
  } else {
    return num * fact(num - 1);
  }

  return 0; // TODO let scc work without this hack
}

int main(void) {
  printf("%d\n", fact(4));
  return 0;
}
