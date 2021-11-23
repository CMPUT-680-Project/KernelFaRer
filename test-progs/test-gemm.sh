#!/bin/bash
clang -Wall -O3 -g -mllvm --enable-kernel-replacer -mllvm --gemmfarer-replacement-mode=cblas-interface -mllvm --debug-only=gemm-replacer-pass -S -emit-llvm gemm-0.cc