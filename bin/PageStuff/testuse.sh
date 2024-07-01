#!/bin/bash

# 定义路径
output_file="../../data/web_archive/webs.dat"
input_dir="../../data/web_rawData_xmls"
bin_dir="../../bin/PageStuff"
page_lib_bin="$bin_dir/PageLib"

# 创建输出文件所在的目录（如果不存在）
output_dir=$(dirname "$output_file")
if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
fi

# 创建输出文件（如果不存在）
if [ ! -f "$output_file" ]; then
    touch "$output_file"
fi

# 获取输入文件夹下的所有文件
input_files=("$input_dir"/*)

# 检查输入文件夹是否为空
if [ ${#input_files[@]} -eq 0 ]; then
    echo "No input files found in $input_dir"
    exit 1
fi

# 处理每个输入文件
for input_file in "${input_files[@]}"; do
    if [ -f "$input_file" ]; then
        "$page_lib_bin" "$output_file" "$input_file"
        if [ $? -ne 0 ]; then
            echo "PageLib encountered an error with file: $input_file"
        fi
    fi
done

echo "All files processed successfully."
exit 0
