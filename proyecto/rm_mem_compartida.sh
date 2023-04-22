#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Se necesitan dos argumentos, el primero el primer id de memoria compartida, el segundo, el último id de memoria compartida a borrar"
  exit 1
fi

i=$1

while($1 -le $2)
do
    ipcrm -m $i
    i=$((i+1))
done

echo "Memorias borradas\n"

exit 1