#!/bin/sh
sim_dir=`xcrun instruments -s | grep "iPhone 6 (8.3 Simulator)" | awk {'print $NF'} | tr -d '[]'`
xcrun instruments -w "iPhone 6 (8.3 Simulator)"
rm -rf /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESToolkit.app/assets
ln -s /Users/falcogirgis/esdev/assets/ /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESToolkit.app
xcrun simctl uninstall booted edu.uah.cpe.ESToolkit
xcrun simctl install booted /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESToolkit.app
xcrun simctl launch booted edu.uah.cpe.ESToolkit
tail -f ~/Library/Logs/CoreSimulator/$sim_dir/system.log