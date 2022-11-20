#!/bin/bash

SCRIPT_DIR="$(dirname $0)";

cd $SCRIPT_DIR;
CPPFLAGS="$CPPFLAGS -g" make clean build/tests.run;
gdb build/tests.run;
