#!/bin/sh
# My assignment 1: Writer script.
# Author: Juan Gomez.

if [ $# != 2 ]
then
    # echo JG: Number of arguments is not equal to 2.
    exit 1
fi

# We have all we need to proceed.
writefile=$1
writestr=$2

curr_dir= pwd

# Check if file exists.
if [ ! -f "$writefile" ]
then
    # File does not exist.
    # echo JG: File does not exist
    # echo JG: $writefile
    dir_target="$(dirname "${writefile}")"
    dir_target="${dir_target}/"
    file_target="$(basename "${writefile}")"
    # echo $dir_target
    # echo $file_target
    # Check if directory exists.
    if [ ! -d "$dir_target" ]
    then
        # echo JG: Directory does not exist
        # echo JG: $dir_target
        # Create directories.
        mkdir -p $dir_target
    fi
    # Go to directory.
    cd $dir_target
    # Create file.
    touch $file_target
    if [ ! -f "$file_target" ]
    then
        # echo JG: File could not be created.
        # echo JG: $writefile
        exit 1
    fi
fi

echo $writestr > $writefile