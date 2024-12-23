#!/bin/bash

# 检查是否提供了进程名或PID
if [ -z "$1" ]; then
    echo "Usage: $0 <process_name_or_pid>"
    exit 1
fi

# 进程名或PID
PROCESS_NAME_OR_PID=$1

# 监控的时间间隔（秒）
INTERVAL=1

# 获取进程的 PID
if [[ $PROCESS_NAME_OR_PID =~ ^[0-9]+$ ]]; then
    # 如果输入的是 PID
    PID=$PROCESS_NAME_OR_PID
else
    # 如果输入的是进程名，查找其 PID
    PID=$(pgrep -o $PROCESS_NAME_OR_PID)  # 获取最早启动的进程PID
    if [ -z "$PID" ]; then
        echo "Process '$PROCESS_NAME_OR_PID' not found."
        exit 2
    fi
fi

# 获取进程的峰值内存占用
get_peak_memory() {
    local pid=$1
    if [ -f "/proc/$pid/status" ]; then
        awk -F':\t' '/VmPeak/ {print $2}' /proc/$pid/status
    else
        echo "N/A"
    fi
}

# 将KB转为MB
convert_kb_to_mb() {
    echo "scale=2; $1 / 1024" | bc
}

# 实时监控函数
monitor_process() {
    local pid=$1
    local peak_mem=$(get_peak_memory $pid)
    local peak_mem_value

    # 提取峰值内存占用值（去除单位KB）
    peak_mem_value=$(echo $peak_mem | awk '{print $1}')
    peak_mem_value=$(convert_kb_to_mb $peak_mem_value)  # 转换为MB
    
    echo "Monitoring process with PID: $pid"
    echo "Peak memory usage: ${peak_mem_value} MB"

    # 持续输出进程的 CPU 和内存占用情况
    while true; do
        if [ ! -e /proc/$pid ]; then
            echo "Process $pid has terminated."
            exit 0
        fi

        # 获取进程的 CPU 使用率和实时内存占用 (RSS)
        cpu_mem_usage=$(ps -p $pid -o %cpu,%mem,rss --no-headers)
        if [ -z "$cpu_mem_usage" ]; then
            echo "Process $pid is not running."
            exit 1
        fi

        # 获取实时内存占用并转换为 MB
        rss_kb=$(echo $cpu_mem_usage | awk '{print $3}')
        rss_mb=$(convert_kb_to_mb $rss_kb)
        
        # 输出 CPU 使用率，内存占用 (RSS)，以及峰值内存占用
        echo "CPU Usage: $(echo $cpu_mem_usage | awk '{print $1}')% | Memory Usage (RSS): ${rss_mb} MB | Peak Memory Usage: ${peak_mem_value} MB"
        
        sleep $INTERVAL
    done
}

# 启动进程监控
monitor_process $PID

