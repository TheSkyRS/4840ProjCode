#ifndef VGASYS_H
#define VGASYS_H

#include <stdint.h>

// ================================
// 初始化与清理
// ================================

/**
 * @brief 初始化 VGA 系统接口（打开 /dev/vga_top）
 *
 * @param dev_name 设备节点路径，一般为 "/dev/vga_top"
 * @return 0 = 成功，-1 = 失败
 */
int vgasys_init(const char *dev_name);

/**
 * @brief 清理 VGA 系统接口，关闭设备文件
 */
void vgasys_cleanup();

// ================================
// 精灵属性构造与写入
// ================================

/**
 * @brief 构造一个精灵属性字（32位打包格式）
 *
 * @param enable 是否显示该 sprite（1=显示）
 * @param flip 是否水平翻转（1=翻转）
 * @param x 精灵横坐标（0~639）
 * @param y 精灵纵坐标（0~479）
 * @param frame 帧编号（0~255）
 * @return uint32_t 打包后的 attr_word
 */
uint32_t make_attr_word(uint8_t enable, uint8_t flip,
                        uint16_t x, uint16_t y,
                        uint8_t frame);

/**
 * @brief 向硬件写入某个精灵的属性（调用 ioctl）
 *
 * @param index 精灵编号（0~31）
 * @param enable 是否显示
 * @param flip 是否水平翻转
 * @param x 精灵横坐标
 * @param y 精灵纵坐标
 * @param frame 帧编号
 */
void write_sprite(uint8_t index, uint8_t enable, uint8_t flip,
                  uint16_t x, uint16_t y, uint8_t frame);

// ================================
// 控制寄存器
// ================================

/**
 * @brief 写入控制寄存器的值（通常在消隐期执行）
 *
 * @param value 控制值（例如 tilemap ID、音频 ID、碰撞开关）
 */
void write_ctrl(uint32_t value);

// ================================
// VGA 状态读取
// ================================

/**
 * @brief 读取 VGA 当前扫描位置（状态寄存器）
 *
 * @param col 返回当前 hcount（0~639）
 * @param row 返回当前 vcount（0~479）
 */
void read_status(unsigned *col, unsigned *row);

#endif // VGASYS_H
