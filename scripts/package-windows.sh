#!/usr/bin/env bash
# Package everything into a "Grabber.zip" file at the root of the git repository

APP_DIR="release"

# Copy all required files to the release directory
./scripts/package.sh $APP_DIR
cp -r src/dist/windows/* $APP_DIR

# Add Qt DLL and files
$Qt5_Dir/bin/windeployqt.exe --dir $APP_DIR "$APP_DIR/Grabber.exe"

# Add OpenSSL and MySQL DLL
cp $OPENSSL_ROOT_DIR/libcrypto-1_1*.dll "$APP_DIR"
cp $OPENSSL_ROOT_DIR/libssl-1_1*.dll "$APP_DIR"
cp $MYSQL_DRIVER_DIR/libmysql.dll "$APP_DIR"

# Zip the whole directory
pushd $APP_DIR
    zip -r "../Grabber_$1.zip" .
popd

# Cleanup
rm -rf $APP_DIR
