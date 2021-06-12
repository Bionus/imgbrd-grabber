#!/usr/bin/env bash
# Generate tag databases and copy them to the "databases" directory

set -e

DESTDIR="databases"
SOURCES=(
    "Gelbooru (0.2)/gelbooru.com"
    "Gelbooru (0.2)/rule34.xxx"
)
MIN_TAG_COUNT=100

mkdir -p $DESTDIR

for FULL in "${SOURCES[@]}"
do
    SOURCE=${FULL%%/*}
    SITE=${FULL##*/}

    echo "Loading tags for $SITE ($SOURCE)..."

    mkdir -p "sites/$SOURCE/$SITE"
    touch "sites/$SOURCE/$SITE/tags.db"

    ./Grabber.AppImage -c --load-tag-database --sources $SITE --tags-min $MIN_TAG_COUNT
    cp "sites/$SOURCE/$SITE/tags.db" "$DESTDIR/$SITE.db"
done
