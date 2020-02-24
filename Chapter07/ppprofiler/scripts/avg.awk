BEGIN {
  FS = "|"
  count[""] = 0
  sum[""] = 0
}
{
  count[$1]++
  sum[$1] += $4
}
END {
  for (i in count) {
    if (i != "") {
      print count[i], sum[i], sum[i]/count[i], i
    }
  }
}
