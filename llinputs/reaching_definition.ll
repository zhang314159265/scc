; LLVM IR is in SSA form. We can not reassign %i later.
; To reproduce the reaching definition example in the dragon book 9.2.4,
; we use alloca/load/store instead

define i32 @main() {
entry:
  %m = alloca i32
  %n = alloca i32
  %u1 = alloca i32
  %u2 = alloca i32
  %u3 = alloca i32

  %i = alloca i32
  %j = alloca i32
  %a = alloca i32

  br label %b1

b1:
  %m1 = load i32, ptr %m
  %i1 = sub i32 %m1, 1
  store i32 %i1, ptr %i ; d1

  %n1 = load i32, ptr %n
  store i32 %n1, ptr %j ; d2

  %u1_1 = load i32, ptr %u1
  store i32 %u1_1, ptr %a ; d3

  br label %b2

b2:
  %i2 = load i32, ptr %i
  %i3 = add i32 %i2, 1
  store i32 %i3, ptr %i; d4

  %j1 = load i32, ptr %j
  %j2 = sub i32 %j1, 1
  store i32 %j2, ptr %j ; d5
  br i1 true, label %b3, label %b4

b3:
  %u2_1 = load i32, ptr %u2
  store i32 %u2_1, ptr %a ; d6
  br label %b4

b4:
  %u3_1 = load i32, ptr %u3
  store i32 %u3_1, ptr %i ; d7
  br i1 true, label %b2, label %exit

exit:
  ret i32 0
}
