#!/bin/sh
sim_dir=`xcrun instruments -s | grep "iPhone 6 (8.3 Simulator)" | awk {'print $NF'} | tr -d '[]'`
xcrun instruments -w "iPhone 6 (8.3 Simulator)"
rm -rf /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESGamma_iOS.app/assets
ln -s /Users/falcogirgis/esdev/assets/ /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESGamma_iOS.app
xcrun simctl uninstall booted edu.uah.cpe.ESGamma-iOS
xcrun simctl install booted /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphonesimulator/ESGamma_iOS.app
xcrun simctl launch booted edu.uah.cpe.ESGamma-iOS
tail -f ~/Library/Logs/CoreSimulator/$sim_dir/system.log