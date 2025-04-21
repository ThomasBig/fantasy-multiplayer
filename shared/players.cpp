#include <sstream>
#include <unordered_set>

#include "players.hpp"

std::string Players::serialize() const {
  std::stringstream serialized;
  for (auto const& [id, player] : data) {
    serialized << id << " " << player.serialize() << " ";
  }
  return serialized.str();
}

void Players::deserialize(std::string serialized) {
  std::stringstream stream(serialized);
  int id;
  int avatar;
  int x;
  int y;
  std::unordered_set<int> used_ids;

  while (stream >> id >> avatar >> x >> y) {
    if (data.find(id) != data.end()) {
      data[id].avatar = avatar;
      data[id].target_x = float(x);
      data[id].target_y = float(y);
    } else {
      data[id] = Player(float(x), float(y), avatar);
    }
    used_ids.insert(id);
  }

  std::unordered_map<int, Player>::iterator iterator = data.begin();
  while (iterator != data.end()) {
    auto const& [id, _player] = *iterator;
    if (used_ids.find(id) == used_ids.end()) {
      iterator = data.erase(iterator);
    } else {
      ++iterator;
    }
  }
}