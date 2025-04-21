#include <sstream>

#include "player.hpp"

Player::Player() : Player(240.0f, 135.0f, 0) {}

Player::Player(float x, float y, int avatar) {
  this->target_x = x;
  this->target_y = y;
  this->current_x = x;
  this->current_y = y;
  this->avatar = avatar;
}

std::string Player::serialize() const {
  std::stringstream serialized;
  serialized << avatar << " " << int(target_x) << " " << int(target_y);
  return serialized.str();
}

void Player::deserialize(std::string serialized) {
  std::stringstream stream(serialized);
  stream >> avatar >> target_x >> target_y;
}

void Player::update(int delta_ms) {
  if (current_x < target_x) {
    current_x = std::min(current_x + delta_ms * speed, target_x);
  } else if (current_x > target_x) {
    current_x = std::max(current_x - delta_ms * speed, target_x);
  }
  if (current_y < target_y) {
    current_y = std::min(current_y + delta_ms * speed, target_y);
  } else if (current_y > target_y) {
    current_y = std::max(current_y - delta_ms * speed, target_y);
  }
}
