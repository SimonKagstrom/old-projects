#!/bin/sh

if [ $# -le 0 ]; then
    echo "Usage: trim_script.sh filename"
    exit 1
fi

while [ $# -gt 0 ]; do
    convert -trim $1 tmp.png
    if [ $? -ne 0 ]; then
	exit 1
    fi
    echo -n "rect_t $1 " | sed 's/.png//g'

    ORIG_X=`identify $1 | sed 's/x/, /g' | awk '{ print $1}'`
    ORIG_Y=`identify $1 | sed 's/x/, /g' | awk '{ print $2}'`
    identify tmp.png | sed 's/x/, /g' | awk '{ printf "= {0,0, %d, %d};\n", $3, $4}'

    rm -f tmp.png

    shift
done