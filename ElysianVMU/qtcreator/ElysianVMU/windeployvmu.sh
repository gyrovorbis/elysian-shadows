cd ../../../executable
rm -rf *.dll
windeployqt --release --compiler-runtime ElysianVMU.exe
cd ../libGyro/lib/win32
cp SDL2.dll ../../../executable
cp OpenAL32.dll ../../../executable
cp SDL2_net.dll ../../../executable