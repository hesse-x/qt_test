#!/bin/bash

format() {
  local target_directory="$1"
  find $target_directory -type d | while read -r folder; do
    # .h
    find "$folder" -maxdepth 1 -type f -name '*.h' | while read -r file; do
      echo $file
      clang-format -i $file        
    done
    # .cpp
    find "$folder" -maxdepth 1 -type f -name '*.cpp' | while read -r file; do
      echo $file
      clang-format -i $file        
    done

  done
}

format $1
