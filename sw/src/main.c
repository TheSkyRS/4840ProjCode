#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vgasys.h"
#include "game_logic.h"
#include "character.h"
#include "sprite_object.h"

// ����ȫ�ֽ�ɫ���������飨main �ṩ������ģ���� extern��
character_t characters[2];
int num_characters = 2;

object_t objects[4];
int num_objects = 4;

int main()
{
    // === ��ʼ�� VGA Ӳ�����豸·������ƽ̨������
    if (vgasys_init("/dev/vga_top") < 0)
    {
        fprintf(stderr, "[main] �޷��� VGA �豸���˳�����\n");
        return 1;
    }

    // === ��ʼ����ɫ
    init_character(&characters[0], 100, 300, TYPE_FIREBOY);
    init_character(&characters[1], 200, 300, TYPE_WATERGIRL);

    // === ��ʼ����ͼ���壨ʾ��Ϊ��һ�������
    memset(objects, 0, sizeof(objects));
    objects[0].x = 240;
    objects[0].y = 280;
    objects[0].frame_id = 2;

    objects[1].x = 360;
    objects[1].y = 280;
    objects[1].frame_id = 3;

    objects[2].x = 100;
    objects[2].y = 200;
    objects[2].frame_id = 4;

    objects[3].x = 300;
    objects[3].y = 180;
    objects[3].frame_id = 5;

    // === ������Ϸ��ѭ��
    run_game_loop();

    // === �����˳�ǰ�ͷ� VGA
    vgasys_cleanup();

    return 0;
}
