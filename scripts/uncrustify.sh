#!/usr/bin/env sh

set -e

# "src/gui/src/"
find "src/lib/src/" "src/cli/src/" "src/e2e/src/" "src/tests/src/" -name '*.cpp' -o -name '*.h' \
| grep -v "vendor" \
| grep -v "crashhandler" \
| grep -v "backports" \
| uncrustify -c "uncrustify.cfg" --replace --no-backup -F -
