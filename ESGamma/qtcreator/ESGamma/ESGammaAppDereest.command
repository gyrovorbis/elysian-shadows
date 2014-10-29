rm -rf ../../../Project/ESGamma.app/Contents/Frameworks/*
mkdir ../../../Project/ESGamma.app/Contents/Frameworks

#Copy SDL.framework into App bundle
mkdir ../../../Project/ESGamma.app/Contents/Frameworks/SDL.framework
cp -R ../../../libGyro/lib/osx/SDL.framework/* ../../../Project/ESGamma.app/Contents/Frameworks/SDL.framework/

