#!/bin/bash

# 定义输出文件路径和输入文件夹路径
output_file="../../data/web_archive/webs.dat"
input_dir="../../data/web_rawData_xmls"
bin_dir="../../bin"
page_lib_bin="$bin_dir/PageStuff/PageLib"

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

# 执行 PageLib 程序
"$page_lib_bin" "$output_file" "${input_files[@]}"

# 检查执行结果
if [ $? -ne 0 ]; then
    echo "PageLib execution failed."
    exit 1
fi

echo "PageLib executed successfully."
exit 0
