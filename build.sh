#!/bin/bash

function main()
{
    local warnings="-Wall -Wextra";
    local optimization="-O0";
    local debug_lvl="-g2";
    local game_name="nidavellir";
    local includes="$PWD/include/";
    local c_files=$(find . -name "*.c");
    local debug_info="" #"-DDEBUG";

    find . -type f -name "*.o" -exec rm {} \;

    for c_file in ${c_files}; do
        gcc ${warnings} ${optimization} -I "${includes}" ${debug_info} ${debug_lvl} -c ${c_file};
    done
    
    local obj_files=$(find . -name "*.o");

    gcc -o ${game_name} ${obj_files};
}

main;
