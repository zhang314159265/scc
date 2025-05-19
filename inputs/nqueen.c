int printf(const char *, ...);

int state[64]; // assume it's enough
int ans;

int abs(int v) {
  if (v >= 0) {
    return v;
  } else {
    return -v;
  }
}

void bt(int step, int N) {
  int i, j, tmp, valid;

  if (step == N) {
    ++ans;
    return;
  }
  for (i = step; i < N; ++i) {
    // swap
    tmp = state[step];
    state[step] = state[i];
    state[i] = tmp;

    valid = 1;
    for (j = 0; j < step; ++j) {
      if (step - j == abs(state[step] - state[j])) {
        valid = 0;
        break;
      }
    }
    if (valid) {
      bt(step + 1, N);
    }

    // swap
    tmp = state[step];
    state[step] = state[i];
    state[i] = tmp;
  }
}

int nqueen(int N) {
  int i;
  for (i = 0; i < N; ++i) {
    state[i] = i;
  }
  bt(0, N);
  return ans;
}

int main(void) {
  int N = 8;
  printf("%d queen solution: %d\n", N, nqueen(N));
  return 0;
}
