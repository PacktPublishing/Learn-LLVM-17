BEGIN {
  FS = "|"; OFS = "|"
}
/enter/ {
  record[$2] = $0
}
/exit/ {
  split(record[$2],val,"|")
  print val[2], val[3], $3, $3-val[3], val[4]
}
