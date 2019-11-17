//
//  ofxCppSketchSoundStream.h
//  deathstortion
//
//  Created by Marek Bereza on 01/11/2019.
//

#include "ofxCppSketchSoundStream.h"
#include "ofxCppSketch.h"



ofxCppSketchSoundStream::ofxCppSketchSoundStream() : ofSoundStream() {
	setSoundStream(ofxCppSketch::getSoundStream());
}

