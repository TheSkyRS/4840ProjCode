/**
 * @file joypad_test.c
 * @brief Joypad Input Test Program
 *
 * This program demonstrates how to use the joypad_input module, including
 * initializing controllers, dynamically inserting controller devices,
 * and reading controller input to control game characters.
 * Supports the classic 8-button joypad layout with continuous press tracking.
 */


#include "../include/joypad_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/joystick.h>
#include <stdbool.h>  // 添加对bool类型的支持

/* 游戏动作常量定义 */
typedef enum {
    ACTION_NONE = 0,      // 无动作
    ACTION_MOVE_LEFT = 1, // 向左移动
    ACTION_MOVE_RIGHT = 2,// 向右移动
    ACTION_JUMP = 3       // 跳跃动作
} game_action_t;

/* Classic joypad button definitions if not already defined */
#ifndef JOYPAD_BTN_UP
#define JOYPAD_BTN_UP 0    // Up direction button
#define JOYPAD_BTN_DOWN 1  // Down direction button
#define JOYPAD_BTN_LEFT 2  // Left direction button
#define JOYPAD_BTN_RIGHT 3 // Right direction button
#define JOYPAD_BTN_A 4     // A button
#define JOYPAD_BTN_B 5     // B button
#define JOYPAD_BTN_X 6     // X button
#define JOYPAD_BTN_Y 7     // Y button
#endif

/* Total number of buttons supported */
#define JOYPAD_BUTTON_COUNT 8

extern int insert_joypad(const char *device_path, int player_index);
extern int input_handler_init(void);
extern void input_handler_cleanup(void);
extern game_action_t get_player_action(int player_index);
extern int is_joypad_connected(int player_index);
extern int get_joypad_button_state(int player_index, int button_id);

/* Global variables */
static volatile int running = 1;

/**
 * @brief Signal handler function, for capturing Ctrl+C
 */
void handle_signal(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nCaught Ctrl+C, exiting...\n");
        running = 0;
    }
}

/**
 * @brief Print player action information
 */
void print_action(int player_index, game_action_t action)
{
    printf("Player %d: ", player_index + 1);

    switch (action)
    {
    case ACTION_NONE:
        printf("No action");
        break;
    case ACTION_MOVE_LEFT:
        printf("Moving left");
        break;
    case ACTION_MOVE_RIGHT:
        printf("Moving right");
        break;
    case ACTION_JUMP:
        printf("Jumping");
        break;
    default:
        printf("Unknown action");
        break;
    }
    printf("\n");
}

/**
 * @brief Display button state
 */
void print_button_state(int player_index, int button_id, const char *button_name)
{
    int state = get_joypad_button_state(player_index, button_id);

    if (state)
    {
        printf("    %s: PRESSED", button_name);
    }
    else
    {
        printf("    %s: released", button_name);
    }
}

/**
 * @brief Format button state for compact display
 */
const char *format_button_state(int player_index, int button_id)
{
    static char buffer[20];
    int state = get_joypad_button_state(player_index, button_id);

    if (state)
    {
        sprintf(buffer, "PRESSED");
    }
    else
    {
        sprintf(buffer, "released");
    }

    return buffer;
}

/**
 * @brief Display joypad connection status and all button states
 */
void show_joypad_status()
{
    printf("\n--- Joypad Connection Status ---\n");

    for (int i = 0; i < 2; i++)
    {
        printf("Player %d Joypad: ", i + 1);

        if (is_joypad_connected(i))
        {
            printf("CONNECTED\n");

            // Display button states
            printf("  Direction buttons (left side):\n");
            print_button_state(i, JOYPAD_BTN_UP, "UP");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_DOWN, "DOWN");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_LEFT, "LEFT");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_RIGHT, "RIGHT");
            printf("\n");

            printf("  Function buttons (right side):\n");
            print_button_state(i, JOYPAD_BTN_A, "A");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_B, "B");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_X, "X");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_Y, "Y");
            printf("\n");
        }
        else
        {
            printf("NOT CONNECTED\n");
        }
        printf("\n");
    }
}

/**
 * @brief Display menu and handle user input
 */
void show_menu()
{
    printf("\n--- Joypad Test Menu ---\n");
    printf("1. Connect Player 1 Joypad\n");
    printf("2. Connect Player 2 Joypad\n");
    printf("3. Test Joypad Input\n");
    printf("4. Test Button Press\n");
    printf("5. Show Joypad Status\n");
    printf("6. Scan Input Devices\n");
    printf("7. Raw Signal Monitor\n");
    printf("0. Exit Program\n");
    printf("Select: ");
}

/**
 * @brief Test button press functionality
 */
void test_continuous_press()
{
    printf("Starting button press test, press Ctrl+C to exit test mode\n");
    printf("Press buttons to see state\n\n");

    while (running)
    {
        // Clear screen
        printf("\033[2J\033[H");
        printf("=== Button Press Test ===\n");
        printf("Press buttons to see their state\n");
        printf("Press Ctrl+C to exit test mode\n\n");

        // Display joypad connection status
        for (int i = 0; i < 2; i++)
        {
            printf("Player %d Joypad: %s\n",
                   i + 1,
                   is_joypad_connected(i) ? "CONNECTED" : "NOT CONNECTED");

            if (is_joypad_connected(i))
            {
                printf("  Button States:\n");

                // 使用基本的按钮状态显示
                printf("\n  Direction Pad:     Function Buttons:\n");
                printf("       [%s]           [%s] [%s]\n",
                       format_button_state(i, JOYPAD_BTN_UP),
                       format_button_state(i, JOYPAD_BTN_Y),
                       format_button_state(i, JOYPAD_BTN_X));

                printf("  [%s] [%s]      [%s] [%s]\n",
                       format_button_state(i, JOYPAD_BTN_LEFT),
                       format_button_state(i, JOYPAD_BTN_RIGHT),
                       format_button_state(i, JOYPAD_BTN_B),
                       format_button_state(i, JOYPAD_BTN_A));

                printf("       [%s]\n",
                       format_button_state(i, JOYPAD_BTN_DOWN));
            }
            printf("\n");
        }

        // Small delay to avoid too fast output
        usleep(50000); // 50ms
    }
}

/**
 * @brief Scan for available input devices
 */
void scan_input_devices()
{
    printf("\n--- Scanning Available Input Devices ---\n");

    // 检查常见的输入设备路径
    const char *paths[] = {
        "/dev/input/event0",
        "/dev/input/event1",
        "/dev/input/event2",
        "/dev/input/event3",
        "/dev/input/event4",
        "/dev/input/event5",
        "/dev/input/js0",
        "/dev/input/js1",
        NULL};

    int found = 0;
    for (int i = 0; paths[i] != NULL; i++)
    {
        int fd = open(paths[i], O_RDONLY | O_NONBLOCK);
        if (fd != -1)
        {
            printf("Found device: %s\n", paths[i]);

            // 根据设备路径类型尝试获取设备信息
            if (strstr(paths[i], "js") != NULL)
            {
                // JS设备信息获取
                char name[128];
                if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) != -1)
                {
                    printf("  Device name: %s\n", name);
                }
                else
                {
                    printf("  Unable to get device name: %s\n", strerror(errno));
                }
            }
            else if (strstr(paths[i], "event") != NULL)
            {
                // EVENT设备信息获取
                char name[256] = "Unknown";
                if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0)
                {
                    printf("  Device name: %s\n", name);
                }
                else
                {
                    printf("  Unable to get device name: %s\n", strerror(errno));
                }

                // 获取设备支持的事件类型
                unsigned long evbit[EV_MAX / 8 + 1];
                memset(evbit, 0, sizeof(evbit));
                if (ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), evbit) >= 0)
                {
                    printf("  Supported event types: ");
                    if (evbit[EV_KEY / 8] & (1 << (EV_KEY % 8)))
                        printf("EV_KEY ");
                    if (evbit[EV_ABS / 8] & (1 << (EV_ABS % 8)))
                        printf("EV_ABS ");
                    if (evbit[EV_REL / 8] & (1 << (EV_REL % 8)))
                        printf("EV_REL ");
                    printf("\n");
                }
            }

            close(fd);
            found++;
        }
        else
        {
            printf("Device not found: %s (%s)\n", paths[i], strerror(errno));
        }
    }

    if (found == 0)
    {
        printf("No input devices found!\n");
        printf("Check connections and permissions.\n");
    }
    else
    {
        printf("Found %d device(s).\n", found);
    }

    printf("\nPress Enter to continue...");
    getchar();
}

/**
 * @brief Monitor raw signals from a joystick device
 */
void raw_signal_monitor()
{
    char device_path[128];

    printf("\n--- Raw Signal Monitor ---\n");
    printf("Enter device path to monitor (e.g. /dev/input/js0 or /dev/input/event0): ");

    if (fgets(device_path, sizeof(device_path), stdin) == NULL)
    {
        return;
    }
    device_path[strcspn(device_path, "\n")] = 0; // Remove newline

    if (strlen(device_path) == 0)
    {
        strcpy(device_path, "/dev/input/event0"); // Default to event0
    }

    printf("Monitoring device: %s (Press Ctrl+C to exit)\n", device_path);

    int fd = open(device_path, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR: Cannot open device %s: %s\n", device_path, strerror(errno));
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    printf("Device opened successfully. Waiting for signals...\n");

    // 设置为阻塞模式以等待信号
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    // 检查是event设备还是js设备
    bool is_event_device = (strstr(device_path, "event") != NULL);

    if (is_event_device)
    {
        // 读取event设备
        struct input_event event;

        while (running)
        {
            int bytes = read(fd, &event, sizeof(event));

            if (bytes == sizeof(event))
            {
                // printf("Event: type=%d, code=%d, value=%d, time=%ld.%06ld\n",
                //        event.type, event.code, event.value,
                //        event.time.tv_sec, event.time.tv_usec);
                printf("Event: type=%d, code=%d, value=%d, time=%ld.%06ld\n",
                       event.type, event.code, event.value,
                       event.time.tv_sec, event.time.tv_usec);

                // 详细解释event字段含义
                printf("\n事件详细信息解释:\n");

                // 解释事件类型(type)
                printf("事件类型(type=%d): ", event.type);
                switch (event.type)
                {
                case EV_SYN:
                    printf("同步事件(EV_SYN)\n");
                    break;
                case EV_KEY:
                    printf("按键事件(EV_KEY)\n");
                    break;
                case EV_REL:
                    printf("相对坐标事件(EV_REL)\n");
                    break;
                case EV_ABS:
                    printf("绝对坐标事件(EV_ABS)\n");
                    break;
                case EV_MSC:
                    printf("杂项事件(EV_MSC)\n");
                    break;
                case EV_SW:
                    printf("开关事件(EV_SW)\n");
                    break;
                default:
                    printf("其他事件类型\n");
                    break;
                }

                // 根据事件类型解释事件代码(code)
                printf("事件代码(code=%d): ", event.code);
                if (event.type == EV_KEY)
                {
                    // 按键代码
                    printf("按键代码 - ");
                    switch (event.code)
                    {
                    case KEY_A:
                        printf("A键\n");
                        break;
                    case KEY_B:
                        printf("B键\n");
                        break;
                    case KEY_X:
                        printf("X键\n");
                        break;
                    case KEY_Y:
                        printf("Y键\n");
                        break;
                    case KEY_UP:
                        printf("上方向键\n");
                        break;
                    case KEY_DOWN:
                        printf("下方向键\n");
                        break;
                    case KEY_LEFT:
                        printf("左方向键\n");
                        break;
                    case KEY_RIGHT:
                        printf("右方向键\n");
                        break;
                    default:
                        printf("其他按键\n");
                        break;
                    }
                }
                else if (event.type == EV_ABS)
                {
                    // 绝对坐标
                    printf("轴代码 - ");
                    switch (event.code)
                    {
                    case ABS_X:
                        printf("X轴\n");
                        break;
                    case ABS_Y:
                        printf("Y轴\n");
                        break;
                    case ABS_Z:
                        printf("Z轴\n");
                        break;
                    case ABS_RX:
                        printf("RX轴\n");
                        break;
                    case ABS_RY:
                        printf("RY轴\n");
                        break;
                    case ABS_RZ:
                        printf("RZ轴\n");
                        break;
                    default:
                        printf("其他轴\n");
                        break;
                    }
                }
                else
                {
                    printf("值 %d\n", event.value);
                }

                // 解释事件值(value)
                printf("事件值(value=%d): ", event.value);
                if (event.type == EV_KEY)
                {
                    printf("%s\n", event.value ? "按下" : "释放");
                }
                else if (event.type == EV_ABS || event.type == EV_REL)
                {
                    printf("位置/变化量: %d\n", event.value);
                }
                else
                {
                    printf("值: %d\n", event.value);
                }

                // 打印整个event结构体的二进制内容
                unsigned char *data = (unsigned char *)&event;
                printf("十六进制: ");
                for (size_t i = 0; i < sizeof(event); i++)
                {
                    printf("%02x ", data[i]);
                }
                printf("\n================");
                printf("\n\n");
            }
            else if (bytes == -1 && errno != EAGAIN)
            {
                printf("Error reading from device: %s\n", strerror(errno));
                break;
            }
        }
    }
    else
    {
        // 读取js设备
        struct js_event event;

        while (running)
        {
            int bytes = read(fd, &event, sizeof(event));

            if (bytes == sizeof(event))
            {
                printf("Event: type=%d, value=%d, number=%d, time=%u\n",
                       event.type, event.value, event.number, event.time);

                // 解释事件类型
                if (event.type & JS_EVENT_BUTTON)
                {
                    printf("  Button %d %s\n",
                           event.number,
                           event.value ? "PRESSED" : "RELEASED");
                }
                if (event.type & JS_EVENT_AXIS)
                {
                    printf("  Axis %d position: %d\n",
                           event.number, event.value);
                }
                if (event.type & JS_EVENT_INIT)
                {
                    printf("  (Initialization event)\n");
                }
            }
            else if (bytes == -1 && errno != EAGAIN)
            {
                printf("Error reading from device: %s\n", strerror(errno));
                break;
            }
        }
    }

    close(fd);
}

/**
 * @brief Main function
 */
int main()
{
    char input[256];
    int choice;
    char device_path[128];

    // Set up signal handling
    signal(SIGINT, handle_signal);

    // Initialize input handling module
    if (input_handler_init() != 0)
    {
        fprintf(stderr, "Failed to initialize input handling module\n");
        return -1;
    }

    printf("=== Joypad Input Test Program ===\n");
    printf("This program demonstrates how to use joypads to control game characters\n");

    // Display initial joypad status
    show_joypad_status();

    while (running)
    {
        show_menu();

        // Get user input
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        choice = atoi(input);

        switch (choice)
        {
        case 0:
            running = 0;
            break;

        case 1:
            printf("Enter Player 1 joypad device path (default: /dev/input/event0): ");
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                input[strcspn(input, "\n")] = 0; // Remove newline

                // Use default value if input is empty
                if (strlen(input) == 0)
                {
                    strcpy(device_path, "/dev/input/event0");
                }
                else
                {
                    strcpy(device_path, input);
                }

                if (insert_joypad(device_path, 0) == 0)
                {
                    printf("Successfully connected Player 1 joypad\n");
                }
                else
                {
                    printf("Failed to connect Player 1 joypad: %s\n", strerror(errno));
                }
            }
            break;

        case 2:
            printf("Enter Player 2 joypad device path (default: /dev/input/event1): ");
            if (fgets(input, sizeof(input), stdin) != NULL)
            {
                input[strcspn(input, "\n")] = 0; // Remove newline

                // Use default value if input is empty
                if (strlen(input) == 0)
                {
                    strcpy(device_path, "/dev/input/event1");
                }
                else
                {
                    strcpy(device_path, input);
                }

                if (insert_joypad(device_path, 1) == 0)
                {
                    printf("Successfully connected Player 2 joypad\n");
                }
                else
                {
                    printf("Failed to connect Player 2 joypad: %s\n", strerror(errno));
                }
            }
            break;

        case 3:
            printf("Starting joypad input test, press Ctrl+C to exit test mode\n");

            // Loop to read and display joypad input
            while (running)
            {
                game_action_t action1 = get_player_action(0);
                game_action_t action2 = get_player_action(1);

                // Clear screen
                printf("\033[2J\033[H");
                printf("=== Joypad Input Test ===\n");
                printf("Press Ctrl+C to exit test mode\n\n");

                // Display joypad connection status
                for (int i = 0; i < 2; i++)
                {
                    printf("Player %d Joypad: %s\n",
                           i + 1,
                           is_joypad_connected(i) ? "CONNECTED" : "NOT CONNECTED");

                    if (is_joypad_connected(i))
                    {
                        // Display all button states
                        printf("  Direction buttons:\n");
                        printf("    UP: %s  DOWN: %s  LEFT: %s  RIGHT: %s\n",
                               get_joypad_button_state(i, JOYPAD_BTN_UP) ? "PRESSED" : "released",
                               get_joypad_button_state(i, JOYPAD_BTN_DOWN) ? "PRESSED" : "released",
                               get_joypad_button_state(i, JOYPAD_BTN_LEFT) ? "PRESSED" : "released",
                               get_joypad_button_state(i, JOYPAD_BTN_RIGHT) ? "PRESSED" : "released");

                        printf("  Function buttons:\n");
                        printf("    A: %s  B: %s  X: %s  Y: %s\n",
                               get_joypad_button_state(i, JOYPAD_BTN_A) ? "PRESSED" : "released",
                               get_joypad_button_state(i, JOYPAD_BTN_B) ? "PRESSED" : "released",
                               get_joypad_button_state(i, JOYPAD_BTN_X) ? "PRESSED" : "released",
                               get_joypad_button_state(i, JOYPAD_BTN_Y) ? "PRESSED" : "released");
                    }
                }
                printf("\n");

                // Display player actions
                printf("Game Actions:\n");
                print_action(0, action1);
                print_action(1, action2);

                // Small delay to avoid too fast output
                usleep(100000); // 100ms
            }
            break;

        case 4:
            test_continuous_press();
            break;

        case 5:
            show_joypad_status();
            break;

        case 6:
            scan_input_devices();
            break;

        case 7:
            raw_signal_monitor();
            break;

        default:
            printf("Invalid selection, please try again\n");
            break;
        }
    }

    // Clean up resources
    input_handler_cleanup();

    printf("Program has exited\n");
    return 0;
}