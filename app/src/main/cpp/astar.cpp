#include "astar.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <gtitem.h>

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

AStar::AStar() : width(0), height(0) {}

AStar::~AStar() {
    for (auto node : grid) {
        delete node;
    }
}

void AStar::parse(World* world, ItemDatabase* item_database) {
    for (auto node : grid) {
        delete node;
    }
    grid.clear();

    if (world->name == "EXIT") {
        return;
    }

    width = world->width;
    height = world->height;
    grid.reserve(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t index = y * width + x;
            Tile tile = world->tiles.at(index);
            Item item = item_database->items[tile.foreground_item_id];
            uint8_t collision_type = item.collision_type;

            grid.emplace_back(new Node(x, y, collision_type));
        }
    }
}

int AStar::calculate_h_cost(int fromX, int fromY, int toX, int toY) const {
    int dx = std::abs(toX - fromX);
    int dy = std::abs(toY - fromY);
    return 14 * std::min(dx, dy) + 10 * (std::max(dx, dy) - std::min(dx, dy));
}

bool AStar::is_inside_grid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool AStar::is_walkable(int x, int y) const {
    if (!is_inside_grid(x, y))
        return false;
    uint8_t collision = grid[y * width + x]->collision_type;
    return collision == 0;
}

std::vector<std::pair<int, int>> AStar::get_neighbors(int x, int y) const {
    std::vector<std::pair<int, int>> neighbors;
    for (const auto& dir : directions) {
        int newX = x + dir.first;
        int newY = y + dir.second;
        if (is_walkable(newX, newY)) {
            if (dir.first != 0 && dir.second != 0) {
                if (is_walkable(x + dir.first, y) && is_walkable(x, y + dir.second)) {
                    neighbors.emplace_back(newX, newY);
                }
            } else {
                neighbors.emplace_back(newX, newY);
            }
        }
    }
    return neighbors;
}

std::vector<std::pair<int, int>> AStar::find_path(int startX, int startY, int goalX, int goalY) {
    std::vector<std::pair<int, int>> path;

    if (!is_inside_grid(startX, startY) || !is_inside_grid(goalX, goalY))
        return path;

    if (!is_walkable(startX, startY) || !is_walkable(goalX, goalY))
        return path;

    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openList;
    std::unordered_set<std::pair<int, int>, pair_hash> closedSet;

    Node* startNode = grid[startY * width + startX];
    startNode->g = 0;
    startNode->h = calculate_h_cost(startX, startY, goalX, goalY);
    startNode->f = startNode->g + startNode->h;
    startNode->parent = nullptr;

    openList.push(startNode);

    while (!openList.empty()) {
        Node* currentNode = openList.top();
        openList.pop();

        if (currentNode->x == goalX && currentNode->y == goalY) {
            Node* node = currentNode;
            while (node != nullptr) {
                path.emplace_back(node->x, node->y);
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            break;
        }

        closedSet.emplace(std::make_pair(currentNode->x, currentNode->y));

        auto neighbors = get_neighbors(currentNode->x, currentNode->y);
        for (const auto& neighborPos : neighbors) {
            int neighborX = neighborPos.first;
            int neighborY = neighborPos.second;

            if (closedSet.find(neighborPos) != closedSet.end())
                continue;

            Node* neighborNode = grid[neighborY * width + neighborX];
            int tentative_g = currentNode->g + ((neighborX != currentNode->x && neighborY != currentNode->y) ? 14 : 10);

            if (tentative_g < neighborNode->g || neighborNode->f == 0) {
                neighborNode->g = tentative_g;
                neighborNode->h = calculate_h_cost(neighborX, neighborY, goalX, goalY);
                neighborNode->f = neighborNode->g + neighborNode->h;
                neighborNode->parent = currentNode;
                openList.push(neighborNode);
            }
        }
    }

    return path;
}