#!/bin/bash
# arg check
function RunTest() {
    if [ "$#" -ne 1 ] || ! [ -a "$1" ]; then
        echo "Usage: $0 exe_file" >&2
        exit 1
    fi
    # exec it
    RESULT=0
    $1 || RESULT=$?
    ls -l
    if [[ ${RESULT} != 0 ]]; then
        echo "ERROR ${RESULT}"
    fi
    # gdb here
    find . -maxdepth 1 -name 'core*' -print
    # return
    if [[ ${RESULT} != 0 ]]; then
        exit ${RESULT}
    fi
}
