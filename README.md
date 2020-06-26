# cppsketch
Simple system for hot-reloading C++ classes in order to sketch ideas and algorithms. It's quite naive, in that it just starts your program again and doesn't save variables. Also, at the moment, it only works for one class at a time.

(Mac only for now)
Here's a video playlist on YouTube for how to use it: https://www.youtube.com/watch?v=-JlMIAOluUg&list=PL3QE2n0UDmRp2R7x-nB5dz1BPHVuBuKZS
It has no dependencies - everything is included in here. (except if you want to run the audio example, see below)

## VIDEO

The best documentation is the 3 minute video on youtube here: https://www.youtube.com/watch?v=-JlMIAOluUg

## How to compile
cd to examples/simple and type `make`

## How to run
type `./simple` - now you can edit MyLiveApp.h live.


## Audio example
(there's a 2 minute video tutorial of how to do this here: https://www.youtube.com/watch?v=hVUjeGlL7Y8)

To compile this example, do the following

1. `cd` to the cppsketch repository
2. type `git submodule init` followed by `git submodule update` - this gets the rtAudio
3. `cd examples/audio`
4. `make`

To run it, type `./audio` - now you can edit MyLiveAudio.h live, and every time you save it should update.

## openFrameworks
Please see the readme in the examples/openFrameworks directory to see how to do that, or go here for an addon https://github.com/elf-audio/ofxCppSketch (probably easier)

## linux / raspberry pi
At the moment, only the simple and audio examples work. You may need to install libasound2-dev. You can do this by typing `sudo apt install libasound2-dev`


