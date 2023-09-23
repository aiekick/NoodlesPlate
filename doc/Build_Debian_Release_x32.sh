 #!/bin/bash
sudo apt update
sudo apt install cmake
sudo apt install libgl1-mesa-dev libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev
sudo apt install curl libssh2-1-dev libjack-dev libasound2-dev libbz2-dev libbrotli-dev libfreetype-dev libharfbuzz-dev
mkdir build_linux
cmake -B build_linux -DCMAKE_BUILD_TYPE=Release -DCMAKE_GENERATOR_PLATFORM=x32
cmake --build build_linux --config Release

