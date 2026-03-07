# 机房助手

这是一个用于对抗极域机房管理助手的小软件。

## 项目架构

目前只有一个文件，可以用来杀掉极域，双击运行即可

## 下载

- [64 位版本](https://github.com/andrewkapok/jiyuhelper/releases/latest/download/MemoryErasure-win64.exe)
- [32 位版本](https://github.com/andrewkapok/jiyuhelper/releases/latest/download/MemoryErasure-win32.exe)

## 使用方法

1. 下载对应架构的版本
3. 程序会自动查找并终止 StudentMain.exe 进程

## 构建

```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## 许可证

MIT License
