#!/bin/bash

for FILENAME in *.ts; do
    FORMATTED="${FILENAME}_formatted"
    xmllint --format "$FILENAME" > "$FORMATTED" && mv "$FORMATTED" "$FILENAME"
done
