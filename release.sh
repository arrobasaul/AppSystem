#! /bin/sh
cmake -S . -B out/release -DCMAKE_INSTALL_PREFIX=run/ ;
cd out/release ; 
cmake --release . ; 
cmake --build . --target install