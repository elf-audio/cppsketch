em++ emscripten/em.cpp -O3 -I../../src/ -I../../ext/rtaudio -I. -o emscripten/em.js \
-s WASM=1 \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXPORTED_FUNCTIONS="['_getSamples', '_update', '_setParameter', '_malloc', '_free']" \
-s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap','ccall']"
