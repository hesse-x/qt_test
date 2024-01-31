#!/bin/bash

# 搜索所有子文件夹中的文件

pwd=`pwd`
output_file="$pwd/qt_iwyu.imp"

iwyu_map_head() {
  echo "[" > $output_file
}


iwyu_map_end() {
  echo "]" >> $output_file
}

# 遍历所有子文件夹
gen_map() {
  local target_directory="$1"
  find $target_directory -type d | while read -r folder; do
      # 在每个子文件夹中找到配对的文件
      find "$folder" -maxdepth 1 -type f -name '*.h' | while read -r file; do
          # 提取文件名（不包括路径和扩展名）
          filename=$(basename "$file" .h)
          
          # 将文件名转换为小写
          lowercase_filename=$(echo "$filename" | tr '[:upper:]' '[:lower:]')

          # 构建相对路径和另一个文件的全名
          counterpart="${folder}/${lowercase_filename}"
          counterpart="${counterpart%.h}"
          
          # 检查另一个文件是否存在（不区分大小写）
          counterpart_match=$(find "$folder" -maxdepth 1 -type f -iname "$lowercase_filename" | head -n 1)

          if [ -n "$counterpart_match" ]; then
              echo "{ include: [ \"\\\"$(basename ${file})\\\"\", \"private\", \"<${counterpart_match}>\", \"public\" ] }," >> $output_file
              echo "{ include: [ \"<$(basename ${file})>\", \"private\", \"<${counterpart_match}>\", \"public\" ] }," >> $output_file
          fi
      done
  done
}

iwyu_map_head
cd /usr/include/x86_64-linux-gnu/qt5
gen_map "QtCore"
gen_map "QtGui"
gen_map "QtWidgets"
iwyu_map_end
