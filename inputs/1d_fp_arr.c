int printf(const char *, ...);
double sqrt(double);

int main(void) {
  float ar[10];
  int i;
  int picked = 8;

  for (i = 0; i < 10; ++i) {
    ar[i] = sqrt(i);
  }
  printf("sqrt of %d is %.2f\n", picked, ar[picked]); // 2.83
  return 0;
}
