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
    MoveAction(Actor * a, World* w, v2 offset) {
       a_ = a; 
       w_ = w;
       offset_ = offset;
    }
    void execute() override{
        auto new_pos = a_->pos + offset_;
        auto new_ang = angle_from_two_pos(a_->pos, new_pos);
        if (w_->pos_valid(new_pos)) {
            a_->pos = new_pos;
            a_->rot = new_ang; // hero currently doesn't have a visual angle indicator
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