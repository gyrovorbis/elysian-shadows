rm -rf ESToolkit.app/Contents/Frameworks/SDL2.framework
cp -R ../../libGyro/lib/osx/SDL2.framework ESToolkit.app/Contents/Frameworks
install_name_tool -change ESToolkit.app/Contents/Frameworks/SDL2/Versions/A/SDL2 @executable_path/../Frameworks/SDL2/Versions/A/SDL2
macdeployqt ESToolkit.app 
#Since MacDeployQt is still too fucking dumb to handle 3rd party Framework deployment

