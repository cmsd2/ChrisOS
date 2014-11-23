#!/bin/sh

git clone https://github.com/rust-lang/rust.git
cd rust && 
./configure --target=i686-unknown-linux-gnu --prefix=/usr/local/cross/ia32 &&
make
