#pragma once
#include "olcPixelGameEngine.h"
#include <vector>
#include <cassert>
#include <unordered_map>
#include <random>
#include <array>
#include <set>

using vec_pts = std::vector<olc::vf2d>;
using veci_pts = std::vector<olc::vi2d>;
using v2 = olc::vf2d;
using v2i = olc::vi2d;
using rint_distr = std::uniform_int_distribution<int>;

static bool g_time_to_cleanup{false};
#define PI 3.14159265
#define RAD2DEG 180. / PI
#define DEG2RAD PI / 180.
#define UPDATE_INTERVAL 0.5

static inline float angle_from_two_pos(v2 before, v2 after) {
    v2 dir = after - before;
    return atan2(dir.y,dir.x);
}

// random number generator
static std::default_random_engine generator(1);