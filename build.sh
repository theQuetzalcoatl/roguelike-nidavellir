#!/bin/bash

function main()
{
    local warnings="-Wall -Wextra";
    local optimization="-O0";
    local debug_lvl="-g2";
    local game_name="nidavellir";

    local c_files=$(find . -name "*.c");

    find . -type f -name "*.o" -exec rm {} \;

    for c_file in ${c_files}; do
        gcc ${warnings} ${optimization} ${debug_lvl} -c ${c_file};
    done
    
    local obj_files=$(find . -name "*.o");

    gcc -o ${game_name} ${obj_files};
}

main;
