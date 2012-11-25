#!/bin/sh

if [ $# -le 0 ]; then
    echo "Usage: create_level.sh filename.xcf"
    exit 1
fi

out_str_mask=""
out_str_level=""
out_str_event=""
while [ $# -gt 0 ]; do
    filename=`tempfile`
    convert $1 $filename.png
    convert $filename-0.png $filename.0.pnm
    convert $filename-1.png $filename.1.pnm
    convert $filename-2.png $filename.2.pnm
    rm -f $filename.png.* $filename

    convert $filename.0.pnm $filename.0.png
    convert $filename.1.pnm $filename.1.png
    convert $filename.2.pnm $filename.2.png

    rm -f $filename.*.pnm

    out_str_level="$out_str_level $filename.0.png"
    out_str_mask="$out_str_mask $filename.1.png"
    out_str_event="$out_str_event $filename.2.png"
    shift;
done

echo -n "Level tiles: "
tilecreator  $out_str_level
mv out.bmp level_tiles.bmp

echo -n "Mask tiles: "
tilecreator $out_str_mask
mv out.bmp mask_tiles.bmp

echo -n "Event tiles: "
tilecreator $out_str_event
mv out.bmp event_tiles.bmp

# Cleanup
rm -f  $filename.*.png
