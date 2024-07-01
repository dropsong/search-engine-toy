#!/bin/bash

# 定义配置文件路径
output_config_file="../conf/wordReco_zh_res_wordFreq.txt"
bin_dir="../bin"
input_dir="../data/zh_wordReco_rawData"
output_file_name="tmpSet.dat"

# 读取配置文件获取输出文件路径
if [ ! -f "$output_config_file" ]; then
    echo "Config file $output_config_file does not exist."
    exit 1
fi

output_dir=$(cat "$output_config_file")

# 检查输出目录是否存在，如果不存在则创建
if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
fi

output_file="$output_dir/$output_file_name"

# 收集所有不以“0”开头的输入文件
input_files=()
for file in "$input_dir"/*; do
    filename=$(basename "$file")
    if [[ ! "$filename" =~ ^0 ]]; then
        input_files+=("$file")
    fi
done

# 检查是否有输入文件
if [ ${#input_files[@]} -eq 0 ]; then
    echo "No input files to process."
    exit 1
fi

# 执行 zhWordFreq 程序
"$bin_dir/zhWordFreq" "$output_file" "${input_files[@]}"

echo "Processing completed. Output file is located at $output_file."

# 重命名所有不以“0”开头的文件
for file in "${input_files[@]}"; do
    filename=$(basename "$file")
    mv "$file" "$input_dir/0$filename"
    echo "Renamed $filename to 0$filename"
done

echo "Renaming completed."
