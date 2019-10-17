# cppsketch with openFrameworks

This only works on the mac, only tested with Xcode 11.

## HOW TO USE cppsketch with openFrameworks
1. Drop all the files in src into an openFrameworks empty project
2. Change your main.cpp to `#include "ofxCppSketch.h"`
3. change the line 	
`ofRunApp(new ofApp());`
to
`ofRunApp(new ofxCppSketch("ofApp", __FILE__));`
4. Go into project build settings, find "Dead code stripping" - change it to NO

Now, every time you save ofApp.h or ofApp.cpp whilst your app is running, it will recompile in the background - errors are displayed in the xcode console if compilation fails.