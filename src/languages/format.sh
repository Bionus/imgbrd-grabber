#!/bin/bash

for FILENAME in *.ts; do
    FORMATTED="${FILENAME}_formatted"
    xmllint --format "$FILENAME" > "$FORMATTED" && mv "$FORMATTED" "$FILENAME"

    grep '<translation type="unfinished">\w' "$FILENAME"

    # sed -E 's/<translation type="unfinished">([^<]*)<\/translation>/<translation>\1<\/translation>/g' "$FILENAME" > "$FIXED"
    # mv "$FIXED" "$FILENAME"
done
