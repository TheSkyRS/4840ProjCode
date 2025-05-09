#ifndef VGASYS_H
#define VGASYS_H

#include <stdint.h>

// ================================
// ��ʼ��������
// ================================

/**
 * @brief ��ʼ�� VGA ϵͳ�ӿڣ��� /dev/vga_top��
 *
 * @param dev_name �豸�ڵ�·����һ��Ϊ "/dev/vga_top"
 * @return 0 = �ɹ���-1 = ʧ��
 */
int vgasys_init(const char *dev_name);

/**
 * @brief ���� VGA ϵͳ�ӿڣ��ر��豸�ļ�
 */
void vgasys_cleanup();

// ================================
// �������Թ�����д��
// ================================

/**
 * @brief ����һ�����������֣�32λ�����ʽ��
 *
 * @param enable �Ƿ���ʾ�� sprite��1=��ʾ��
 * @param flip �Ƿ�ˮƽ��ת��1=��ת��
 * @param x ��������꣨0~639��
 * @param y ���������꣨0~479��
 * @param frame ֡��ţ�0~255��
 * @return uint32_t ������ attr_word
 */
uint32_t make_attr_word(uint8_t enable, uint8_t flip,
                        uint16_t x, uint16_t y,
                        uint8_t frame);

/**
 * @brief ��Ӳ��д��ĳ����������ԣ����� ioctl��
 *
 * @param index �����ţ�0~31��
 * @param enable �Ƿ���ʾ
 * @param flip �Ƿ�ˮƽ��ת
 * @param x ���������
 * @param y ����������
 * @param frame ֡���
 */
void write_sprite(uint8_t index, uint8_t enable, uint8_t flip,
                  uint16_t x, uint16_t y, uint8_t frame);

// ================================
// ���ƼĴ���
// ================================

/**
 * @brief д����ƼĴ�����ֵ��ͨ����������ִ�У�
 *
 * @param value ����ֵ������ tilemap ID����Ƶ ID����ײ���أ�
 */
void write_ctrl(uint32_t value);

// ================================
// VGA ״̬��ȡ
// ================================

/**
 * @brief ��ȡ VGA ��ǰɨ��λ�ã�״̬�Ĵ�����
 *
 * @param col ���ص�ǰ hcount��0~639��
 * @param row ���ص�ǰ vcount��0~479��
 */
void read_status(unsigned *col, unsigned *row);

#endif // VGASYS_H
