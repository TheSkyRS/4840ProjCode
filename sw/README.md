# Forest Fire and Ice - Software Component

## 项目结构

```
forest_fire_ice/
├── Makefile          # 用于编译软件的Makefile
├── README.md         # 本文档
├── USAGE.md          # 使用说明
├── include/          # 头文件目录
│   ├── audio_control.h
│   ├── game_logic.h
│   ├── hw_interface.h
│   └── input_handler.h
├── src/              # 源代码目录
│   ├── audio_control.c
│   ├── game_logic.c
│   ├── hw_interface.c
│   ├── input_handler.c
│   └── main.c
└── build/            # 编译输出目录 (由Makefile创建)
    ├── game_sw       # 编译后的可执行文件
    └── *.o           # 编译产生的对象文件
```

## 主要模块

- **`hw_interface`**: 处理与FPGA硬件的底层通信。通过`/dev/mem`进行内存映射，提供读写控制寄存器、状态寄存器和精灵属性表的功能，并包含VBlank同步机制。
- **`input_handler`**: (占位符) 处理来自玩家的输入（预期为USB键盘）。将按键映射到游戏动作。
- **`game_logic`**: 实现游戏的核心逻辑，包括角色状态机（FSM）、物理（移动、跳跃、重力）、碰撞检测（角色与地图、角色之间）、交互机制（按钮、宝石、出口等）以及游戏状态管理。
- **`audio_control`**: 通过硬件接口触发FPGA播放指定的音效。
- **`main`**: 程序入口点，负责初始化所有模块，运行主游戏循环（固定帧率），协调游戏状态更新和硬件同步，并在结束时清理资源。

## 编译

### 依赖项

- **ARM交叉编译器**: `arm-linux-gnueabihf-gcc` (或其他适用于目标ARM平台的交叉编译器)。
- **make**: 用于执行Makefile。

### 编译步骤

1.  确保已安装`make`和`arm-linux-gnueabihf-gcc`。
    ```bash
    sudo apt-get update
    sudo apt-get install make gcc-arm-linux-gnueabihf
    ```
2.  在项目根目录 (`forest_fire_ice/`) 下运行 `make` 命令：
    ```bash
    make
    ```
3.  编译成功后，可执行文件 `game_sw` 将位于 `build/` 目录下。

### 清理编译文件

要删除 `build/` 目录下的所有编译产物，运行：
```bash
make clean
```

## 注意事项

- **硬件地址**: `hw_interface.h` 中定义的 `FPGA_MMIO_BASE` 和寄存器偏移量是示例值，**必须**根据实际FPGA设计中的Avalon总线地址映射进行修改。
- **输入处理**: `input_handler.c` 中的输入读取逻辑是占位符。需要根据实际使用的键盘和Linux系统接口（如 `libusb` 或 `evdev`）进行具体实现。
- **地图数据**: `game_logic.c` 中的地图数据是硬编码的示例。实际应用中应从文件加载关卡数据。
- **运行权限**: 由于需要访问 `/dev/mem` 进行内存映射，编译后的程序通常需要以root权限运行。

## 文档

代码中包含了详细的Doxygen风格注释。可以使用Doxygen工具生成代码文档。

