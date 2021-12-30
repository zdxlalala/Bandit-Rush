#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
const float FISH_BB_WIDTH = 0.4f * 296.f;
const float FISH_BB_HEIGHT = 0.4f * 165.f;
const float SKULL_BB_WIDTH = 0.1f * 500.f;
const float SKULL_BB_HEIGHT = 0.1f * 348.f;

const float CASTLE_BB_WIDTH = 0.4f * 495;
const float CASTLE_BB_HEIGHT = 0.4f * 440;
const float TURRET_BB_WIDTH = 0.4f * 148;
const float TURRET_BB_HEIGHT = 0.4f * 200;
const float WALL_BB_WIDTH = 0.4f * 240;
const float WALL_BB_HEIGHT = 0.4f * 201;
const float BULLET_BB_WIDTH = 0.04f * 836;
const float BULLET_BB_HEIGHT = 0.04f * 171;

const float BUTTON_BB_WIDTH = 0.4f * 400;
const float BUTTON_BB_HEIGHT = 0.4f * 185;

// the player
Entity createSalmon(RenderSystem* renderer, vec2 pos);
// the prey
Entity createFish(RenderSystem* renderer, vec2 position);
Entity createTurret(vec2 position, RenderSystem* renderer, float health, TurretType type);
Entity createWall(vec2 position, RenderSystem* renderer, float health);
Entity createBullet(RenderSystem* renderer, vec2 position, int damage);
// the enemy
Entity createSkull(RenderSystem* renderer, vec2 position, float w, float h, int type);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size, float angle);
// a pebble
Entity createPebble(vec2 pos, vec2 size);

// the castle
Entity createCastle(RenderSystem* renderer, vec2 position);

Entity createBar(RenderSystem* renderer, vec2 position, vec2 scale);

// the button
Entity createButton(RenderSystem* renderer, vec2 position);

// the button
Entity createLight(RenderSystem* renderer, vec2 position, Entity target);


