#!/bin/sh
sim_dir=`xcrun instruments -s | grep "iPhone 6 (8.3 Simulator)" | awk {'print $NF'} | tr -d '[]'`
xcrun instruments -w "iPhone 6 (8.3 Simulator)"
rm -rf /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESToolkit.app/assets
ln -s /Users/falcogirgis/esdev/assets/ /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESToolkit.app
xcrun simctl uninstall booted com.elysianshadows.estk
xcrun simctl install booted /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESToolkit.app
xcrun simctl launch booted com.elysianshadows.estk
tail -f ~/Library/Logs/CoreSimulator/$sim_dir/system.log