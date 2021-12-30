//
//  btree.cpp
//  BanditRush
//
//  Created by 张冬旭 on 2021-11-17.
//

#include <stdio.h>
#include <cassert>
#include <iostream>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "tiny_ecs_registry.hpp"
// The return type of behaviour tree processing
enum class BTState {
    Running,
    Success,
    Failure
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
    virtual void init(Entity e) {};

    virtual BTState process(Entity e) = 0;
};

// A composite node that loops through all children and exits when one fails
class BTRunPair : public BTNode {
private:
    int m_index;
    BTNode* m_children[2];

public:
    BTRunPair(BTNode* c0, BTNode* c1)
        : m_index(0) {
        m_children[0] = c0;
        m_children[1] = c1;
    }

    void init(Entity e) override
    {
        m_index = 0;
        // initialize the first child
        const auto& child = m_children[m_index];
        child->init(e);
    }

    BTState process(Entity e) override {
        if (m_index >= 2)
            return BTState::Success;

        // process current child
        BTState state = m_children[m_index]->process(e);

        // select a new active child and initialize its internal state
        if (state == BTState::Success) {
            ++m_index;
            if (m_index >= 2) {
                return BTState::Success;
            }
            else {
                m_children[m_index]->init(e);
                return BTState::Running;
            }
        }
        else {
            return state;
        }
    }
};

// A composite node that loops through all children and exits when one fails
class BTAnimation : public BTNode {
private:
    int m_index;
    BTNode* m_children[4];

public:
    BTAnimation(BTNode* c0, BTNode* c1, BTNode* c2, BTNode* c3)
        : m_index(0) {
        m_children[0] = c0;
        m_children[1] = c1;
        m_children[2] = c2;
        m_children[3] = c3;
    }

    void init(Entity e) override
    {
        m_index = 0;
        // initialize the first child
        const auto& child = m_children[m_index];
        child->init(e);
    }

    BTState process(Entity e) override {
        if (m_index >= 4)
            return BTState::Success;

        // process current child
        BTState state = m_children[m_index]->process(e);

        // select a new active child and initialize its internal state
        while (state == BTState::Failure) {
            ++m_index;
            if (m_index >= 4) {
                return BTState::Success;
            }
            else {
                state = m_children[m_index]->process(e);
            }
        }
        m_index = 0;
        return state;
    }
};

// A general decorator with lambda condition
class BTIfCondition : public BTNode
{
public:
    BTIfCondition(BTNode* child, std::function<bool(Entity)> condition)
        : m_child(child), m_condition(condition){
    }

    virtual void init(Entity e) override {
        m_child->init(e);
    }

    virtual BTState process(Entity e) override {
        if (m_condition(e))
            return m_child->process(e);
        else
            return BTState::Failure;
    }

private:
    BTNode* m_child;
    std::function<bool(Entity)> m_condition;
};

// Leaf node examples
class RunNSteps : public BTNode {
public:
    RunNSteps(int steps) noexcept
        : m_targetSteps(steps), m_stepsRemaining(0) {
    }

private:
    void init(Entity e) override {
        m_stepsRemaining = m_targetSteps;
    }

    BTState process(Entity e) override {
        // update internal state
        --m_stepsRemaining;

        // modify world
        auto& motion = registry.motions.get(e);
        motion.position += motion.velocity;

        // return progress
        if (m_stepsRemaining > 0) {
            return BTState::Running;
        } else {
            return BTState::Success;
        }
    }

private:
    int m_targetSteps;
    int m_stepsRemaining;
};

class TurnAround : public BTNode {
private:
    void init(Entity e) override {
    }

    BTState process(Entity e) override {
        // modify world
        auto& vel = registry.motions.get(e).velocity;
        vel = -vel;

        // return progress
        return BTState::Success;
    }
};

class ToAttack : public BTNode {
private:
    void init(Entity e) override {
    }

    BTState process(Entity e) override {
        // modify world
        int id;
        int id1 = (int)TEXTURE_ASSET_ID::ATTACK_SKULL_00;
        int id2 = (int)TEXTURE_ASSET_ID::ATTACK_SKULL2_00;
        int id3 = (int)TEXTURE_ASSET_ID::ATTACK_SKULL3_00;
        
        if (registry.hardShells.has(e)) {
            RenderRequest& render_requester = registry.renderRequests.get(e);
            State& state = registry.states.get(e);
            Motion& motion = registry.motions.get(e);
            HardShell& hs = registry.hardShells.get(e);
            if ((int)hs.type == 0) {
                id = id1;
            } else if ((int)hs.type == 1) {
                id = id2;
            } else if ((int)hs.type == 2) {
                id = id3;
            } else {
                return BTState::Failure;
            }
            state.current_state = SkullState::Attacking;
            render_requester.used_texture = (TEXTURE_ASSET_ID)id;
            motion.pre_vel.x = motion.velocity.x;
            motion.pre_vel.y = motion.velocity.y;
            motion.velocity = { 0 , 0 };
            return BTState::Success;
        }

        // return progress
        return BTState::Failure;
    }
};

class ToDie : public BTNode {
private:
    void init(Entity e) override {
    }

    BTState process(Entity e) override {
        // modify world
        int id;
        int id1 = (int)TEXTURE_ASSET_ID::DIE_SKULL_00;
        int id2 = (int)TEXTURE_ASSET_ID::DIE_SKULL2_00;
        int id3 = (int)TEXTURE_ASSET_ID::DIE_SKULL3_00;
        if (registry.hardShells.has(e)) {
            RenderRequest& render_requester = registry.renderRequests.get(e);
            State& state = registry.states.get(e);
            Motion& motion = registry.motions.get(e);
            HardShell& hs = registry.hardShells.get(e);
            if ((int)hs.type == 0) {
                id = id1;
            } else if ((int)hs.type == 1) {
                id = id2;
            } else if ((int)hs.type == 2) {
                id = id3;
            } else {
                return BTState::Failure;
            }
            state.current_state = SkullState::Dying;
            render_requester.used_texture = (TEXTURE_ASSET_ID)id;
            motion.pre_vel.x = motion.velocity.x;
            motion.pre_vel.y = motion.velocity.y;
            motion.velocity = { 0 , 0 };
            
            return BTState::Success;
        }
        // return progress
        return BTState::Failure;
    }
};

class InWalk : public BTNode {
private:
    void init(Entity e) override {
    }

    BTState process(Entity e) override {
        // modify world
        int wlast1 = (int)TEXTURE_ASSET_ID::WALK_SKULL_17;
        int wlast2 = (int)TEXTURE_ASSET_ID::WALK_SKULL2_17;
        int wlast3 = (int)TEXTURE_ASSET_ID::WALK_SKULL3_17;
        int wlength = (int)TEXTURE_ASSET_ID::WALK_SKULL_17 - (int)TEXTURE_ASSET_ID::WALK_SKULL_00 + 1;
        RenderRequest& rr = registry.renderRequests.get(e);
        int id = (int)rr.used_texture;
        id++;
        HardShell& hs = registry.hardShells.get(e);
        if ((int)hs.type == 0) {
            if (id > wlast1) {
                id -= wlength;
            }
        } else if ((int)hs.type == 1) {
            if (id > wlast2) {
                id -= wlength;
            }
        } else if ((int)hs.type == 2) {
            if (id > wlast3) {
                id -= wlength;
            }
        } else {
            return BTState::Failure;
        }
        rr.used_texture = (TEXTURE_ASSET_ID) id;

        // return progress
        return BTState::Success;
    }
};

class InAttack : public BTNode {
private:
    void init(Entity e) override {
    }

    BTState process(Entity e) override {
        // modify world
        int alast1 = (int)TEXTURE_ASSET_ID::ATTACK_SKULL_11;
        int alast2 = (int)TEXTURE_ASSET_ID::ATTACK_SKULL2_11;
        int alast3 = (int)TEXTURE_ASSET_ID::ATTACK_SKULL3_11;
        int alength = (int)TEXTURE_ASSET_ID::ATTACK_SKULL_11 - (int)TEXTURE_ASSET_ID::ATTACK_SKULL_00 + 1;
        RenderRequest& rr = registry.renderRequests.get(e);
        int id = (int)rr.used_texture;
        id++;
        HardShell& hs = registry.hardShells.get(e);
        if ((int)hs.type == 0) {
            if (id > alast1) {
                id -= alength;
            }
        } else if ((int)hs.type == 1) {
            if (id > alast2) {
                id -= alength;
            }
        } else if ((int)hs.type == 2) {
            if (id > alast3) {
                id -= alength;
            }
        } else {
            return BTState::Failure;
        }
        rr.used_texture = (TEXTURE_ASSET_ID) id;

        // return progress
        return BTState::Success;
    }
};

class InIdle : public BTNode {
private:
    void init(Entity e) override {
    }

    BTState process(Entity e) override {
        // modify world
        int ilast1 = (int)TEXTURE_ASSET_ID::IDLE_SKULL_11;
        int ilast2 = (int)TEXTURE_ASSET_ID::IDLE_SKULL2_11;
        int ilast3 = (int)TEXTURE_ASSET_ID::IDLE_SKULL3_11;
        int ilength = (int)TEXTURE_ASSET_ID::IDLE_SKULL_11 - (int)TEXTURE_ASSET_ID::IDLE_SKULL_00 + 1;
        RenderRequest& rr = registry.renderRequests.get(e);
        int id = (int)rr.used_texture;
        id++;
        HardShell& hs = registry.hardShells.get(e);
        if ((int)hs.type == 0) {
            if (id > ilast1) {
                id -= ilength;
            }
        } else if ((int)hs.type == 1) {
            if (id > ilast2) {
                id -= ilength;
            }
        } else if ((int)hs.type == 2) {
            if (id > ilast3) {
                id -= ilength;
            }
        } else {
            return BTState::Failure;
        }
        rr.used_texture = (TEXTURE_ASSET_ID) id;

        // return progress
        return BTState::Success;
    }
};

class Dying : public BTNode {
private:
    void init(Entity e) override {
    }

    BTState process(Entity e) override {
        // modify world
        int dlast1 = (int)TEXTURE_ASSET_ID::DIE_SKULL_14;
        int dlast2 = (int)TEXTURE_ASSET_ID::DIE_SKULL2_14;
        int dlast3 = (int)TEXTURE_ASSET_ID::DIE_SKULL3_14;
        RenderRequest& rr = registry.renderRequests.get(e);
        int id = (int)rr.used_texture;
        id++;
        HardShell& hs = registry.hardShells.get(e);
        if ((int)hs.type == 0) {
            if (id > dlast1) {
                registry.remove_all_components_of(e);
                return BTState::Success;
            }
        } else if ((int)hs.type == 1) {
            if (id > dlast2) {
                registry.remove_all_components_of(e);
                return BTState::Success;
            }
        } else if ((int)hs.type == 2) {
            if (id > dlast3) {
                registry.remove_all_components_of(e);
                return BTState::Success;
            }
        } else {
            return BTState::Failure;
        }
        rr.used_texture = (TEXTURE_ASSET_ID) id;

        // return progress
        return BTState::Running;
    }
};
