#!/bin/bash 

i=0
while [ $i -le 100 ]
do
  echo "hello world $i" >> file
  let i++
done

