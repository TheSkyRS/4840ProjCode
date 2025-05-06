// game_logic.h
// 游戏逻辑主接口，负责主循环调度与全局状态控制

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "character.h"     // 引入角色模块定义
#include "sprite_object.h" // 引入游戏物体模块定义
#include <stdbool.h>

// 游戏整体状态（可扩展用于暂停、胜利、失败等）
typedef enum
{
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_WIN
} game_state_t;

// 更新一帧的完整游戏状态（角色、物体、环境）
void update_game_state(character_t *characters, int num_characters,
                       object_t *objects, int num_objects,
                       float delta_time);

// 可选函数：用于切换关卡状态、同步全局参数到硬件（如灯光、计分等）
void sync_game_state_to_hardware(game_state_t state);

#endif // GAME_LOGIC_H
