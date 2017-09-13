cd ../../executable
rm -rf *.dll
windeployqt --release --compiler-runtime ESToolkit.exe
cd ../libGyro/lib/win32
cp *.dll ../../../executable
chmod -R 777 ../../../executable