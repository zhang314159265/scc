define i32 @main() {
entry:
  switch i32 0, label %b3 [
    i32 1, label %b1
    i32 2, label %b2
  ]

b1:
  br label %exit
b2:
  br label %exit
b3:
  br label %exit
exit:
  ret i32 0 
}
