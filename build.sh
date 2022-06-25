#!/bin/bash

function remove_binaries()
{
    local tmp=$(find . -name "*.o");
    rm ${tmp};
}

function main()
{
    local warnings="-Wall -Wextra";
    local optimization="-O2";
    local debug_lvl="-g0";
    local game_name="nidavellir";

    local c_files=$(find . -name "*.c");
    local h_files=$(find . -name "*.h");

    remove_binaries;

    for c_file in ${c_files}; do
        gcc ${warnings} ${optimization} ${debug_lvl} -c ${c_file};
    done
    
    local obj_files=$(find . -name "*.o");

    gcc -o ${game_name} ${obj_files};
}

main;