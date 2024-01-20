#!/bin/bash

function main()
{
    local warnings="-Wall -Wextra";
    local optimization="-O0";
    local debug_lvl="-g3";
    local game_name="nidavellir";
    local includes="$PWD/include/";
    local c_files=$(find -name "*.c");

		mkdir ./bin &> /dev/null;

    find ./bin -type f -name "*.o" -delete;

    for c_file in ${c_files}; do
        gcc ${warnings} ${optimization} -I "${includes}" -I "${PWD}/tools/" ${debug_info} ${debug_lvl} -c ${c_file} &
    done

    wait;

    local obj_files=$(find . -name "*.o");

    gcc -o ${game_name} ${obj_files};

    find . -name "*.o" -delete ;
}

main;
