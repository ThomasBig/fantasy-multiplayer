#include <sstream>
#include <unordered_map>
#include <unordered_set>

struct Player {
  static constexpr float speed = 0.1f;
  float target_x = 480.0f;
  float target_y = 270.0f;
  float current_x = 480.0f;
  float current_y = 270.0f;

  std::string serialize() const {
    std::stringstream serialized;
    serialized << current_x << " " << current_y;
    return serialized.str();
  }

  void update(std::string serialized) {
    std::stringstream stream(serialized);
    stream >> current_x >> current_y;
  }
};

struct Players
{
  std::unordered_map<int, Player> data;

  std::string serialize() const {
    std::stringstream serialized;
    for (auto const& [id, player] : data) {
      serialized << id << " " << player.serialize() << " ";
    }
    return serialized.str();
  }

  void update(std::string serialized) {
    std::stringstream stream(serialized);
    int id;
    float x;
    float y;
    std::unordered_set<int> used_ids;

    while (stream >> id >> x >> y) {
      if (data.find(id) != data.end()) {
        data[id].current_x = x;
        data[id].current_y = y;
      } else {
        data[id] = {x,y, x,y};
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
