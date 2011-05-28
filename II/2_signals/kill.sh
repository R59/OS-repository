#!/bin/bash

EXIT_SIGNAL=20  # Ctrl+Z (SIGTSTP)

SIGKILL=9
SIGSTOP=19

if (($#==0))
then
        echo Usage: kill.sh process_name
        exit
fi;

./$1&
pid=$!

#pid=$(pidof $1)
#[[ -z $pid ]] && exit

echo my pid = $!
sleep 1

for ((sig=0; sig<32; sig++));
do
        if ((sig!=SIGKILL && sig!=SIGSTOP && sig!=EXIT_SIGNAL))
        then
                echo send signal $sig
                kill -n $sig $pid
        fi;
done

echo send EXIT_SIGNAL\($EXIT_SIGNAL\)
kill -n $EXIT_SIGNAL $pid
