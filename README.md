# 机房助手

这是一个用于对抗极域机房管理助手的小软件。

## 项目架构

1. [memoryerase](#memoryerasure)
2. [jiyuUdpAttack](#jiyuudpattack)
### MemoryErasure
这个项目可以自动杀掉极域机房小助手，采用内存覆写的方法，拥有极小的体积。

将此文件下载后双击运行即可

#### 下载

- [64 位版本](https://github.com/andrewkapok/jiyuhelper/releases/latest/download/MemoryErasure-win64.exe)
- [32 位版本](https://github.com/andrewkapok/jiyuhelper/releases/latest/download/MemoryErasure-win32.exe)

#### 编译

```bash
cd MemoryErasure
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

### JiyuUdpAttack

这个项目是使用qt重写的[ht0Ruial/Jiyu_udp_attack](https://github.com/ht0Ruial/Jiyu_udp_attack)

出于某种原因，这个项目需要一些额外的库，因此体积较大

在极域的一些较新的版本中，极域的学生端会验证udp包的安装地址。因此，在一些较新的机房中，本程序可能无法产生预期的效果
