/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_PATHFINDING_SYSTEM_HPP
#define ELYSIAN_PATHFINDING_SYSTEM_HPP

#include <ESGamma/system/elysian_system.hpp>
#include <cstdlib>

namespace elysian {

class PathfindingSystem : public System {
 public:
  struct Node {
    Node* Predecessor = nullptr;
    unsigned short PosX;
    unsigned short PosY;
    int Distance = -1;
    bool Visited = false;

    Node* getPredecessor() const { return Predecessor; }
    int getDistance() const { return Distance; }
  };

  PathfindingSystem() = default;

  void findAllPathsTo(const unsigned short x, const unsigned short y,
                      const int maxDistance);

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
