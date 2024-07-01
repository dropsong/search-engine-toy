#!/bin/bash

# 定义配置文件路径
wordFreq_config_file="../conf/wordReco_zh_res_wordFreq.txt"
indexTable_config_file="../conf/wordReco_zh_res_indexTable.txt"
bin_dir="../bin"
index_bin="$bin_dir/zh_index_table"

# 读取配置文件获取目录路径
if [ ! -f "$wordFreq_config_file" ]; then
    echo "Config file $wordFreq_config_file does not exist."
    exit 1
fi

if [ ! -f "$indexTable_config_file" ]; then
    echo "Config file $indexTable_config_file does not exist."
    exit 1
fi

wordFreq_dir=$(cat "$wordFreq_config_file")
indexTable_dir=$(cat "$indexTable_config_file")

# 检查目录是否存在
if [ ! -d "$wordFreq_dir" ]; then
    echo "Directory $wordFreq_dir does not exist."
    exit 1
fi

if [ ! -d "$indexTable_dir" ]; then
    mkdir -p "$indexTable_dir"
fi

# 设置输入和输出文件路径
input_file="$wordFreq_dir/oldSet.dat"
output_file="$indexTable_dir/zh_index_table.dat"

# 检查输入文件是否存在
if [ ! -f "$input_file" ]; then
    echo "Input file $input_file does not exist."
    exit 1
fi

# 如果输出文件不存在，先创建它
if [ ! -f "$output_file" ]; then
    touch "$output_file"
fi

# 执行 zh_index_table 程序
"$index_bin" "$input_file" "$output_file"

# 检查执行结果
if [ $? -ne 0 ]; then
    echo "zh_index_table execution failed."
    exit 1
fi

echo "zh_index_table executed successfully."
exit 0
