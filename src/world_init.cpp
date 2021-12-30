#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"


Entity createSalmon(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SALMON);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 150.f;
	motion.scale.x *= -1; // point front to the right

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::SALMON,
			GEOMETRY_BUFFER_ID::SALMON });

	return entity;
}

Entity createBullet(RenderSystem* renderer, vec2 position, int damage)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
//	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	SoftShell& ss = registry.softShells.emplace(entity);
    ss.bullet_damage = damage;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FISH,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createSkull(RenderSystem* renderer, vec2 position, float w, float h, int type)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
    // motion.velocity = { 50.f, 50.f };
    // Setting path randomly
//    motion.path = (PATH)(rand()%(int)PATH::COUNT);
//    if (motion.path == PATH::THREE) {
//        float angle = atan2((h/2) - position.y, (w/2) - position.x);
//        motion.velocity.x *= cos(angle);
//        motion.velocity.y *= sin(angle);
//    } else {
//        int dirx = (position.x > (w/2)) ? -1 : 1;
//        int diry = (position.y > (h/2)) ? -1 : 1;
//        motion.velocity.x *= dirx;
//        motion.velocity.y *= diry;
//    }
    
    // Make sure the skull spawn within the window
    if (position.x - (SKULL_BB_WIDTH/2) <= -w/2) {
        position.x += SKULL_BB_WIDTH/2;
    }
    if (position.x + (SKULL_BB_WIDTH/2) >= (w+w/2)) {
        position.x -= SKULL_BB_WIDTH/2;
    }
    if (position.y - (SKULL_BB_HEIGHT/2) <= -h/2) {
        position.y += SKULL_BB_HEIGHT/2;
    }
    if (position.y + (SKULL_BB_HEIGHT/2) >= (h+h/2)) {
        position.y -= SKULL_BB_HEIGHT/2;
    }
	motion.position = position;
    
    for(int i = 0; i < 21; i++) {
        for (int j = 0; j< 21; j++) {
            motion.pastNodes[i][j] = 0;
        }
    }

	// Setting initial values, scale is negative to make it face the opposite way
    int dir = (position.x > (w/2)) ? -1 : 1;
    
    //motion.velocity = {0,0};

	// Create and (empty) Skull component to be able to refer to all skulls
	HardShell& hs = registry.hardShells.emplace(entity);
    motion.scale = vec2({ dir*SKULL_BB_WIDTH, SKULL_BB_HEIGHT });
    if (type == 0) {
        hs.skull_health = 500;
        hs.skull_damage = 100;
        hs.type = SkullType::ONE;
        registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::WALK_SKULL_00,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE });
    } else if (type == 1) {
        motion.scale *= 1.25f;
        hs.skull_health = 800;
        hs.skull_damage = 150;
        hs.type = SkullType::TWO;
        registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::WALK_SKULL2_00,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE });
    } else {
        motion.scale *= 1.5f;
        hs.skull_health = 1200;
        hs.skull_damage = 300;
        hs.type = SkullType::THREE;
        registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::WALK_SKULL3_00,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE });
    }
    registry.states.insert(entity, {SkullState::Walking, (SkullState)NULL});

	return entity;
}

Entity createWall(vec2 position, RenderSystem* renderer, float health)
{
    auto entity = Entity();
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//    registry.meshPtrs.emplace(entity, &mesh);
    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -WALL_BB_WIDTH, WALL_BB_HEIGHT });
    
    Wall& wall = registry.walls.emplace(entity);
    wall.health = health;
    wall.position = position;
    
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::WALL,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createTurret(vec2 position, RenderSystem* renderer, float health, TurretType type)
{
    auto entity = Entity();
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//    registry.meshPtrs.emplace(entity, &mesh);
    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -TURRET_BB_WIDTH, TURRET_BB_HEIGHT });
    
    Turret& turret = registry.turrets.emplace(entity);
    turret.health = health;
    turret.position = position;
    turret.attack = false;
    turret.tt = type;
    turret.counter = 0.f;
    
    if (type == TurretType::ONE) {
        registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::TURRET,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE });
    } else if (type == TurretType::TWO) {
        registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::TURRET2,
             EFFECT_ASSET_ID::TEXTURED,
             GEOMETRY_BUFFER_ID::SPRITE });
    }

    return entity;
}

Entity createLine(vec2 position, vec2 scale, float angle)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::LINE,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = angle;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createPebble(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = size;

	// Create and (empty) Salmon component to be able to refer to all turtles
	registry.players.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::PEBBLE,
			GEOMETRY_BUFFER_ID::PEBBLE });

	return entity;
}

// Create the castle
Entity createCastle(RenderSystem* renderer, vec2 position)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -CASTLE_BB_WIDTH, CASTLE_BB_HEIGHT });

    registry.players.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::CASTLE,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

// Create the castle
Entity createButton(RenderSystem* renderer, vec2 position)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -BUTTON_BB_WIDTH, BUTTON_BB_HEIGHT });

    registry.buttons.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::BUTTON,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });
    
    return entity;
}

Entity createLight(RenderSystem* renderer, vec2 position, Entity target)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = vec2({ -50, 50 });

    Light& l = registry.light.emplace(entity);
    l.target = target;
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT,
         EFFECT_ASSET_ID::LIGHT,
         GEOMETRY_BUFFER_ID::PEBBLE });
    
    return entity;
}

// Create the castle
Entity createBar(RenderSystem* renderer, vec2 position, vec2 scale)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
//    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
//    registry.meshPtrs.emplace(entity, &mesh);

    // Initialize the motion
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;

    // Setting initial values, scale is negative to make it face the opposite way
    motion.scale = scale;

    registry.gb.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::BAR,
         EFFECT_ASSET_ID::BAR,
         GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}
