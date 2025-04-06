#include <sstream>
#include <unordered_map>
#include <unordered_set>

struct Player {
  static constexpr float speed = 0.1f;
  float target_x = 240.0f;
  float target_y = 135.0f;
  float current_x = 240.0f;
  float current_y = 135.0f;
  int skin = 0;

  std::string serialize() const {
    std::stringstream serialized;
    serialized << skin << " " << target_x << " " << target_y;
    return serialized.str();
  }

  void deserialize(std::string serialized) {
    std::stringstream stream(serialized);
    stream >> skin >> target_x >> target_y;
  }

  void update(int delta_ms) {
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
};

struct Players {
  std::unordered_map<int, Player> data;

  std::string serialize() const {
    std::stringstream serialized;
    for (auto const& [id, player] : data) {
      serialized << id << " " << player.serialize() << " ";
    }
    return serialized.str();
  }

  void deserialize(std::string serialized) {
    std::stringstream stream(serialized);
    int id;
    int skin;
    float x;
    float y;
    std::unordered_set<int> used_ids;

    while (stream >> id >> skin >> x >> y) {
      if (data.find(id) != data.end()) {
        data[id].skin = skin;
        data[id].target_x = x;
        data[id].target_y = y;
      } else {
        data[id] = {x,y, x,y, skin};
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
};
