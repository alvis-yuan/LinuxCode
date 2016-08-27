#!/bin/bash



function proc()
{
  str=''
  i=0
  arr=("|" "\\" "-" "/")
  while [ $i -le 100 ]
  do
    let index=i%4
    printf "[%-100s][%d%%][%c]\r" "$str" "$i" "${arr[index]}"
    str=${str}'#'
    sleep 0.1
    let i++
  done
}

function main()
{
  proc
}

main
