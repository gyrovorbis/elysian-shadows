rm -rf ElysianVMU.app/Contents/Frameworks/SDL2.framework
rm -rf ElysianVMU.app/Contents/Frameworks/SDL2_net.framework
macdeployqt ElysianVMU.app 
cp -R ../../../libGyro/lib/osx/SDL2.framework ElysianVMU.app/Contents/Frameworks
cp -R ../../../libGyro/lib/osx/SDL2_net.framework ElysianVMU.app/Contents/Frameworks

install_name_tool -change ElysianVMU.app/Contents/Frameworks/SDL2/Versions/A/SDL2 @executable_path/../Frameworks/SDL2/Versions/A/SDL2 ElysianVMU.app/Contents/MacOS/ElysianVMU
install_name_tool -change ElysianVMU.app/Contents/Frameworks/SDL2_net/Versions/A/SDL2_net @executable_path/../Frameworks/SDL2_net/Versions/A/SDL2_net ElysianVMU.app/Contents/MacOS/ElysianVMU



#Since MacDeployQt is still too fucking dumb to handle 3rd party Framework deployment

