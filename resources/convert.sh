#!/bin/bash

P="../trunk/resources"

#echo $P;

mkdir -p $P/hicolor/256x256/apps/
mkdir -p $P/hicolor/128x128/apps/
mkdir -p $P/hicolor/64x64/apps/
mkdir -p $P/hicolor/48x48/apps/
mkdir -p $P/hicolor/32x32/apps/
mkdir -p $P/hicolor/22x22/apps/
mkdir -p $P/hicolor/16x16/apps/

convert -quality 90 -resize 256x256 $P/vimka.png $P/hicolor/256x256/apps/vimka.png
convert -quality 90 -resize 128x128 $P/vimka.png $P/hicolor/128x128/apps/vimka.png
convert -quality 90 -resize   64x64 $P/vimka.png $P/hicolor/64x64/apps/vimka.png
convert -quality 90 -resize   48x48 $P/vimka.png $P/hicolor/48x48/apps/vimka.png
convert -quality 90 -resize   32x32 $P/vimka.png $P/hicolor/32x32/apps/vimka.png
convert -quality 90 -resize   22x22 $P/vimka.png $P/hicolor/22x22/apps/vimka.png
convert -quality 90 -resize   16x16 $P/vimka.png $P/hicolor/16x16/apps/vimka.png

