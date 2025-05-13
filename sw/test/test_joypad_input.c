/**
 * @file test_joypad_input.c
 * @brief 手柄输入设备驱动程序的测试文件
 *
 * 此文件提供了对游戏手柄输入设备驱动程序的测试功能，
 * 测试包括初始化、按键检测和资源清理等功能。
 */

#include "../include/joypad_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 全局变量，用于中断处理
volatile int keep_running = 1;

/**
 * @brief 处理CTRL+C信号，优雅地结束测试
 */
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("接收到中断信号，正在退出...\n");
        keep_running = 0;
    }
}

/**
 * @brief 测试手柄连接状态
 */
void test_joypad_connection() {
    printf("===== 手柄连接状态测试 =====\n");
    
    for (int i = 0; i < 2; i++) {
        if (is_joypad_connected(i)) {
            printf("玩家%d手柄: 已连接\n", i + 1);
        } else {
            printf("玩家%d手柄: 未连接\n", i + 1);
            printf("默认设备路径: %s\n", get_default_joypad_path(i));
        }
    }
    printf("\n");
}

/**
 * @brief 测试手柄按键状态
 */
void test_joypad_buttons() {
    printf("===== 手柄按键状态测试 =====\n");
    printf("请按下手柄按键，程序将显示按键状态...\n");
    printf("按下Ctrl+C停止测试\n\n");
    
    // 注册信号处理器
    signal(SIGINT, handle_signal);
    
    while (keep_running) {
        // 清除屏幕
        printf("\033[H\033[J");
        printf("手柄按键状态测试 (Ctrl+C退出)\n");
        printf("-------------------------\n");
        
        for (int player = 0; player < 2; player++) {
            if (is_joypad_connected(player)) {
                printf("玩家%d手柄状态:\n", player + 1);
                printf("  方向键: 上[%s] 下[%s] 左[%s] 右[%s]\n",
                       get_joypad_button_state(player, JOYPAD_BTN_UP) ? "按下" : "释放",
                       get_joypad_button_state(player, JOYPAD_BTN_DOWN) ? "按下" : "释放",
                       get_joypad_button_state(player, JOYPAD_BTN_LEFT) ? "按下" : "释放",
                       get_joypad_button_state(player, JOYPAD_BTN_RIGHT) ? "按下" : "释放");
                
                printf("  功能键: A[%s] B[%s] X[%s] Y[%s]\n",
                       get_joypad_button_state(player, JOYPAD_BTN_A) ? "按下" : "释放",
                       get_joypad_button_state(player, JOYPAD_BTN_B) ? "按下" : "释放",
                       get_joypad_button_state(player, JOYPAD_BTN_X) ? "按下" : "释放",
                       get_joypad_button_state(player, JOYPAD_BTN_Y) ? "按下" : "释放");
                
                // 显示当前动作
                game_action_t action = get_player_action(player);
                printf("  当前动作: ");
                switch(action) {
                    case ACTION_NONE:
                        printf("无动作\n");
                        break;
                    case ACTION_MOVE_LEFT:
                        printf("向左移动\n");
                        break;
                    case ACTION_MOVE_RIGHT:
                        printf("向右移动\n");
                        break;
                    case ACTION_JUMP:
                        printf("跳跃\n");
                        break;
                    default:
                        printf("未知动作\n");
                }
            } else {
                printf("玩家%d手柄未连接\n", player + 1);
            }
            printf("\n");
        }
        
        // 延迟100毫秒
        usleep(100000);
    }
}

/**
 * @brief 测试手柄动态连接
 */
void test_joypad_connection_change() {
    printf("===== 手柄动态连接测试 =====\n");
    
    char device_path[256];
    int player_index;
    
    printf("输入手柄设备路径 (例如 /dev/input/event0): ");
    scanf("%255s", device_path);
    
    printf("输入要连接的玩家编号 (0 or 1): ");
    scanf("%d", &player_index);
    
    if (insert_joypad(device_path, player_index) == 0) {
        printf("成功连接玩家%d的手柄\n", player_index + 1);
    } else {
        printf("连接玩家%d的手柄失败\n", player_index + 1);
    }
    printf("\n");
}

/**
 * @brief 主函数
 */
int main() {
    printf("=================================\n");
    printf("手柄输入设备驱动程序测试\n");
    printf("=================================\n\n");
    
    // 初始化手柄输入模块
    if (input_handler_init() != 0) {
        printf("初始化手柄输入模块失败！\n");
        return -1;
    }
    
    // 测试手柄连接状态
    test_joypad_connection();
    
    // 测试手柄动态连接（可选）
    char choice;
    printf("是否要测试动态连接手柄？(y/n): ");
    scanf(" %c", &choice);
    if (choice == 'y' || choice == 'Y') {
        test_joypad_connection_change();
    }
    
    // 测试手柄按键状态
    choice = 0;
    printf("是否要测试手柄按键状态？(y/n): ");
    scanf(" %c", &choice);
    if (choice == 'y' || choice == 'Y') {
        test_joypad_buttons();
    }
    
    // 清理手柄输入模块
    input_handler_cleanup();
    printf("测试完成，手柄输入模块已清理\n");
    
    return 0;
} 