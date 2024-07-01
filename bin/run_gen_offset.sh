#!/bin/bash

# 固定文件路径
OUTPUT_FILE="../data/web_archive/offset.dat"
INPUT_FILE="../data/web_archive/webs.dat"

# 检查输入文件是否存在
if [ ! -f "$INPUT_FILE" ]; then
    echo "Input file $INPUT_FILE does not exist."
    exit 1
fi

# 检查输出文件所在的目录是否存在，不存在则创建
OUTPUT_DIR=$(dirname "$OUTPUT_FILE")
if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir -p "$OUTPUT_DIR"
fi

# 如果输出文件不存在，则创建它
if [ ! -f "$OUTPUT_FILE" ]; then
    touch "$OUTPUT_FILE"
fi

# 执行 gen_offset 可执行文件
"./gen_offset" "$OUTPUT_FILE" "$INPUT_FILE"

# 检查 gen_offset 执行是否成功
if [ $? -eq 0 ]; then
    echo "gen_offset executed successfully."
else
    echo "gen_offset execution failed."
    exit 1
fi
