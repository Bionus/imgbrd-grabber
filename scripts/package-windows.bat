:: Package everything into a "Grabber.zip" file at the root of the git repository

:: Copy all required files to the release directory
bash scripts/package.sh "release"
rmdir /S /Q "release/sites/"
mkdir "release/sites"
xcopy /I /E /Y /K /F /EXCLUDE:src\sites\exclude_xcopy.txt "src/sites" "release/sites/"
xcopy /I /E /Y /K /F /H "src/dist/windows" "release"

:: Add Qt DLL and files
%Qt6_Dir%\bin\windeployqt --dir "release" "release/Grabber.exe" --release --no-quick-import --no-opengl-sw
copy %Qt6_Dir%\lib\qscintilla2_qt6.dll "release"
copy %Qt6_Dir%\bin\Qt6PrintSupport.dll "release"

:: Add OpenSSL and MySQL DLL
copy %OPENSSL_ROOT_DIR%/libcrypto-3*.dll "release"
copy %OPENSSL_ROOT_DIR%/libssl-3*.dll "release"
copy %MYSQL_DRIVER_DIR%/libmysql.dll "release"

:: Zip the whole directory
pushd release
    7z a -r "../Grabber.zip" .
popd

:: Cleanup
::rmdir /S /Q release
