#!/bin/sh
# My assignment 1: Finder script.
# Author: Juan Gomez.

if [ $# != 2 ]
then
    # echo JG: Number of arguments is not equal to 2
    exit 1
fi

# We have all we need to proceed.
filesdir=$1
searchstr=$2

if [ ! -d "$filesdir" ]
then
    # echo JG: Directory does not exist
    exit 1
fi

# Now we need to find the total number of files in the directory and subdirectories.
num_files=$(find $filesdir -type f | wc -l)

# Now we need to find the number of lines that contain the substring.
file_list="${filesdir}/*"
matching_lines=$(grep $searchstr $file_list | wc -l)

echo "The number of files are ${num_files} and the number of matching lines are ${matching_lines}"