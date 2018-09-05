#include <ESGamma/system/elysian_pathfinding_system.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <AssetIO/crtp/elysian_level.hpp>
#include <AssetIO/utility/elysian_ring_buffer.hpp>

namespace elysian {

void PathfindingSystem::findAllPathsTo(const unsigned short x,
                                           const unsigned short y,
                                           const int maxDistance) {
  // Although static, it's not necessarry to clear frontier, because at the end
  // of this function, it will always be empty
  static RingBuffer<Node*, 2048> frontier;

  // Exit if no area loaded, bad tile coordinates or init failed (e.g. bad
  // alloc)
  init();
  if (nullptr == mMap || nullptr == mCurrentArea || nullptr == mCurrentLevel ||
      x >= mCurrentArea->width || y >= mCurrentArea->height) {
    return;
  }

  // Breadth first search
  Node* current = &mMap[x + mCurrentArea->width * y];
  Node* previous = nullptr;
  Node* next = nullptr;
  frontier.push_back(current);
  while (!frontier.empty()) {
    current = frontier.front();
    frontier.pop_front();

    // Check for collider
    bool collidable = false;
    for (int layer = 0; layer < 3; ++layer) {
      if (mCurrentLevel->getTile(mCurrentArea->getTileEntry(
                                                   layer, current->PosX,
                                                   current->PosY).getIndex())
              ->getColliderType() != Tile::COLLIDER_NONE) {
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
    // TODO Consider adding the diagonal neighbors with distance = sqrt(2)
    if (current->PosX > 0) {
      next = &mMap[current->PosX - 1 + mCurrentArea->width * current->PosY];
      if (next != previous && !next->Visited) {
        next->Predecessor = current;
        next->Visited = true;
        frontier.push_back(current);
      }
    }
    if (current->PosX < mCurrentArea->width - 2) {
      next = &mMap[current->PosX + 1 + mCurrentArea->width * current->PosY];
      if (next != previous && !next->Visited) {
        next->Predecessor = current;
        next->Visited = true;
        frontier.push_back(current);
      }
    }
    if (current->PosY > 0) {
      next = &mMap[current->PosX + mCurrentArea->width * (current->PosY - 1)];
      if (next != previous && !next->Visited) {
        next->Predecessor = current;
        next->Visited = true;
        frontier.push_back(current);
      }
    }
    if (current->PosY < mCurrentArea->height - 2) {
      next = &mMap[current->PosX + mCurrentArea->width * (current->PosY + 1)];
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

const PathfindingSystem::Node* PathfindingSystem::getTileNode(
    const unsigned short x, const unsigned short y) const {
  if (nullptr == mCurrentArea || nullptr == mMap || x >= mCurrentArea->width ||
      y >= mCurrentArea->height) {
    return nullptr;
  }
  return &mMap[x + mCurrentArea->width * y];
}

void PathfindingSystem::init() {
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
  for (size_t y = 0; y < mCurrentArea->height; ++y) {
    for (size_t x = 0; x < mCurrentArea->width; ++x) {
      mMap[x + mCurrentArea->width * y].PosX = x;
      mMap[x + mCurrentArea->width * y].PosY = y;
    }
  }
  initSearch();
}

void PathfindingSystem::initSearch() {
  if (nullptr == mMap || nullptr == mCurrentArea) {
    return;
  }
  for (size_t y = 0; y < mCurrentArea->height; ++y) {
    for (size_t x = 0; x < mCurrentArea->width; ++x) {
      mMap[x + mCurrentArea->width * y].Predecessor = nullptr;
      mMap[x + mCurrentArea->width * y].Distance = -1;
      mMap[x + mCurrentArea->width * y].Visited = false;
    }
  }
}
}
