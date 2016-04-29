#!/bin/bash

(
    sudo apt-get install libglew-dev cmake libglu1-mesa-dev freeglut3-dev mesa-common-dev git xorg-dev libgl1-mesa-dev -y
    TMP_DIR=`mktemp -d`
    cd ${TMP_DIR}
    git clone https://github.com/glfw/glfw && \
    cd glfw && \
    cmake . && \
    make && \
    sudo make install
    rm -fr ${TMP_DIR}
)
