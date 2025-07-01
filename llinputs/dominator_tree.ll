; This file reproduces the dominator tree example in the dragon book 9.6.1

define i32 @main() {
b1:
  br i1 true, label %b2, label %b3    

b2:
  br label %b3

b3:
  br label %b4

b4:
  switch i32 0, label %b3 [
    i32 1, label %b5
    i32 2, label %b6
  ]

b5:
  br label %b7

b6:
  br label %b7

b7:
  br i1 true, label %b4, label %b8

b8:
  switch i32 0, label %b3 [
    i32 1, label %b9
    i32 2, label %b10
  ]

b9:
  br label %b1

b10:
  br label %b7

}
