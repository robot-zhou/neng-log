#!/bin/bash

SCRIPTFILE=$(readlink -f $0)
SCRIPTNAME=$(basename ${SCRIPTFILE})
SCRIPTDIR=$(dirname ${SCRIPTFILE})

function usage() {
    echo "usage: ${SCRIPTNAME} [options] -f|--file <objfile>"
    echo "usage: ${SCRIPTNAME} [options] -d|--dir  <objdir>"
    echo "usage: ${SCRIPTNAME} --dbg-package=<debian debug package name>"
    echo ""
    echo "  options:"
    echo "    -k|--keep-debug,          keep debug symbol in object file"
    echo "    -o|--output <directory>,  output debug symbol file to directory, default: directory of object file"
    echo "    -h|--help,                show this message"
    echo ""
}

while [ $# -gt 0 ]; do
    case $1 in
    --dbg-package=*)
        pkgName=${1##--dbg-package=}
        objDir=${pkgName%%-dbg}
        if [ -z "${objDir}" -o -z "${pkgName}" ]; then
            echo "Error: package cannot empty"
            exit 1
        fi
        objDir="debian/${objDir}"
        if [ ! -d "${objDir}" ]; then 
            echo "Error: invalid objdir argument, not directory"
            exit 1
        fi
        outputDir="debian/${pkgName}"
        shift 1
        ;;
    -d | --dir)
        if [ -z "$2" ]; then
            echo "Error: missing object directory argument"
            usage
            exit 1
        fi
        objDir=$2
        if [ ! -d "${objDir}" ]; then
            echo "Error: invalid objdir argument, not directory"
            exit 1
        fi
        shift 2
        ;;
    -f | --file)
        if [ -z "$2" ]; then
            echo "Error: missing object file argument"
            usage
            exit 1
        fi
        objFile=$2
        if [ ! -f "${objFile}" ]; then
            echo "Error: invalid objfile argument, not file"
            exit 1
        fi
        shift 2
        ;;
    -k | --keep-debug)
        keepDebug=true
        shift
        ;;
    -o | --output)
        if [ -z "$2" ]; then
            echo "Error: missing object file argument"
            usage
            exit 1
        fi
        outputDir=$2
        shift 2
        ;;
    -h | --help)
        usage
        exit 0
        ;;
    *)
        echo "Error: invalide argument: $@"
        usage
        exit 1
        ;;
    esac
done

if [ -z "$objFile" -a -z "${objDir}" ]; then
    echo "Error: <objfile> and <objdir> cannot be empty at the same time"
    usage
    exit 1
fi

if [ -n "$objFile" -a -n "${objDir}" ]; then
    echo "Error: <objfile> and <objdir> cannot be not empty at the same time"
    usage
    exit 1
fi

function strip_test() {
    local objFile=$1
    local objType=$(file ${objFile})

    if [[ "${objType}" =~ .*ELF.*(executable|shared).*not\ stripped.* ]]; then
        return 0
    fi

    echo "@@ strip test error @@: ${objFile} type: ${objType}"
    return 1
}

function strip_file() {
    local locSrcFile=$1
    local locDstDir=$2
    local locKeepDebug=$3
    local locSrcDir=$(dirname ${locSrcFile})
    local objName=$(basename ${locSrcFile})

    cd ${locSrcDir} || exit 21
    objcopy --only-keep-debug "${objName}" "${objName}.debug" || exit 21
    if [ -z "${locKeepDebug}" ]; then
        objcopy --strip-all "${objName}" || exit 22
        objcopy --add-gnu-debuglink="${objName}.debug" "${objName}" || exit 23
    fi
    cd -

    if [ -n "${locDstDir}" ]; then
        if [ ! -d "${locDstDir}" ] && ! mkdir -p "${locDstDir}"; then
            rm -f "${locSrcFile}.debug"
            return 24
        fi

        if ! mv -f "${locSrcFile}.debug" "${locDstDir}/"; then
            rm -f "${locSrcFile}.debug"
            return 25
        fi

        locDstFile="${locDstDir%%/}/${objName}.debug"
    else
        locDstFile="${locSrcFile}.debug"
    fi

    echo "@@ strip file success @@: ${locSrcFile} => ${locDstFile} KeepDebug: ${locKeepDebug}"
}

if [ -n "${objFile}" ]; then
    strip_test ${objFile} || exit 1
    strip_file "${objFile}" "${outputDir}" "${keepDebug}"
elif [ -n "${objDir}" ]; then
    find ${objDir} -type f | while read ITEM; do
        strip_test "${ITEM}" >/dev/null 2>&1 || continue
        if [ -n "${outputDir}" ]; then
            itemSubDir=$(dirname ${ITEM})
            itemSubDir=${itemSubDir##${objDir}}
            itemOutputDir=${outputDir%%/}/${itemSubDir##/}
            itemOutputDir=${itemOutputDir%%/}
        fi
        # echo "${ITEM} => ${itemOutputDir}"
        strip_file "${ITEM}" "${itemOutputDir}" "${keepDebug}"
    done
fi
