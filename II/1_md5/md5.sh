#!/bin/bash

if [[ $# != 2 ]];
then
	echo -e "\n  Usage: md5.sh file1 file2\n"
else
	declare -A Data

	# reading first file to Data
	exec < $1
	while read md5 size;
	do
		#echo +++$md5_size+++
		while read file && [[ -n $file ]];
		do
			#echo ---$file---
			Data[$file]="[$md5|$size]"
		done;
	done;

	# reading second file and comparing with Data
	exec < $2
	while read md5 size file;
	do
		#echo +++$md5*$size*$file+++
		if [[ -n $md5 && -n $size && -n $file ]];
		then
			if [[ -n ${Data[$file]} && ${Data[$file]} != "[$md5|$size]" ]];
			then
				echo file: $file
				echo -e "\texpected -> [$md5|$size]"
				echo -e "\tin place -> ${Data["$file"]}"
			fi;
		fi;
	done;
fi;
