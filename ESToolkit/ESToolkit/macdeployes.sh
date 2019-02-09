cd ../../executable
rm -rf ESToolkit.app/Contents/Frameworks/SDL2.framework
rm -rf ESToolkit.app/Contents/Frameworks/SDL2_net.framework
macdeployqt ESToolkit.app 
cp -R ../../libGyro/lib/osx/SDL2.framework ESToolkit.app/Contents/Frameworks
cp -R ../../libGyro/lib/osx/SDL2_net.framework  ESToolkit.app/Contents/Frameworks
install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 @executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2 ESToolkit.app/Contents/MacOS/ESToolkit
install_name_tool -change @rpath/SDL2_net.framework/Versions/A/SDL2_net @executable_path/../Frameworks/SDL2_net.framework/Versions/A/SDL2_net ESToolkit.app/Contents/MacOS/ESToolkit



#Since MacDeployQt is still too fucking dumb to handle 3rd party Framework deployment

