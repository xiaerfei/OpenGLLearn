#!/usr/bin/env bash
#
# bootstrap.sh —— 一键生成 Xcode 工程并打开。
# 全新 clone 后只需运行：./bootstrap.sh
#
set -euo pipefail
cd "$(dirname "$0")"

# 1. 确保 XcodeGen 可用（缺失则尝试用 Homebrew 安装）
if ! command -v xcodegen >/dev/null 2>&1; then
    echo "未找到 XcodeGen。"
    if command -v brew >/dev/null 2>&1; then
        echo "正在用 Homebrew 安装 XcodeGen…"
        brew install xcodegen
    else
        echo "错误：未检测到 Homebrew。请先安装 Homebrew (https://brew.sh)，" >&2
        echo "或手动安装 XcodeGen (https://github.com/yonaskolb/XcodeGen)，然后重试。" >&2
        exit 1
    fi
fi

# 2. 从 project.yml 生成工程
echo "正在生成 LearnOpenGL.xcodeproj…"
xcodegen generate

# 3. 打开
echo "完成 ✅ 正在打开 Xcode…"
open LearnOpenGL.xcodeproj
