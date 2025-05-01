#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../include/collision_detection.h"
#include "../include/game_logic.h"

// 测试辅助函数：打印测试结果
void print_test_result(const char* test_name, bool passed) {
    printf("%s: %s\n", test_name, passed ? "PASSED" : "FAILED");
}

// 测试 AABB 碰撞检测
void test_aabb_collision() {
    bounding_box_t box1 = {
        .x = 0,
        .y = 0,
        .width = 10,
        .height = 10
    };

    // 测试用例1：重叠的盒子
    bounding_box_t box2 = {
        .x = 5,
        .y = 5,
        .width = 10,
        .height = 10
    };
    bool result1 = check_aabb_collision(&box1, &box2);
    print_test_result("AABB Collision - Overlapping", result1);

    // 测试用例2：不重叠的盒子
    box2.x = 20;
    box2.y = 20;
    bool result2 = check_aabb_collision(&box1, &box2);
    print_test_result("AABB Collision - Non-overlapping", !result2);

    // 测试用例3：边界接触
    box2.x = 10;
    box2.y = 10;
    bool result3 = check_aabb_collision(&box1, &box2);
    print_test_result("AABB Collision - Touching", result3);
}

// 测试角色与地图边界碰撞
void test_map_boundary_collision() {
    character_t character = {
        .x = 0,
        .y = 0,
        .width = 16,
        .height = 16
    };

    // 测试用例1：在地图内
    bool result1 = check_map_boundary_collision(&character);
    print_test_result("Map Boundary - Inside map", !result1);

    // 测试用例2：超出左边界
    character.x = -1;
    bool result2 = check_map_boundary_collision(&character);
    print_test_result("Map Boundary - Left boundary", result2);

    // 测试用例3：超出右边界
    character.x = MAP_WIDTH * TILE_SIZE + 1;
    bool result3 = check_map_boundary_collision(&character);
    print_test_result("Map Boundary - Right boundary", result3);
}

// 测试角色与危险物碰撞
void test_hazard_collision() {
    character_t fireboy = {
        .x = 16,
        .y = 16,
        .width = 16,
        .height = 16,
        .type = CHARACTER_FIREBOY
    };

    character_t watergirl = {
        .x = 16,
        .y = 16,
        .width = 16,
        .height = 16,
        .type = CHARACTER_WATERGIRL
    };

    // 测试用例1：火男孩与水的碰撞
    bool result1 = check_hazard_collision(&fireboy, TILE_WATER);
    print_test_result("Hazard Collision - Fireboy with water", result1);

    // 测试用例2：水女孩与火的碰撞
    bool result2 = check_hazard_collision(&watergirl, TILE_FIRE);
    print_test_result("Hazard Collision - Watergirl with fire", result2);

    // 测试用例3：任意角色与毒药的碰撞
    bool result3 = check_hazard_collision(&fireboy, TILE_POISON);
    bool result4 = check_hazard_collision(&watergirl, TILE_POISON);
    print_test_result("Hazard Collision - Characters with poison", result3 && result4);
}

// 测试碰撞穿透深度计算
void test_penetration_depth() {
    bounding_box_t box1 = {
        .x = 0,
        .y = 0,
        .width = 10,
        .height = 10
    };

    bounding_box_t box2 = {
        .x = 5,
        .y = 5,
        .width = 10,
        .height = 10
    };

    float penetration_x, penetration_y;
    calculate_penetration_depth(&box1, &box2, &penetration_x, &penetration_y);

    // 检查穿透深度是否合理
    bool valid_penetration = (penetration_x != 0 || penetration_y != 0) &&
                           (fabs(penetration_x) <= box1.width + box2.width) &&
                           (fabs(penetration_y) <= box1.height + box2.height);
    
    print_test_result("Penetration Depth Calculation", valid_penetration);
}

// 测试碰撞响应
void test_collision_resolution() {
    character_t character = {
        .x = 50,
        .y = 50,
        .width = 16,
        .height = 16,
        .velocity_x = 10,
        .velocity_y = 10,
        .on_ground = false,
        .state = STATE_FALLING
    };

    // 测试向下碰撞（落地）
    resolve_collision(&character, 0, -5);
    bool ground_collision = character.on_ground && 
                          character.velocity_y == 0 && 
                          character.state == STATE_IDLE;
    print_test_result("Collision Resolution - Ground collision", ground_collision);

    // 测试水平碰撞
    character.velocity_x = 10;
    resolve_collision(&character, 5, 0);
    bool wall_collision = character.velocity_x == 0;
    print_test_result("Collision Resolution - Wall collision", wall_collision);
}

int main() {
    printf("Starting collision detection tests...\n\n");

    test_aabb_collision();
    printf("\n");
    
    test_map_boundary_collision();
    printf("\n");
    
    test_hazard_collision();
    printf("\n");
    
    test_penetration_depth();
    printf("\n");
    
    test_collision_resolution();
    printf("\n");

    printf("All tests completed.\n");
    return 0;
} 