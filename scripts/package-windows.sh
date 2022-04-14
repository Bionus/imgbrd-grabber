#!/usr/bin/env bash
# Package everything into a "Grabber.zip" file at the root of the git repository

# Helper to fix Windows env variables to work in bash
function fixpath() {
    line=$1
    line=${line/C://c}
    line=${line/D://d}
    line=${line//\\//}
    echo "$line"
}

APP_DIR="release"

# Copy all required files to the release directory
./scripts/package.sh $APP_DIR
cp -r src/dist/windows/* $APP_DIR

# Add Qt DLL and files
$(fixpath $Qt5_Dir)/bin/windeployqt.exe --dir $APP_DIR "$APP_DIR/Grabber.exe"

# Add OpenSSL and MySQL DLL
cp $(fixpath $OPENSSL_ROOT_DIR)/libcrypto-1_1*.dll "$APP_DIR"
cp $(fixpath $OPENSSL_ROOT_DIR)/libssl-1_1*.dll "$APP_DIR"
cp $(fixpath $MYSQL_DRIVER_DIR)/libmysql.dll "$APP_DIR"

# Zip the whole directory
pushd $APP_DIR
    zip -r "../Grabber_$1.zip" .
popd

# Cleanup
rm -rf $APP_DIR
