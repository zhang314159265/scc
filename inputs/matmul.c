int printf(const char *, ...);

int main(void) {
  // fibnacii numbers
  // 0, 1, 1, 2, 3, 5, 8, 13, 21, 34
  /*
   * [[ Fn+1,   Fn ]   *      [[1, 1,]  =  [[Fn+2,   Fn+1],
   *  [ Fn,     Fn-1 ]]       [1, 0]]       [Fn+1,   Fn]]
   *
   * Extend:
   * [[ Fn+1, Fn ]   *   [[Fm+1, Fm,]  =    [[Fn+m+1,   Fn+m],
   *  [ Fn,   Fn-1 ]]     [Fm,   Fm-1]]      [Fn+m,     Fn+m-1]]
   */
  double mat1[2][2];
  double mat2[2][2];
  double matres[2][2];
  int i, j, k;

  // n = 2
  mat1[0][0] = 2;
  mat1[0][1] = 1;
  mat1[1][0] = 1;
  mat1[1][1] = 1;

  // m = 3
  mat2[0][0] = 3;
  mat2[0][1] = 2;
  mat2[1][0] = 2;
  mat2[1][1] = 1;

  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 2; ++j) {
      matres[i][j] = 0;
      for (k = 0; k < 2; ++k) {
        matres[i][j] += mat1[i][k] * mat2[k][j];
      }
    }
  }
  /*
   * expected result (m+n = 5)
   * - 8, 5
   * - 5, 3
   */
  printf("[[%.0f, %.0f], [%.0f, %.0f]]\n",
    matres[0][0],
    matres[0][1],
    matres[1][0],
    matres[1][1]);
  return 0;
}
