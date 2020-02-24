BEGIN {
  FS = "|"
  count[""] = 0
}
/enter/ {
  count[$2]++
}
END {
  for (i in count) {
    if (i != "") {
      print count[i], i
    }
  }
}
