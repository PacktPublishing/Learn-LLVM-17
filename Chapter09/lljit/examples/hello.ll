declare i32 @printf(ptr, ...)

@hellostr = private unnamed_addr constant [13 x i8] c"Hello world\0A\00"

define dso_local i32 @main(i32 %argc, ptr %argv) {
  %res = call i32 (ptr, ...) @printf(ptr @hellostr)
  ret i32 0
}
