#!/usr/bin/env bash

export LD_LIBRARY_PATH=`realpath out/lib`
./out/exe/miniat_tests
exit $?
