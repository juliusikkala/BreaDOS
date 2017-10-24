#! /bin/bash

cd "${0%/*}"

function require_programs {
    for program in "$@"
    do
        if ! [ -x "$(command -v $program)" ]
        then
            echo "Error: $program is required for building BreaDOS" >&2
            exit 1
        fi
    done
}
require_programs wget make gcc as ar objcopy ld strip meson automake

tools/setup.sh
rm -rf build
meson build --cross-file cross_file.txt
