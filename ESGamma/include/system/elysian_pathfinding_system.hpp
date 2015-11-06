#ifndef ELYSIAN_PATHFINDING_SYSTEM_HPP
#define ELYSIAN_PATHFINDING_SYSTEM_HPP

#include <elysian_system.hpp>
#include <cstdlib>

namespace elysian {

class PathfindingSystemBase : public System {
 public:
  struct Node {
    Node* Predecessor = nullptr;
    int Distance = -1;
    bool Visited = false;
  };

  PathfindingSystemBase() = default;

  void findAllPathsTo(const unsigned short x, const unsigned short y,
                      const int maxDistance = -1);

  const Node* getTileNode(const unsigned short x, const unsigned short y) const;

 private:
  void init();
  void initSearch();

  Node* mMap = nullptr;
  Area* mCurrentArea = nullptr;
  Level* mCurrentLevel = nullptr;
};
}
#endif  // ELYSIAN_PATHFINDING_SYSTEM_HPP
