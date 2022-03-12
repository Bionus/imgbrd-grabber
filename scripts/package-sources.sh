#!/usr/bin/env bash
# Package every sources into a "Name.zip" file in the "sources_package" directory.

OUTPUT_DIR=`realpath sources_packages`
mkdir -p $OUTPUT_DIR

for DIR in src/sites/*/; do
    NAME=$(basename "$DIR")

    [[ $NAME =~ ^(node_modules|Sankaku|Tumblr|Reddit)$ ]] && continue
    [[ ! -f "$DIR/model.js" ]] && continue

    pushd "$DIR"
        zip "$OUTPUT_DIR/$NAME.zip" model.js icon.png sites.txt */defaults.ini */tag-types.txt
    popd
done
