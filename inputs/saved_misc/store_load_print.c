int printf(const char *, ...);

// mem2reg should optimize IR from
/*
define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 2, ptr %x, align 4
  %y = alloca i32, align 4
  store i32 2, ptr %y, align 4
  %0 = load i32, ptr %x, align 4
  %1 = load i32, ptr %y, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @0, i32 %0, i32 %1)
  ret i32 0
}
 */
// to
/*
define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, i32 2, i32 2)
  ret i32 0
}
*/

int main(void) {
  int x = 1 * 2 + 3 / 4;
  int y = 1 * 2 + 3 / 4;
  printf("%d %d\n", x, y);
  return 0;
}
