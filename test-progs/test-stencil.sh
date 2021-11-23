#!/bin/bash
clang -Wall -O3 -g -mllvm --enable-stencil-finder -mllvm --debug-only=stencil-finder-pass -S -emit-llvm test.cc