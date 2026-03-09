# 智能摄像头控制系统

## 项目概述

智能摄像头控制系统是一个基于C语言开发的综合性摄像头应用系统，集成了摄像头图像捕获、格式转换、AI智能问答、天气识别和LED警报控制等功能。系统支持x86和ARM架构的编译，可在不同平台上运行。

## 作者信息

- **作者**：陕西科技大学-刘延安
- **学号**：202207010724

## 功能特性

- **摄像头图像捕获**：使用V4L2接口获取摄像头图像，支持640x480分辨率，YUYV格式
- **格式转换**：将YUV422格式图像转换为JPEG格式
- **AI智能问答**：集成AI助手，支持文本和图像输入的智能问答
- **天气识别**：通过关键词匹配识别雷雨天气
- **LED警报控制**：根据天气识别结果控制LED灯的闪烁
- **友好的菜单界面**：提供直观的命令行菜单，支持中文界面

## 目录结构

```
camera/
├── bin/             # 可执行文件
│   ├── camera_capture  # 摄像头捕获程序
│   ├── yuv_to_jpeg     # YUV转JPEG程序
│   ├── menu            # 主菜单程序
│   ├── menu-arm        # ARM架构菜单程序
│   ├── thunder         # 天气识别程序
│   ├── test            # AI助手程序
│   └── test_v4l2       # V4L2测试程序
├── src/             # 源代码文件
│   ├── main.c          # 摄像头捕获源码
│   ├── yuv_to_jpeg.c   # 格式转换源码
│   ├── menu.c          # 菜单界面源码
│   ├── thunder.c       # 天气识别源码
│   └── test_v4l2.c     # V4L2测试源码
├── config/          # 配置文件
│   ├── CMakeLists.txt      # CMake构建配置
│   └── arm-toolchain.cmake # ARM交叉编译配置
├── test_files/      # 测试文件
│   ├── test.yuv     # 测试YUV图像
│   ├── test.jpg     # 测试JPEG图像
│   ├── ai_output.txt # AI输出文件
│   └── history.json  # 对话历史文件
├── output/          # 输出目录
├── program/         # 编译好的程序
│   ├── build-x86/   # x86架构程序
│   └── build-arm/   # ARM架构程序
├── led/             # LED控制
│   ├── led.c        # LED控制源码
│   ├── led          # LED控制程序
│   └── Makefile     # LED编译配置
├── libcai/          # AI库
│   ├── build/       # x86构建目录
│   └── build-arm/   # ARM构建目录
├── build/           # CMake构建目录
├── build-arm/       # ARM CMake构建目录
├── Makefile         # 主编译配置
└── README.md        # 项目文档
```

## 系统要求

### 硬件要求
- 摄像头设备（/dev/video0）
- LED设备（/sys/class/leds/led3/brightness）
- ARM设备（如树莓派）（用于ARM版本）

### 软件要求
- GCC编译器
- ARM交叉编译器（arm-linux-gnueabihf-gcc）
- CMake
- libjpeg库
- V4L2库

## 安装与编译

### 1. 安装依赖

```bash
# 安装基础依赖
sudo apt update
sudo apt install build-essential cmake libjpeg-dev

# 安装ARM交叉编译器（用于编译ARM版本）
sudo apt install gcc-arm-linux-gnueabihf
```

### 2. 编译x86版本

```bash
# 编译主程序
make

# 编译LED控制程序
cd led
make
cd ..

# 编译AI助手（在libcai目录）
cd libcai
mkdir -p build
cd build
cmake ..
make
cd ../..
```

### 3. 编译ARM版本

```bash
# 编译主程序（ARM版本）
arm-linux-gnueabihf-gcc -Wall -Wextra -o menu-arm src/menu.c
arm-linux-gnueabihf-gcc -Wall -Wextra -o thunder-arm src/thunder.c

# 编译LED控制程序（ARM版本）
cd led
arm-linux-gnueabihf-gcc -Wall -Wextra -o led-arm led.c
cd ..

# 编译AI助手（ARM版本）
cd libcai
mkdir -p build-arm
cd build-arm
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
make
cd ../..
```

## 使用方法

### 1. 运行主菜单

```bash
# 运行x86版本
cd program/build-x86
./menu

# 运行ARM版本（在ARM设备上）
cd program/build-arm
./menu
```

### 2. 菜单操作

- **1. 拍摄照片**：使用摄像头捕获图像并保存为test.yuv
- **2. 转换格式**：将test.yuv转换为test.jpg
- **3. AI助手**：与AI助手进行问答，可选择是否使用test.jpg作为图像输入
- **4. LED警报**：根据AI助手的输出识别雷雨天气并控制LED灯
- **0. 退出系统**：退出主菜单

### 3. 直接运行各模块

```bash
# 摄像头捕获
./camera_capture /dev/video0 test.yuv

# YUV转JPEG
./yuv_to_jpeg test.yuv test.jpg

# AI助手
./test "你的问题" [可选：图像文件]

# 天气识别
./thunder

# LED控制
./led 1  # 启动LED闪烁
./led 0  # 关闭LED
```

## 环境变量

- **CAI_API_KEY**：AI助手的API密钥，默认值为 `e25c2846-195e-40c8-b19c-84a388ad06e6`

## 故障排除

### 1. 摄像头无法打开
- 检查摄像头是否正确连接
- 确保用户有访问/dev/video0的权限
- 尝试使用 `v4l2-ctl --list-devices` 查看摄像头设备

### 2. LED无法控制
- 检查LED设备路径是否正确
- 确保用户有写入权限（可能需要sudo）
- 尝试手动执行 `echo 1 > /sys/class/leds/led3/brightness` 测试

### 3. AI助手无响应
- 检查网络连接
- 验证API密钥是否有效
- 查看ai_output.txt文件是否生成

### 4. 天气识别不准确
- 确保AI助手的输出包含雷雨相关关键词
- 检查thunder.c中的关键词列表是否需要更新

## 项目结构说明

### 核心模块

1. **摄像头捕获模块**（main.c）
   - 使用V4L2接口捕获图像
   - 支持640x480分辨率，YUYV格式

2. **格式转换模块**（yuv_to_jpeg.c）
   - 使用libjpeg库将YUV422转换为JPEG
   - 支持640x480分辨率

3. **菜单界面模块**（menu.c）
   - 提供用户友好的命令行界面
   - 集成所有功能模块
   - 支持超时机制，避免AI助手调用卡住

4. **天气识别模块**（thunder.c）
   - 通过关键词匹配识别雷雨天气
   - 支持中英文关键词和模糊匹配

5. **LED控制模块**（led/led.c）
   - 控制LED灯的亮灭和闪烁
   - 根据天气识别结果自动控制

## 技术细节

### V4L2摄像头捕获流程
1. 打开摄像头设备
2. 设置图像格式（640x480, YUYV）
3. 申请缓冲区
4. 开始流控制
5. 读取图像数据
6. 停止流控制
7. 释放缓冲区
8. 关闭设备

### YUV转JPEG流程
1. 打开YUV文件
2. 创建JPEG压缩对象
3. 设置压缩参数
4. 读取YUV数据并转换
5. 写入JPEG文件
6. 清理资源

### AI助手集成
1. 调用AI API获取回答
2. 解析API返回结果
3. 保存回答到ai_output.txt
4. 支持文本和图像输入

### 天气识别算法
1. 读取AI助手的输出
2. 关键词匹配（雷雨、雷暴、暴雨等）
3. 模糊匹配（"雷"和"雨"在近距离内）
4. 返回识别结果（1=雷雨，0=正常）

## 版本历史

- **v1.0**（2026-03-09）
  - 初始版本
  - 实现摄像头捕获、格式转换、AI问答、天气识别、LED控制
  - 支持x86和ARM架构
  - 友好的菜单界面

## 许可证

本项目采用MIT许可证。

## 联系方式

如有问题或建议，请联系项目维护者。