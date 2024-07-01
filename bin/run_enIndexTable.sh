#!/bin/bash

# 定义配置文件路径
wordFreq_config_file="../conf/wordReco_en_res_wordFreq.txt"
indexTable_config_file="../conf/wordReco_en_res_indexTable.txt"
bin_dir="../bin"

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
output_dir="$indexTable_dir"

# 检查输入文件是否存在
if [ ! -f "$input_file" ]; then
    echo "Input file $input_file does not exist."
    exit 1
fi

# 执行 en_index_table 程序
"$bin_dir/en_index_table" "$input_file" "$output_dir/en_index_table.dat"
