#pragma once
#include <cstdint>


//未定
const uint32_t COLLISION_ATTRIBUTE_NONE = 0b0;

//プレイヤー陣営
const uint32_t COLLISION_ATTRIBUTE_PLAYER = 0b1;

//敵陣営
const uint32_t COLLISION_ATTRIBUTE_ENEMY = 0b1 << 1;

//敵の攻撃の当たり判定
const uint32_t COLLISION_ATTRIBUTE_ENEMY_ATTACK = 0b1 << 2;

//一発アウトの敵の攻撃の当たり判定
const uint32_t COLLISION_ATTRIBUTE_STRONG_ENEMY = 0b1 << 3;

//プレイヤー(一発アウトの敵用)
const uint32_t COLLISION_ATTRIBUTE_PLAYER2 = 0b1 << 4;

//懐中電灯
const uint32_t COLLISION_ATTRIBUTE_FLASH_LIGHT = 0b1 << 5;

//敵2(懐中電灯用)
const uint32_t COLLISION_ATTRIBUTE_ENEMY2 = 0b1 << 6;

//プレイヤー(レベルエディタのオーディオオブジェクト)
const uint32_t COLLISION_ATTRIBUTE_PLAYER3 = 0b1 << 7;

//オーディオオブジェクト(レベルエディタ)
const uint32_t COLLISION_ATTRIBUTE_AUDIO_OBJECT = 0b1 << 8;

//プレイヤー(レベルエディタのステージオブジェクト)
const uint32_t COLLISION_ATTRIBUTE_PLAYER4 = 0b1 << 9;

//ステージオブジェクト(レベルエディタ)
const uint32_t COLLISION_ATTRIBUTE_STAGE_OBJECT = 0b1 << 10;
