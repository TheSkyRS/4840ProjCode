// game_logic.c
// 游戏主调度模块，实现每帧统一控制角色、物体、环境逻辑

#include "game_logic.h"    // 包含本模块头文件
#include "character.h"     // 角色更新函数
#include "sprite_object.h" // 地图物体同步函数
#include "collision.h"     // 三类碰撞逻辑
#include "input_handler.h" // 玩家输入读取

/**
 * 每帧调用：更新所有角色与地图物体的状态与显示
 *
 * 参数：
 * - characters：所有角色数组
 * - num_characters：角色数量
 * - objects：地图物体数组（钻石、机关）
 * - num_objects：地图物体数量
 * - delta_time：当前帧时间（秒）
 */
void update_game_state(character_t *characters, int num_characters,
                       object_t *objects, int num_objects,
                       float delta_time)
{
    // === 步骤 1：处理每个角色 ===
    for (int i = 0; i < num_characters; ++i)
    {
        character_t *ch = &characters[i];

        game_action_t input = get_player_action(i);                  // 读取玩家输入
        update_character_state(ch, input, delta_time);               // 更新动作状态
        handle_character_tile_collision(ch, delta_time);             // 地形碰撞处理
        handle_character_object_collision(ch, objects, num_objects); // 与物体碰撞
    }

    // === 步骤 2：角色之间的相对处理（只支持2个角色） ===
    if (num_characters >= 2)
    {
        handle_character_vs_character(&characters[0], &characters[1]);
    }

    // === 步骤 3：更新物体动画 ===
    for (int i = 0; i < num_objects; ++i)
    {
        if (!objects[i].collected)
        {
            update_object_animation(&objects[i], delta_time);
        }
    }

    // === 步骤 4：渲染同步 ===
    sync_characters_to_hardware(characters, num_characters);
    sync_objects_to_hardware(objects, num_objects, 20); // 从槽位20开始绘制物体
}

/**
 * 可选接口：根据全局状态与硬件同步，如数码管、LED指示灯等
 */
void sync_game_state_to_hardware(game_state_t state)
{
    // 当前未实现：可拓展控制计分板、生命值、机关状态等
}
