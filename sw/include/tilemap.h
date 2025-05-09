// tilemap.h
// ����ͼ��Tile Map��ģ��ͷ�ļ�

#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdbool.h>

// === ��ͼ�������� ===
#define MAP_WIDTH 40  // ��ͼ��ȣ���λ��tile��
#define MAP_HEIGHT 30 // ��ͼ�߶ȣ���λ��tile��
#define TILE_SIZE 16  // ÿ�� tile �����سߴ�

// Tile ����Լ�������ݵ�ͼ����߶��壩
#define TILE_EMPTY 0 // ��ͨ��
#define TILE_WALL 1  // ʵ��ǽ�壨����ͨ�У�
#define TILE_FIRE 2  // ��أ����к���ͨ����ˮŮ��������
#define TILE_WATER 3 // ˮ�أ�ˮŮ����ͨ�������к�������
#define TILE_EXIT 4  // �����յ�

// ��ͼ�������飨�� tilemap.c ���壩
extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];

/**
 * ���ָ�����������Ƿ���ײ�κ��赲 tile
 *
 * ������
 * - x, y          �����Ͻ����꣨��λ�����أ�
 * - width, height ������������С
 * ����ֵ��true ��ʾ����ײ��false ��ʾ����ײ
 */
bool is_tile_blocked(float x, float y, float width, float height);

#endif // TILEMAP_H
