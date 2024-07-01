#!/bin/bash

# 定义输入文件目录和配置文件路径
input_dir="../data/en_wordReco_rawData"
config_file="../conf/wordReco_en_res_wordFreq.txt"
bin_dir="../bin"

# 检查输入文件目录是否存在
if [ ! -d "$input_dir" ]; then
    echo "Input directory $input_dir does not exist."
    exit 1
fi

# 读取配置文件获取输出文件目录
if [ ! -f "$config_file" ]; then
    echo "Config file $config_file does not exist."
    exit 1
fi

output_dir=$(cat "$config_file")

# 检查输出文件目录是否存在，如果不存在则创建
if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
fi

# 遍历输入文件目录下的每一个文件
for input_file in "$input_dir"/*; do
    if [ -f "$input_file" ]; then
        # 获取文件名
        filename=$(basename "$input_file")
        # 设置输出文件名
        output_file_1="1${filename%.*}dictMap.dat"
        output_file_0="0${filename%.*}dictMap.dat"
        
        # 检查输出文件目录下是否已经存在相应的 .dat 文件
        if [ -f "$output_dir/$output_file_1" ] || [ -f "$output_dir/$output_file_0" ]; then
            echo "File $output_dir/$output_file_1 or $output_dir/$output_file_0 already exists. Skipping $input_file."
        else
            # 执行 enWordFreq 程序
            "$bin_dir/enWordFreq" "$input_file" "$output_dir/$output_file_1"
        fi
    fi
done
