#!/usr/bin/env sh

set -e

# "src/gui/src/" "src/tests/src/"
find "src/lib/src/" "src/cli/src/" "src/e2e/src/" -name '*.cpp' -o -name '*.h' \
| grep -v "vendor" \
| grep -v "crashhandler" \
| grep -v "backports" \
| uncrustify -c "uncrustify.cfg" --replace --no-backup -F -
