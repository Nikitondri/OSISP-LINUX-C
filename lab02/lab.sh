#!/bin/bash
>/tmp/errors.err
SRC="$1" 2>>/tmp/errors.err
XCUTE="${SRC%%.*}.exe"
if gcc -o "$XCUTE" "$SRC" 2>>/tmp/errors.err;
then
shift
"./$XCUTE" $*
fi
while read line; do
	echo "${0##*/}: $line" >&2
done <  /tmp/errors.err
rm /tmp/errors.err 
