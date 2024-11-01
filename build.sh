#!/bin/bash

function main()
{

#set -x

  local debug=1;

  local game_name="nidavellir";
  local includes="$PWD/include/";
  local c_files=$(find -name "*.c");

  local copts='-funsigned-char';
  local warnings="-Wall -Wextra -Wmissing-include-dirs -Wdouble-promotion -Wignored-qualifiers -Wswitch-default -Wuninitialized";
  local additional_warnings="-Wconversion -fanalyzer"; 
	local optimization='-O2';
	local debug_lvl='-g0';

  if [[ ${debug} -eq 1 ]]; then
    local debug_lvl="-g2";
    optimization="-O0";
    #local aux='-aux-info aux.txt';
  fi

  opts="${debug_lvl} ${warnings} ${optimization} ${aux}";

  gcc ${opts} -pedantic -I "${includes}" -I "${PWD}/tools/" ${copts} ${c_files} --output ${game_name};
}

main;
