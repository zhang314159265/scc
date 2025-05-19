int printf(const char *, ...);

int main(void) {
  float mat[10][10];
  int i, j;
  float sum = 0;

  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 10; ++j) {
      mat[i][j] = 0;
    }
  }

  for (i = 0; i < 10; ++i) {
    mat[i][i] = 1;
  }
  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 10; ++j) {
      sum += mat[i][j];
    }
  }
  printf("sum is %f\n", sum);
  return 0;
}
