#!/bin/bash
# arg check
function RunTest() {
    if [ "$#" -ne 1 ] || ! [ -a "$1" ]; then
        echo "Usage: $0 exe_file" >&2
        return 1
    fi
    # exec it
    RESULT=0
    $1 || RESULT=$?
    ls -l
    if [[ ${RESULT} != 0 ]]; then
        echo "ERROR ${RESULT}"
    fi
    # gdb here
    gdb $1 core -ex "thread apply all bt" -ex "set pagination 0" -batch
    # return
    if [[ ${RESULT} != 0 ]]; then
        return ${RESULT}
    fi
}
