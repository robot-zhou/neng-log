#!/bin/bash

function test_file() {
    local objFile=$1
    local objType=$(file ${objFile})

    if [[ "$objType" =~ .*ELF.*(executable|shared).*not\ stripped.* ]]; then
        return 0
    fi

    exit 10
}

function strip_file() {
    local objFile=$1
    local objDir=$(dirname $objFile)
    local objName=$(basename $objFile)

    cd "${objDir}" || exit 20
    objcopy --only-keep-debug "${objName}" "${objName}.debug" || exit 21
    objcopy --strip-all "${objName}" || exit 22
    objcopy --add-gnu-debuglink="${objName}.debug" "${objName}" || exit 23
}

[ -n "$1" ] || exit 1
test_file $1
strip_file $1
echo "== neng-strip file success: $1"
