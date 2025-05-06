# Forest Fire and Ice - 使用说明

本文档提供在目标硬件（如DE1-SoC）上运行"Forest Fire and Ice"游戏软件部分的说明。

## 先决条件

1.  **硬件**: DE1-SoC或其他兼容的FPGA开发板，已烧录对应的"Forest Fire and Ice"硬件设计（`.sof`文件）。
2.  **操作系统**: 开发板上的ARM处理器运行Linux系统（例如，从SD卡启动的Linux发行版）。
3.  **连接**: 通过SSH或串口连接到开发板的Linux系统终端。
4.  **输入设备**: 连接一个USB键盘到开发板的USB端口，供玩家输入。
5.  **输出设备**: 连接一个VGA显示器到开发板的VGA端口，用于显示游戏画面。
6.  **软件代码**: 将编译好的`game_sw`可执行文件（位于`build/`目录）传输到开发板的Linux系统中。

## 运行步骤

1.  **传输文件**: 使用`scp`或其他文件传输工具将`game_sw`可执行文件复制到开发板上的某个目录，例如`/home/root/`。
    ```bash
    # 在你的开发主机上执行 (假设开发板IP为 192.168.1.100)
    scp /path/to/forest_fire_ice/build/game_sw root@192.168.1.100:/home/root/
    ```

2.  **登录开发板**: 通过SSH或串口登录到开发板的Linux系统。
    ```bash
    ssh root@192.168.1.100
    ```

3.  **授予执行权限**: 在开发板上，给`game_sw`文件添加执行权限。
    ```bash
    chmod +x /home/root/game_sw
    ```

4.  **运行游戏**: **必须使用`sudo`或以root用户身份运行**，因为程序需要访问`/dev/mem`来进行内存映射。
    ```bash
    sudo /home/root/game_sw
    ```

5.  **游戏控制**:
    *   **Fireboy (玩家0)**: 使用 `W` (跳跃), `A` (左移), `D` (右移) 键。
    *   **Watergirl (玩家1)**: 使用 `↑` (向上箭头，跳跃), `←` (向左箭头), `→` (向右箭头) 键。
    *   (注意: 当前`input_handler.c`为占位符，实际按键映射需在代码中实现)。

6.  **退出游戏**: 游戏将在角色死亡或关卡完成时自动退出。你也可以在运行游戏的终端按 `Ctrl+C` 来强制终止程序。

## 故障排除

*   **无法打开 /dev/mem**: 确保你是以root权限运行程序 (`sudo ./game_sw`)。
*   **内存映射失败**: 
    *   确认`hw_interface.h`中的`FPGA_MMIO_BASE`地址与FPGA设计中的Avalon总线地址匹配。
    *   确认FPGA设计已正确加载并运行。
*   **无画面/画面异常**: 
    *   检查VGA连接。
    *   确认FPGA设计中的VGA控制器工作正常。
    *   检查`hw_interface.c`中的VBlank同步逻辑。
*   **键盘无响应**: 
    *   确认USB键盘已正确连接并被Linux系统识别（可以检查`dmesg`或`/dev/input/`下的设备）。
    *   **重要**: `input_handler.c`需要被正确实现以读取键盘事件。当前的占位符代码不会处理任何输入。
*   **游戏逻辑错误**: 检查`game_logic.c`中的状态机、物理和碰撞检测逻辑。

