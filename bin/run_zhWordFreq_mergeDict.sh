#!/bin/bash

# 定义配置文件路径
config_file="../conf/wordReco_zh_res_wordFreq.txt"
bin_dir="../bin"
merge_bin="$bin_dir/merge_en_WordFreq"
output_file_name="oldSet.dat"
temp_output_file_name="tempSet.dat"
tmp_file_name="tmpSet.dat"

# 读取配置文件获取输出文件夹路径
if [ ! -f "$config_file" ]; then
    echo "Config file $config_file does not exist."
    exit 1
fi

output_dir=$(cat "$config_file")

# 检查输出目录是否存在
if [ ! -d "$output_dir" ]; then
    echo "Output directory $output_dir does not exist."
    exit 1
fi

output_file="$output_dir/$output_file_name"
temp_output_file="$output_dir/$temp_output_file_name"
tmp_file="$output_dir/$tmp_file_name"

# 检查 oldSet.dat 是否存在
if [ ! -f "$output_file" ]; then
    mv "$tmp_file" "$output_file"
    echo "No oldSet.dat found. Renamed tmpSet.dat to oldSet.dat."
    exit 0
fi

# 若 oldSet.dat 存在，则合并 tmpSet.dat 和 oldSet.dat
"$merge_bin" "$temp_output_file" "$tmp_file" "$output_file"

# 检查合并是否成功
if [ $? -ne 0 ]; then
    echo "Merging files failed."
    exit 1
fi

# 删除 tmpSet.dat 和 oldSet.dat
rm "$tmp_file"
rm "$output_file"

# 将 tempSet.dat 重命名为 oldSet.dat
mv "$temp_output_file" "$output_file"

echo "Merging completed and oldSet.dat updated."
exit 0
