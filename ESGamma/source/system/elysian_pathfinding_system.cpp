#include "elysian_pathfinding_system.hpp"
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_level.hpp>
#include <deque>

namespace elysian {

void PathfindingSystemBase::findAllPathsTo(const unsigned short x,
                                       const unsigned short y,
                                       const int maxDistance) {
  // Although static, it's not necessarry to clear frontier, because at the end
  // of this function, it will always be empty
  static std::deque<Node*> frontier;

  // Exit if no area loaded, bad tile coordinates or init failed (e.g. bad
  // alloc)
  init();
  if (nullptr == mMap || nullptr == mCurrentArea || nullptr == mCurrentLevel ||
      x >= mCurrentArea->width || y >= mCurrentArea->height) {
    return;
  }

  // Breadth first search
  int currPosX = x;
  int currPosY = y;
  Node* current = &mMap[currPosX + mCurrentArea->width * currPosY];
  Node* previous = nullptr;
  Node* next = nullptr;
  frontier.push_back(current);
  while (!frontier.empty()) {
    current = frontier.front();
    frontier.pop_front();

    // Check for collider
    bool collidable = false;
    for (int layer = 0; layer < 3; ++layer) {
      if (mCurrentLevel->getTile(mCurrentArea->getTileEntry(layer, currPosX,
                                                            currPosY)
                                     .getIndex())->getColliderType() !=
          Tile::COLLIDER_NONE) {
        collidable = true;
        break;
      }
    }
    if (collidable) {
      continue;
    }

    // Get distance and compare agains max distance
    current->Distance = (previous == nullptr ? 0 : previous->Distance + 1);
    if (maxDistance >= 0 && current->Distance >= maxDistance) {
      continue;
    }

    // Add children to frontier
    if (currPosX > 0) {
      next = &mMap[--currPosX + mCurrentArea->width * currPosY];
      if (next != previous && !next->Visited) {
        next->Predecessor = current;
        next->Visited = true;
        frontier.push_back(current);
      }
    }
    if (currPosX < mCurrentArea->width - 2) {
      next = &mMap[++currPosX + mCurrentArea->width * currPosY];
      if (next != previous && !next->Visited) {
        next->Predecessor = current;
        next->Visited = true;
        frontier.push_back(current);
      }
    }
    if (currPosY > 0) {
      next = &mMap[currPosX + mCurrentArea->width * --currPosY];
      if (next != previous && !next->Visited) {
        next->Predecessor = current;
        next->Visited = true;
        frontier.push_back(current);
      }
    }
    if (currPosY < mCurrentArea->height - 2) {
      next = &mMap[currPosX + mCurrentArea->width * ++currPosY];
      if (next != previous && !next->Visited) {
        next->Predecessor = current;
        next->Visited = true;
        frontier.push_back(current);
      }
    }
    previous = current;
  }
  return;
}

const PathfindingSystemBase::Node* PathfindingSystemBase::getTileNode(
    const unsigned short x, const unsigned short y) const {
  if (nullptr == mCurrentArea || nullptr == mMap || x >= mCurrentArea->width ||
      y >= mCurrentArea->height) {
    return nullptr;
  }
  return &mMap[x + mCurrentArea->width * y];
}

void PathfindingSystemBase::init() {
  if (mCurrentArea == LevelManager::getCurrentArea()) {
    initSearch();
    return;
  }
  mCurrentArea = LevelManager::getCurrentArea();
  mCurrentLevel = LevelManager::getCurrentLevel();
  if (nullptr == mCurrentArea) {
    return;
  }
  if (mMap) {
    free(mMap);
    mMap = nullptr;
  }
  const size_t byteSize =
      sizeof(Node) * mCurrentArea->height * mCurrentArea->width;
  mMap = (Node*)malloc(byteSize);
  initSearch();
}

void PathfindingSystemBase::initSearch() {
  if (nullptr == mMap || nullptr == mCurrentArea) {
    return;
  }
  const size_t nodeCount = mCurrentArea->width * mCurrentArea->height;
  for (size_t i = 0; i < nodeCount; ++i) {
    mMap[i].Predecessor = nullptr;
    mMap[i].Distance = -1;
    mMap[i].Visited = false;
  }
}
}
