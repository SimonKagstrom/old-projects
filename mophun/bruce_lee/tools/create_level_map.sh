#!/bin/sh

if [ $# -le 3 ]; then
    echo "Usage: create_level_map.sh BASENAME LEVEL_TILES MASK_TILES EVENT_TILES filename.xcf"
    exit 1
fi

basename=$1
level_tiles=$2
mask_tiles=$3
event_tiles=$4
in_file=$5

filename=`tempfile`
convert $in_file $filename.png
convert $filename-0.png $filename.0.pnm
convert $filename-1.png $filename.1.pnm
convert $filename-2.png $filename.2.pnm

rm -f $filename.png.* $filename

convert $filename.0.pnm tmp0.png
convert $filename.1.pnm tmp1.png
convert $filename.2.pnm tmp2.png

rm -f $filename.*.pnm

mapcreator -n $basename $level_tiles tmp0.png
mapcreator -n $basename\_mask $mask_tiles tmp1.png
tools/eventmap/eventmap $basename\_events $event_tiles tmp2.png

# Cleanup
rm -f tmp0.png tmp1.png tmp2.png
