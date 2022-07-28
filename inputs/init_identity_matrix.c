{
  int i;
  int j;
  double a[10][10];

  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 10; ++j) {
      a[i][j] = 0.0;
    }
  }

  for (i = 0; i < 10; ++i) {
    a[i][i] = 1.0;
  }
}
