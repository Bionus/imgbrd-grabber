#!/usr/bin/env sh

set -e

find "src/" -name '*.cpp' -o -name '*.h' \
| grep -v "vendor" \
| grep -v "crashhandler" \
| grep -v "backports" \
| grep -v "cmake" \
| grep -v "gui" \
| uncrustify -c "uncrustify.cfg" --replace --no-backup -F -
