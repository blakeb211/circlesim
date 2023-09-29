#pragma once
#include "actor.h"
#include "util.h"
#include "world.h"

class Actor;

class Action {
public:
  virtual ~Action(){};
  virtual void execute() = 0;
};

class MoveAction : public Action {
  World *_w;
  Actor *_a;
  v2 _offset;

public:
  /// @brief change position and/or rotation of actor
  /// @param a actor pointer
  /// @param w world pointer
  /// @param offset  position offset
  MoveAction() = delete;
  MoveAction(Actor *a, World *w, v2 offset) {
    _a = a;
    _w = w;
    _offset = offset;
  }
  void execute() override {
    auto new_pos = _a->pos + _offset;
    // set facing member variable to the angle of the move direction
    {
      auto new_ang = angle_from_two_pos(_a->pos, new_pos);
      _a->facing = new_ang; // have to decide on point of rotation
    }
    // done with setting facing

    auto sz_periph = _a->periph.size();
    if (_w->pos_valid(_a, new_pos)) {
      // If object has any, check if perhipheral segments are valid positions
      // too
      for (int i = 0; i < sz_periph; i++) {
        if (_a->visible[i]) {
          if (!_w->pos_valid(_a, _a->periph[i] + new_pos))
            return;
        }
      }
      // realize the translation
      _a->pos = new_pos;
      // add positons to the world occupation map
      _w->occupation[v2i(static_cast<int>(_a->pos.x),
                         static_cast<int>(_a->pos.y))] =
          cell_context{_a->get_id(), _a->shape};

      for (int i = 0; i < sz_periph; i++) {
        const auto abs_loc = _a->periph[i] + _a->pos;
        const auto x = static_cast<int>(abs_loc.x);
        const auto y = static_cast<int>(abs_loc.y);
        _w->occupation[v2i(x, y)] = cell_context{_a->get_id(), _a->shape};
      }
    }
  }
};

class RotateAction : public Action {
  Actor *_a;
  World *_w;
  float _offset;

public:
  RotateAction() = delete;
  RotateAction(Actor *a, World *w, float offset)
      : _a{a}, _w{w}, _offset{offset} {}
  void execute() {
    // only rotate the peripheral segments of the actor
    const auto center = _a->pos;
    auto periph_copy = _a->periph;
    for (int i = 0; i < periph_copy.size(); i++) {
      v2 center = _a->pos;
      v2 cell = periph_copy[i] + center;
      periph_copy[i].x = (cell.x - center.x) * cos(_offset) -
                         (cell.y - center.y) * sin(_offset) + center.x;
      periph_copy[i].y = (cell.x - center.x) * sin(_offset) +
                         (cell.y - center.y) * cos(_offset) + center.y;
      periph_copy[i].x = std::round(periph_copy[i].x);
      periph_copy[i].y = std::round(periph_copy[i].y);
    }
    int count_of_valid_moves{0};
    const auto sz_periph = periph_copy.size();
    for (int i = 0; i < sz_periph; i++) {
      if (_w->pos_valid(_a, periph_copy[i])) {
        count_of_valid_moves++;
        periph_copy[i] -= center;
      }
    }

    if (count_of_valid_moves == sz_periph) {
      // realize the rotation
      _a->periph = periph_copy;
     
      // add positons to the world occupation map
      _w->occupation[v2i(static_cast<int>(center.x),
                         static_cast<int>(center.y))] =
          cell_context{_a->get_id(), _a->shape};

      for (int i = 0; i < sz_periph; i++) {
        const auto abs_loc = periph_copy[i] + _a->pos;
        const auto x = static_cast<int>(abs_loc.x);
        const auto y = static_cast<int>(abs_loc.y);
        _w->occupation[v2i(x, y)] = cell_context{_a->get_id(), _a->shape};
      }
    }
  }
};

class QuitAction : public Action {
public:
  QuitAction() {}
  void execute() override { g_time_to_cleanup = true; }
};
