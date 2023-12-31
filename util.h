// Free helper functions. Many are used from the olc namespace and the qt engine but a few are here.
#pragma once
#include "olcPixelGameEngine.h"
#include <array>
#include <cassert>
#include <functional>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

using vec_pts = std::vector<olc::vf2d>;
using veci_pts = std::vector<olc::vi2d>;
using v2 = olc::vf2d;
using v2i = olc::vi2d;
using rint_distr = std::uniform_int_distribution<int>;
#define PI 3.14159265
#define RAD2DEG 180. / PI
#define DEG2RAD PI / 180.
#define UPDATE_INTERVAL 0.4

static inline float clamp(float num, float min, float max) {
  if (num < min) {
    return min;
  }
  if (num > max) {
    return max;
  }
  return num;
};

static inline float angle_from_two_pos(v2 before, v2 after) {
  v2 dir = after - before;
  return atan2(dir.y, dir.x);
}

// random number generator
static std::default_random_engine generator(1);