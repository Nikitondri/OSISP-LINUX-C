#!/bin/bash
SRC="$1"
XCUTE="${SRC%%.*}.exe"
if gcc -o "$XCUTE" "$SRC";
then
shift
"./$XCUTE" $*
else
echo "Compilation error"
fi 
