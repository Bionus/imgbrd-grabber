#!/usr/bin/env bash

if [[ $USER == 'root' ]]; then permission=
else
	if type sudo > /dev/null 2>&1 ; then permission='sudo '
	else
		echo 'Run as root or install sudo.'
		exit 1
	fi
fi

# Restore submodules in case the repo was clone without --recursive
git submodule update --init --recursive

# Install required packages
declare -i no_pm=1 installed=0
for pm in pacman apt-get emerge yum; do
	if type "$pm" > /dev/null 2>&1 ; then
		no_pm=0
		case "$pm" in
			pacman)
				$permission pacman -Sy
				$permission pacman -S "qt" "gcc" "cmake" "libpulse" "nodejs" "npm" && installed=1
				;;
			apt-get)
				$permission apt-get install -qq "qtbase5-dev" "qtscript5-dev" "qtmultimedia5-dev" "qtdeclarative5-dev" "qttools5-dev" "qttools5-dev-tools" || continue
				$permission apt-get install -qq "g++" "cmake" "libssl-dev" "nodejs" "npm" && installed=1
				;;
			emerge)
				## Assumes default USE flags are enabled.
				$permission emerge --noreplace --ask --verbose \
					"dev-qt/qtcore" "dev-qt/qtscript" "dev-qt/qtmultimedia" "dev-qt/qtdeclarative" "dev-qt/qtsql" \
					"dev-qt/qtnetwork" "dev-qt/qtnetworkauth" \
					"sys-devel/gcc" "dev-util/cmake" "net-libs/nodejs" \
				&& installed=1
				;;
			yum)
				## Assumes default USE flags are enabled.
				$permission yum install -y \
          openssl-devel g++ cmake nodejs npm qt5-qtbase-devel qt5-qtscript-devel qt5-qtmultimedia-devel qt5-qtdeclarative-devel qt5-qttools-devel rsync \
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
if (($#)); then ./scripts/build.sh "$@" || exit 21
else ./scripts/build.sh 'gui' 'translations' || exit 21
fi

# Move the built binary to the release folder with its config
./scripts/package.sh "release"
cp -r src/dist/linux/* "release"

echo "Grabber has been compiled in the release directory. To run it, type './release/Grabber'"
