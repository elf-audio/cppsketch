# cppsketch
Simple system for hot-reloading C++ classes in order to sketch ideas and algorithms. It's quite naive, in that it just starts your program again and doesn't save variables. Also, at the moment, it only works for one class at a time.

(Mac only for now)

It has no dependencies - everything is included in here. (except if you want to run the audio example, see below)

## How to compile
cd to examples/simple and type `make`

## How to run
type `./simple` - now you can edit MyLiveApp.h live.


## Audio example

To compile this example, do the following
1. `cd` to the cppsketch repository
2. type `git submodule init` followed by `git submodule update` - this gets the rtAudio
3. `cd examples/audio`
4. `make`

To run it, type `./audio` - now you can edit MyLiveAudio.h live, and every time you save it should update.

## openFrameworks
Please see the readme in the examples/openFrameworks directory to see how to do that.