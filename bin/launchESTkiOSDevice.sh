#!/bin/sh

#rm -rf /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphoneos/ESToolkit.app/assets

#ln -s /Users/falcogirgis/esdev/assets/ /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphoneos/ESToolkit.app

this_dir=`dirname $0`
echo "GO FUCK YOURSELF"
$this_dir/ios-deploy -9 -1 com.elysianshadows.estk -i 617f198c931bbecb325bfd7341413098d3216a0c
cp -rf /Users/falcogirgis/esdev/assets /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphoneos/ESToolkit.app
echo "BEFORE RUBY"
/usr/bin/ruby $this_dir/transporter_chief.rb /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphoneos/ESToolkit.app -v -d 617f198c931bbecb325bfd7341413098d3216a0c
echo "AFTER RUBY"
$this_dir/deviceconsole -p ESToolkit&
instruments -w 617f198c931bbecb325bfd7341413098d3216a0c -t /Applications/Xcode.app/Contents/Applications/Instruments.app/Contents/PlugIns/AutomationInstrument.xrplugin/Contents/Resources/Automation.tracetemplate /Users/falcogirgis/esdev/ESToolkit/ESToolkit/Debug-iphoneos/ESToolkit.app -e UIASCRIPT /Users/falcogirgis/esdev/assets/bin/Launch-App.js -e UIARESULTSPATH output.txt
