#!/usr/bin/env bash
#####################{{{1
# Variables
#####################
srcDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
oldApp="${HOME}/Applications/Grabber.app/Contents/MacOS"
appDir="${srcDir}/gui/build/release/Grabber.app/Contents/MacOS"
logfile="$(mktemp -t $(basename $0))"
[[ $? -ne 0 ]] && echo -e "${logfile}" && exit 1

#####################{{{1
# Build Environment Validation
#####################

# Is Homebrew Installed? {{{2
export BREW_BIN="$(which brew)"
if [[ -z "${BREW_BIN}" ]]
then
	echo "Homebrew not found. Installing with the following command: "
	echo '	ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"'
	read -p 'Do you agree? [y|N]' AGREE
	case "${AGREE}" in
		[yY]|[yY][eE]|[yY][eE][sS]) #{{{3
			ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
			export BREW_BIN=$(which brew)
			;;
		*) #{{{3
			echo "You must type 'yes' to accept. Aborting."; exit 1
			;; #}}}3
	esac
fi

#Is cmake installed? {{{2
which cmake > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
	echo "cmake(1) is required to build imgbrd-grabber. Installing with brew..."
	${BREW_BIN} install cmake
	which cmake > /dev/null 2>&1
	[[ $? -ne 0 ]] && echo "Failed to install cmake. Aborting." && exit 1
fi

#Is gcc installed? {{{2
which gcc > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
	echo "gcc(1) is required to build imgbrd-grabber. Installing with brew..."
	${BREW_BIN} install gcc
	which gcc > /dev/null 2>&1
	[[ $? -ne 0 ]] && echo "Failed to install gcc. Aborting." && exit 1
fi

#Is node installed? {{{2
which node > /dev/null 2>&1
if [[ $? -ne 0 ]]
then
	echo "node is required to build imgbrd-grabber. Installing with brew..."
	${BREW_BIN} install node
	which npm > /dev/null 2>&1
	[[ $? -ne 0 ]] && echo "Failed to install nodejs. Aborting." && exit 1
fi

# Is Qt installed?  {{{2
QT_BIN_ROOT="$(${BREW_BIN} --prefix)/opt/qt/bin"
if [[ ! -e "${QT_BIN_ROOT}" ]]
then
	echo "Qt5 build binaries not found at '${QT_BIN_ROOT}'. Installing with homebrew."
	echo "Qt5 can be installed with mysql integration. This requires qt5 to be compiled from scratch and takes a VERY long time."
	read -p 'Do you want to compile Qt5 with mysql (can take 1hr or more to finish) [y|N|abort]' AGREE
	case "${AGREE}" in
		[yY]|[yY][eE]|[yY][eE][sS]) #{{{3
			echo "Compiling qt from source code with mysql. This can take one hour or more to complete..."
			"${BREW_BIN}" install qt --with-docs --with-mysql
			QT_BIN_ROOT="$(${BREW_BIN} --prefix)/opt/qt/bin"
			;;
		[aA]|[aA][bB]|[aA][bB][oO]|[aA][bB][oO][rR]|[aA][bB][oO][rR][tT]) #{{{3
			echo "User requested abort."
			exit 1
			;;
		*) #{{{3
			"${BREW_BIN}" install qt
			QT_BIN_ROOT="$(${BREW_BIN} --prefix)/opt/qt/bin"
			;; #}}}3
	esac
	"${QT_BIN_ROOT}/qmake" -v >/dev/null 2>&1
	ERR=$?
	[[ $ERR -ne 0 ]] && echo "Failed to install Qt. Aborting." && exit 1
fi

# Are the Qt build binaries found in our Path variable? {{{2
echo "${PATH}"|grep "${QT_BIN_ROOT}" >/dev/null 2>&1
ERR=$?
if [[ $ERR -ne 0 ]]
then
	echo "QT build binaries not found in path. Adding '${QT_BIN_ROOT}' to path."
	export PATH="${QT_BIN_ROOT}:${PATH}"
fi

# We use the app JQ to parse brews package details. Is it installed? {{{2
JQ_PATH="$(which jq)"
ERR=$?
if [[ $ERR -ne 0 ]]
then
	echo "The program jq(1) is used to parse Brew package information but was not found. Installing..."
	"${BREW_BIN}" install jq
	JQ_PATH="$(which jq)"
	ERR=$?
	[[ $ERR -ne 0 ]] && echo "Failed to install jq. Aborting." && exit 1
fi

# Is OpenSSL installed? {{{2
export OPENSSL_ROOT_DIR="$("${BREW_BIN}" info --json=v1 openssl|"${JQ_PATH}" -r '.[0].bottle.stable.cellar,.[0].name,.[0].installed[-1].version'|tr '\n' '/')"
export OPENSSL_INCLUDE_DIR="${OPENSSL_ROOT_DIR}/lib"
if [[ ! -e "${OPENSSL_ROOT_DIR}" ]]
then
	echo "A local version of OpenSSL is required to build against. Installing. (This will not overwrite the systems OpenSSL installation)"
	"${BREW_BIN}" install openssl
	export OPENSSL_ROOT_DIR="$("${BREW_BIN}" info --json=v1 openssl|"${JQ_PATH}" -r '.[0].bottle.stable.cellar,.[0].name,.[0].installed[-1].version'|tr '\n' '/')"
	export OPENSSL_INCLUDE_DIR="${OPENSSL_ROOT_DIR}/lib"

	if [[ ! -e "${OPENSSL_INCLUDE_DIR}" ]]
	then
		echo "Failed to install openssl, or was unable to find the includes directory at '${OPENSSL_INCLUDE_DIR}'. Aborting."
		exit 1
	fi
fi

# Is the imgbrd-grabber repo checked out to the current directory? 
if [[ ! -e "${srcDir}/.git" ]]
then
	APP_NAME="$(basename $0 2>/dev/null )"
	ERR=$?
	TEMPDIR="$(mktemp -d -t ${APP_NAME}.tmpdir 2>/dev/null )"
	ERR=$(( $ERR + $? ))
	git clone https://github.com/Bionus/imgbrd-grabber.git "${TEMPDIR}"
	ERR=$(( $ERR + $? ))
	cp -r "${TEMPDIR}/" "${srcDir}/"
	ERR=$(( $ERR + $? ))
	if [[ $ERR -ne 0 ]]
	then
		echo "Failed to clone down the git repository of imgbrd-grabber. Cannot compile most recent version. Aborting."
		exit 1
	fi
	rm -rf "${TEMPDIR}"
else
	git pull
	ERR=$?
	if [[ $ERR -ne 0 ]]
	then
		echo "Failed to pull updates to the git repository of imgbrd-grabber. Continuing, but you may not be compiling the latest release..."
	fi
fi

#####################{{{1
# Main
#####################

#Clean the build environment {{{2
if [[ -e "${srcDir}/build" ]]
then
	rm -rf "${srcDir}/build"
	mkdir "${srcDir}/build"
else
	mkdir "${srcDir}/build"
fi

#Add the macos Icon files
cp -r "${srcDir}"/macos/* "${srcDir}/TEMP-Grabber.app/Contents"

#Create the template directory structure for a MacOS App {{{2
mkdir -p "${appDir}"

#Commence Building {{{2
echo "Building imgbrd-grabber. Build output can be found at:"
echo
echo "	${logfile}"
echo
cd "${srcDir}/build"
cmake .. -DOPENSSL_ROOT_DIR="${OPENSSL_ROOT_DIR}" -DOPENSSL_LIBRARIES="${OPENSSL_INCLUDE_DIR}" > "${logfile}" 2>&1
ERR=$?
make -j8 > "${logfile}" 2>&1
ERR=$(( $ERR + $? ))
cd ..

if [[ $ERR -ne 0 ]]
then
	echo
	echo "Failed to build imgbrd-grabber."
	echo "Last 40 lines of the log file follow."
	tail -n 40 "${logfile}"
	echo
	exit 1
fi
#Pack up the MacOS Application {{{2
\cp "${srcDir}/build/gui/Grabber" "${appDir}"
\cp -r "${srcDir}"/release/* "${appDir}"
\touch "${appDir}/settings.ini"

#Copy imgbrd-grabber configs from previous builds and inject into new App {{{2
if [[ -e "${HOME}/Applications/Grabber.app" ]]
then
	echo "Old build has been found at '${HOME}/Applications/Grabber.app'. Migrating configs into new build..."
	\cp "${oldApp}"/*.txt "${appDir}" >/dev/null 2>&1
	\cp "${oldApp}"/*.ini "${appDir}" >/dev/null 2>&1
	\cp "${oldApp}"/*.log "${appDir}" >/dev/null 2>&1
	\cp "${oldApp}"/*.igl "${appDir}" >/dev/null 2>&1
	\cp -r "${oldApp}/thumbs" "${appDir}" >/dev/null 2>&1
fi

#Clean out any build of imgbrd-grabber that still exists from a previous execution of this script {{{2
if [[ -e "${srcDir}/TEMP-Grabber.app" ]]
then
	rm -rf "${srcDir}/TEMP-Grabber.app"
fi
if [[ -e "${srcDir}/grabber-release" ]]
then
	rm -rf "$srcDir/grabber-release"
fi

#Add the macos Icon files
cp -r "${srcDir}"/macos/* "${srcDir}/TEMP-Grabber.app/Contents"

mv "${srcDir}/gui/build/release/Grabber.app" "${srcDir}/TEMP-Grabber.app"

#Decide if we are supposed to move the App to ${HOME}/Applications {{{2
echo "Finished Compiling updated version of imgbrd-grabber. Application is now at '${srcDir}/TEMP-Grabber.app'"
read -p "Would you like to copy this to '${HOME}/Applications/Grabber.app'? [Y|n]" AGREE
case "${AGREE}" in
	[nN]|[nN][oO]) #{{{3
		echo "Will not copy application from '${srcDir}/TEMP-Grabber.app' to ~/Applications/Grabber.app"
		echo "WARNING - This application will be destroyed the next time ${0} executes!"
		cp -R "${srcDir}/TEMP-Grabber.app/Contents/MacOS/Grabber" "${srcDir}/TEMP-Grabber.app/Contents/MacOS/TEMP-Grabber" >/dev/null 2>&1
		APP_PATH="${srcDir}/TEMP-Grabber.app"
		;;
	*) #{{{3
		if [[ -e ${HOME}/Applications/Grabber.app ]] #{{{4
		then
			echo "A copy of imgbrd-grabber already exists at '${HOME}/Applications/Grabber.app'"
			DATE=$(date +%Y-%m-%d_%H%M%S)
			read -p "Copy '${HOME}/Applications/Grabber.app' to '${HOME}/Applications/Grabber_${DATE}.app'? [y|N]" AGREE
			case "${AGREE}" in
				[yY]|[yY][eE]|[yY][eE][sS]) #{{{5
					cp -R ${HOME}/Applications/Grabber.app ${HOME}/Applications/Grabber_${DATE}.app >/dev/null 2>&1
					ERR=$?
					[[ $ERR -ne 0 ]] && echo "Unable to copy '${HOME}/Applications/Grabber.app' to '${HOME}/Applications/Grabber_${DATE}.app'. Aborting." && exit 1
					cp -R "${srcDir}/TEMP-Grabber.app" "${HOME}/Applications/Grabber.app" >/dev/null 2>&1
					APP_PATH="${HOME}/Applications/Grabber.app"
					;;
				*) #{{{5
					echo "Will not move application from '${srcDir}/TEMP-Grabber.app' to ~/Applications/Grabber.app"
					echo "WARNING - This application will be destroyed the next time ${0} executes!"
					cp -R "${srcDir}/TEMP-Grabber.app/Contents/MacOS/Grabber" "${srcDir}/TEMP-Grabber.app/Contents/MacOS/TEMP-Grabber" >/dev/null 2>&1
					APP_PATH="${srcDir}/TEMP-Grabber.app"
					;; #}}}5
			esac
		else #{{{4
			cp -R "${srcDir}/TEMP-Grabber.app" "${HOME}/Applications/Grabber.app" >/dev/null 2>&1
			APP_PATH="${HOME}/Applications/Grabber.app"
		fi #}}}4
		;; #}}}3
esac


#Create a .dmg file? {{{2
echo "${srcDir}"
read -p 'Would you like to create a .dmg file? [Y|n]' AGREE
case "${AGREE}" in
	[yY]|[yY][eE]|[yY][eE][sS]|'') #{{{3
		mkdir -p "${srcDir}/grabber-release"
		ln -s  /Applications "${srcDir}/grabber-release/Applications" 
		mv "${srcDir}/TEMP-Grabber.app" "${srcDir}/grabber-release/Grabber.app"
		macdeployqt "${srcDir}/grabber-release/Grabber.app"
		hdiutil create -volname grabber -srcfolder "${srcDir}/grabber-release/" -ov -format UDRW -o "${srcDir}/grabber"
		;;
	*) #{{{3
		echo "Finished dmg can be found at '${srcDir/grabber.dmg}'"
		;; #}}}3
esac #}}}2



#Launch the finished app? {{{2
echo "${APP_PATH}"
read -p 'Would you like to launch the app now? [Y|n]' AGREE
case "${AGREE}" in
	[yY]|[yY][eE]|[yY][eE][sS]|'') #{{{3
		\open "${APP_PATH}"
		;;
	*) #{{{3
		echo "Finished Application can be found at '${APP_PATH}'"
		;; #}}}3
esac #}}}2
#}}}1
# vim:ts=4:sw=4:tw=0:noexpandtab:autoindent:foldmethod=marker:foldcolumn=4
