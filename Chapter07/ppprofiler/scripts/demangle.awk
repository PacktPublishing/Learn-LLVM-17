{
  cmd = "llvm-cxxfilt " $4
  (cmd) | getline name
  close(cmd)
  $4 = name
  print
}
