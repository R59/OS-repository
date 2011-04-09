#!/bin/bash

EXIT_SIGNAL=20	# Ctrl+Z (SIGTSTP)

pid=$(pidof signals.out)
[[ -z $pid ]] && exit

echo start

for ((sig=0; sig<32; sig++));
do
	if ((sig!=9 && sig!=19 && sig!=EXIT_SIGNAL))
	then
		echo $sig
		kill -n $sig $pid
	fi;
done

echo sleep 1.5 second
sleep 1.5
kill -n $EXIT_SIGNAL $pid
