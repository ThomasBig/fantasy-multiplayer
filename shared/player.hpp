#pragma once

#include <string>

struct Player {
  static constexpr float speed = 0.1f;
  float target_x;
  float target_y;
  float current_x;
  float current_y;
  int avatar;

public:
  Player();
  Player(float x, float y, int avatar);
  std::string serialize() const;
  void deserialize(std::string serialized);
  void update_position(int delta_ms);
};
