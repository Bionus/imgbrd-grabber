#!/bin/bash

for FILENAME in *.ts; do
    FORMATTED="${FILENAME}_formatted"
    xmllint --format "$FILENAME" > "$FORMATTED"

    FIXED="${FILENAME}_fixed"
    sed 's/<numerusform\/>//' "$FORMATTED" | perl -0777 -pe 's/<translation type="unfinished">\s*<\/translation>/<translation type="unfinished"\/>/g' - > "$FIXED"

    mv "$FIXED" "$FILENAME"
    rm "$FORMATTED"

    grep '<translation type="unfinished">\w' "$FILENAME"

    # sed -E 's/<translation type="unfinished">([^<]*)<\/translation>/<translation>\1<\/translation>/g' "$FILENAME" > "$FIXED"
    # mv "$FIXED" "$FILENAME"
done
