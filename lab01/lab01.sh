#!/bin/bash
exec 2>>/tmp/errors.err
IFS=$'\n'
path=$( readlink -f $1 )
if [ -d $path ]
then
	>$2
	for i in $(find $path -type d); do
		count=0
		size=0
		for  j in $(find $i -maxdepth 1 -type f); do
			let count=count+1
			let size=$(stat -c %s $j)+size
		done
		echo "$i $size $count">>$2
	done
else
	echo "$path: is not an existing directory" >> /tmp/errors.err
fi
while read line; do
	echo "${0##*/}: $line" &(1>&2)
done <  /tmp/errors.err
rm /tmp/errors.err
