#pragma once
#include "util.h"
#include "actor.h"
#include "world.h"

class Actor;


class Action {
   public:
   virtual ~Action() {};
   virtual void execute() = 0; 
};

class MoveAction : public Action {
    World * w_;
    Actor * a_;
    v2 offset_;
    public:
    /// @brief change position and/or rotation of actor
    /// @param a actor pointer
    /// @param w world pointer
    /// @param offset  position offset
    /// @param rot     new rotation (not an offset!)
    MoveAction(Actor * a, World* w, v2 offset, float rot) {
       a_ = a; 
       w_ = w;
       offset_ = offset;
    }
    void execute() override{
        auto new_pos = a_->pos + offset_;
        auto new_ang = angle_from_two_pos(a_->pos, new_pos);
        auto sz_periph = a_->periph.size();
        if (w_->pos_valid(new_pos)) {
            // If object has any, check if perhipheral segments are valid positions too
            for(int i = 0; i < sz_periph; i++)  {
                if (a_->visible[i]) {
                    if (!w_->pos_valid(a_->periph[i] + new_pos))
                    return;
                }
            }
            a_->pos = new_pos;
            a_->rot = new_ang; // have to decide on point of rotation 
        }
    }
};

class QuitAction : public Action {
    public:
    QuitAction() {}
    void execute() override {
        time_to_cleanup = true;
    }
};