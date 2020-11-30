// include em.js before this file

// the function pointer to get the audio from emscripten
let getSamples = null;
let setParameter = null;
let update = null;

// you can only start calling c++ functions once emscripten's "runtime" has started
Module.onRuntimeInitialized = function() {
	
	// this is the function signature for the getSamples function.
	// equivalent to 
	// 	void getSamples(float *samples, int length, int numChans);
	//
	// the first item is the function name, 'getSamples'
	// the next item is 'number' - I think that refers to the return type,
	// probably shouldnt be a number, but it works anyway
	// the next item is an array of parameters - the first of those
	// is a float array, but emscripten treats c arrays/pointers
	// as exactly that, so you actually pass this method a number
	// that corresponds to the byte offset in emscriptens
	// heap memory for this variable. Bit confusing, sorry
	getSamples = Module.cwrap(
		'getSamples', 'number', ['number', 'number', 'number']
	);
	setParameter = Module.cwrap('setParameter', null, ['string', 'string']);
	update = Module.cwrap('update', null, null);
}


var dataHeap = null;
var dataPtr = null;

var data = new Float32Array();

// l and r are float arrays for the left and right channels that need to be filled
function audioCallback(l, r) {

	// lazy loading of the audio buffer we use to talk to c++/emscripten
    if(dataHeap == null) {
        data = new Float32Array(l.length * 2);
        // Get data byte size, allocate memory on Emscripten heap, and get pointer
        var nDataBytes = data.length * data.BYTES_PER_ELEMENT;
        dataPtr = Module._malloc(nDataBytes);

        // Copy data to Emscripten heap (directly accessed from Module.HEAPU8)
        dataHeap = new Uint8Array(Module.HEAPU8.buffer, dataPtr, nDataBytes);
        dataHeap.set(new Uint8Array(data.buffer));
    }

    // now we actually call the C++
    getSamples(dataHeap.byteOffset, l.length, 2);

    // turn the emscripten heap array to something we can work with in javascript
    // allocating on the audio thread, I know!
    var result = new Float32Array(dataHeap.buffer, dataHeap.byteOffset, data.length);

    // deinterleave the result
    for(i = 0; i < l.length; i++) {
        l[i] = result[i*2];
        r[i] = result[i*2+1];
    }

    //// Free memory - we're not going to do that here. Ever.... AHAAAHAAHAHAH!!
    // this is how you would free memory, if you needed to, but we keep needing
    // the audio buffer so lets hang onto it. FOREVER!!!
    //Module._free(dataHeap.byteOffset);
}


// all this code below sets up the web audio callback, nothing unusual here, 
// once startAudio() is called, it starts. It can't happen automatically on
// page load due to security restrictions, has to be initiated by a user 
// interaction such as a click or touch.

var audioRunning = false;
var scriptNode = null;
var source = null;
var audioCtx = null;



function setupAudio(fn) {
	// Create AudioContext and buffer source
	var AudioContext = window.AudioContext // Default
	|| window.webkitAudioContext // Safari and old versions of Chrome
	|| false; 

	if (!AudioContext) {
	    // Do whatever you want using the Web Audio API
	    alert("Sorry, but the Web Audio API is not supported by your browser. Please, consider upgrading to the latest version or downloading Google Chrome or Mozilla Firefox");
	}
	audioCtx = new AudioContext();
	source = audioCtx.createBufferSource();

	// buff size, ins, outs
	scriptNode = audioCtx.createScriptProcessor(1024, 0, 2);

	scriptNode.onaudioprocess = function(audioProcessingEvent) {
		fn(audioProcessingEvent.outputBuffer.getChannelData(0), audioProcessingEvent.outputBuffer.getChannelData(1)); 
	};
}

function startAudio() {
	if(audioRunning) return;
	setupAudio(audioCallback);
	scriptNode.connect(audioCtx.destination);
	source.start();
	audioRunning = true;
	document.getElementById("startStop").innerHTML = "stop &#x23F9;";
	document.getElementById("startStop").href = "javascript: stopAudio();";
}

function stopAudio() {
	audioRunning = false;
	audioCtx.suspend().then(function() { 
		document.getElementById("startStop").innerHTML = "start &#x25b6;";
		document.getElementById("startStop").href = "javascript: startAudio();";
	});

// <a id="startStop" href="javascript: startAudio()">start &#x25b6;</a>
}


