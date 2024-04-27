#!/bin/bash

function main()
{
  local debug=0;

  local game_name="nidavellir";
  local includes="$PWD/include/";
  local c_files=$(find -name "*.c");

  local copts='-funsigned-char';

  if [[ ${debug} -eq 1 ]]; then
    local debug_lvl="-g3";
    local warnings="-Wall -Wextra -Wmissing-include-dirs -Wdouble-promotion -Wignored-qualifiers -Wswitch-default -Wuninitialized";
    local optimization="-O0";
    #local aux='-aux-info aux.txt';

    local debug_opts="${debug_lvl} ${warnings} ${optimization} ${aux}";
  fi

  gcc ${debug_opts} -pedantic -I "${includes}" -I "${PWD}/tools/" ${copts} ${c_files} --output ${game_name} ;
}

main;
