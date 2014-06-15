#!/bin/bash

DIR="$( cd "$( dirname "$0" )" && pwd )"

cd $DIR/..

find -E `pwd` -type f  \
    -regex '.*\.(cpp|h)' -and \
    -not -path '*.git' -and \
    -not -path '*.alt/*' -and \
    -not -path '*/build/*' \
    > ~/cscope.files

cd ~ #the directory with 'cscope.files' in it
cscope -b -q -k
