APP_PATH=/Users/rabbit/Downloads/dev/SwitchGamepad/build-SwitchGamepad-Qt_6_9_3_for_macOS-Release
QT_PATH=/Users/rabbit/Qt/6.9.3

brew unlink qt
brew unlink qt6

$QT_PATH/macos/bin/macdeployqt $APP_PATH/SwitchGamepad.app

cp /opt/homebrew/opt/gcc/lib/gcc/14/libgcc_s.1.1.dylib \
    $APP_PATH/SwitchGamepad.app/Contents/Frameworks/
cp /opt/homebrew/opt/gcc/lib/gcc/14/libgfortran.5.dylib \
    $APP_PATH/SwitchGamepad.app/Contents/Frameworks/
install_name_tool -id @rpath/libgcc_s.1.1.dylib \
    $APP_PATH/SwitchGamepad.app/Contents/Frameworks/libgcc_s.1.1.dylib
install_name_tool -id @rpath/libgfortran.5.dylib \
    $APP_PATH/SwitchGamepad.app/Contents/Frameworks/libgfortran.5.dylib
install_name_tool -change libgcc_s.1.1.dylib @rpath/libgcc_s.1.1.dylib \
    $APP_PATH/SwitchGamepad.app/Contents/Frameworks/libgfortran.5.dylib

$QT_PATH/macos/bin/macdeployqt $APP_PATH/SwitchGamepad.app -no-plugins

xattr -cr $APP_PATH/SwitchGamepad.app
codesign --force --deep --sign - $APP_PATH/SwitchGamepad.app

brew link qt
brew link qt6