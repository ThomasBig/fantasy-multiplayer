#pragma once

#include <unordered_map>
#include "player.hpp"

struct Players {
  std::unordered_map<int, Player> data;

  std::string serialize() const;
  void deserialize(std::string serialized);
};
