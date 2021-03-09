#!/usr/bin/env bash

# Restore submodules in case the repo was clone without --recursive
git submodule update --init --recursive

# Install required packages
declare -i no_pm=1 installed=0
for pm in pacman apt-get emerge; do
	if type "$pm" > /dev/null 2>&1 ; then
		no_pm=0
		case "$pm" in
			pacman)
				sudo pacman -Sy
				sudo pacman -S "qt" "gcc" "cmake" "libpulse" "nodejs" "npm" && installed=1
				;;
			apt-get)
				sudo apt-get install -qq "qtbase5-dev" "qtscript5-dev" "qtmultimedia5-dev" "qtdeclarative5-dev" "qttools5-dev" "qttools5-dev-tools" || continue
				sudo apt-get install -qq "g++" "cmake" "libssl-dev" "nodejs" "npm" && installed=1
				;;
			emerge)
				## Dependencies untested!
				sudo emerge --no-replace --ask --verbose \
					"dev-qt/qtcore" "dev-qt/qtscript" "dev-qt/qtmultimedia" "dev-qt/qtdeclarative" \
					"sys-devel/gcc" "dev-util/cmake" "net-libs/nodejs" \
				&& installed=1
				;;
		esac
		if ((installed)); then break; fi
	fi
done
if ((no_pm)); then
	echo "Failed to detect your distribution's package manager."
	exit 11
fi
if ((!installed)); then exit 12; fi

# Build the project in the build directory
./scripts/build.sh ${1:-gui translations} || exit 21

# Move the built binary to the release folder with its config
./scripts/package.sh "release"
cp -r src/dist/linux/* "release"

echo "Grabber has been compiled in the release directory. To run it, type './release/Grabber'"
