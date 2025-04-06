#include <sstream>
#include <unordered_map>
#include <unordered_set>

struct Player {
  static constexpr float speed = 0.1f;
  float target_x;
  float target_y;
  float current_x;
  float current_y;
  int skin;

  Player(float x, float y, int skin) {
    target_x = x;
    target_y = y;
    current_x = x;
    current_y = y;
    this->skin = skin;
  }

  Player() : Player(240.0f, 135.0f, 0) {}

  std::string serialize() const {
    std::stringstream serialized;
    serialized << skin << " " << int(target_x) << " " << int(target_y);
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
    int x;
    int y;
    std::unordered_set<int> used_ids;

    while (stream >> id >> skin >> x >> y) {
      if (data.find(id) != data.end()) {
        data[id].skin = skin;
        data[id].target_x = float(x);
        data[id].target_y = float(y);
      } else {
        data[id] = Player(float(x), float(y), skin);
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
