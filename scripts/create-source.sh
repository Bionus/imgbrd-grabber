#!/usr/bin/env bash
# Create the skeleton source code for a new source
# Usage: ./scripts/create-source.sh "Source Name" "www.source-domain.com"

NAME=$1
DOMAIN=$2

# Create source
DIR="src/sites/$NAME"
mkdir -p "$DIR"
echo $DOMAIN > "$DIR/sites.txt"
echo $DOMAIN > "$DIR/supported.txt"
touch "$DIR/model.ts"
wget -qO- "https://$DOMAIN/favicon.ico" | convert ico:- "$DIR/icon.png"

# Create site
DIR_SITE="$DIR/$DOMAIN"
mkdir -p "$DIR_SITE"
echo -e "[General]\nname=$NAME\nssl=true" > "$DIR_SITE/defaults.ini"
