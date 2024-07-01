#!/bin/bash

# 定义配置文件路径和合并程序路径
config_file="../conf/wordReco_en_res_wordFreq.txt"
bin_dir="../bin"
merge_program="$bin_dir/merge_en_WordFreq"

# 读取配置文件获取输出文件目录
if [ ! -f "$config_file" ]; then
    echo "Config file $config_file does not exist."
    exit 1
fi

output_dir=$(cat "$config_file")

# 检查输出文件目录是否存在
if [ ! -d "$output_dir" ]; then
    echo "Output directory $output_dir does not exist."
    exit 1
fi

# 初始化参数列表
params=()
output_file="$output_dir/newSet.dat"
params+=("$output_file")

# 遍历输出文件目录下的每一个文件
for file in "$output_dir"/*; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        
        if [[ "$filename" == "oldSet.dat" ]]; then
            # 如果文件名为 oldSet.dat，将它加入参数列表
            params+=("$file")
        elif [[ "$filename" == 1* ]]; then
            # 如果文件名以 1 开头，将它加入参数列表
            params+=("$file")
        fi
    fi
done

# 检查是否有文件需要合并
if [ ${#params[@]} -eq 1 ]; then
    echo "No files to merge."
    exit 0
fi

# 调用 merge_en_wordFreq 进行合并
"$merge_program" "${params[@]}"

# 检查 newSet.dat 是否生成
if [ ! -f "$output_file" ]; then
    echo "Failed to create $output_file."
    exit 1
fi

# 重命名文件
for file in "$output_dir"/*; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        
        if [[ "$filename" == 1* ]]; then
            # 将以 1 开头的文件改为以 0 开头
            new_filename="${filename/1/0}"
            mv "$file" "$output_dir/$new_filename"
        fi
    fi
done

# 删除 oldSet.dat
rm -f "$output_dir/oldSet.dat"

# 将 newSet.dat 改名为 oldSet.dat
mv "$output_file" "$output_dir/oldSet.dat"
