#!/bin/bash

# 定义文件和目录路径
OUTPUT_DIR="../../data/web_archive"
OUTPUT_FILE="$OUTPUT_DIR/invertIndexTable.dat"
OFFSET_FILE="$OUTPUT_DIR/offset.dat"
WEBS_FILE="$OUTPUT_DIR/webs.dat"
EXECUTABLE="./invertIndex"

# 检查并创建数据目录
if [ ! -d "$OUTPUT_DIR" ]; then
  echo "Creating directory: $OUTPUT_DIR"
  mkdir -p "$OUTPUT_DIR"
fi

# 检查并创建输出文件
if [ ! -f "$OUTPUT_FILE" ]; then
  echo "Creating file: $OUTPUT_FILE"
  touch "$OUTPUT_FILE"
fi

# 检查 offset.dat 和 webs.dat 是否存在
if [ ! -f "$OFFSET_FILE" ]; then
  echo "Error: $OFFSET_FILE does not exist."
  exit 1
fi

if [ ! -f "$WEBS_FILE" ]; then
  echo "Error: $WEBS_FILE does not exist."
  exit 1
fi

# 执行 invertIndex 程序
if [ -x "$EXECUTABLE" ]; then
  echo "Running $EXECUTABLE with parameters:"
  echo "Output file: $OUTPUT_FILE"
  echo "Offset file: $OFFSET_FILE"
  echo "Webs file: $WEBS_FILE"
  
  $EXECUTABLE "$OUTPUT_FILE" "$OFFSET_FILE" "$WEBS_FILE"
else
  echo "Error: $EXECUTABLE is not executable or not found."
  exit 1
fi

echo "Script execution completed."
