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
    if [[ ${RESULT} != 0 ]]; then
        echo "$1 return ERROR ${RESULT}"
    fi
    # gdb and return
    if [[ ${RESULT} != 0 ]]; then
        gdb $1 core -ex "thread apply all bt" -ex "set pagination 0" -batch
        return ${RESULT}
    fi
}
