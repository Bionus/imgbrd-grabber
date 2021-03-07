#!/usr/bin/env bash
# Generate tag databases and copy them to the "databases" directory

set -e

DESTDIR="databases"
SOURCES=(
    "Gelbooru (0.2)/gelbooru.com"
)
MIN_TAG_COUNT=100

mkdir -p $DESTDIR

./scripts/package.sh "app_dir"
pushd "app_dir"

for FULL in "${SOURCES[@]}"
do
    SOURCE=${FULL%%/*}
    SITE=${FULL##*/}

    ./Grabber-cli --load-tag-database --sources $SITE --tags-min $MIN_TAG_COUNT
    cp "sites/$SOURCE/$SITE/tags.db" "../$DESTDIR/$SITE ($MIN_TAG_COUNT).db"
done

popd