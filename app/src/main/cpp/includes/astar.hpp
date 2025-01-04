#pragma once

#include <utility>
#include <vector>
#include <queue>
#include <unordered_set>
#include <functional>
#include <memory>
#include <world.hpp>
#include <gtitem.h>

struct Node {
    int g;
    int h;
    int f;
    int x, y;
    uint8_t collision_type;
    Node* parent;

    Node(int _x, int _y, uint8_t _collision_type)
            : g(0), h(0), f(0), x(_x), y(_y), collision_type(_collision_type), parent(nullptr) {}
};

struct CompareNode {
    bool operator()(const Node* a, const Node* b) const {
        if (a->f == b->f)
            return a->h > b->h;
        return a->f > b->f;
    }
};

class AStar {
public:
    AStar();
    ~AStar();

    void parse(World* world, ItemDatabase* item_database);
    std::vector<std::pair<int, int>> find_path(int startX, int startY, int goalX, int goalY);

private:
    AStar(const AStar&) = delete;
    AStar& operator=(const AStar&) = delete;

    int calculate_h_cost(int fromX, int fromY, int toX, int toY) const;
    bool is_walkable(int x, int y) const;
    std::vector<std::pair<int, int>> get_neighbors(int x, int y) const;
    bool is_inside_grid(int x, int y) const;

    std::vector<Node*> grid;
    int width, height;

    const std::vector<std::pair<int, int>> directions = {
            {0, -1},  {0, 1},  {-1, 0}, {1, 0},    // Cardinal directions
            {-1, -1}, {1, -1}, {-1, 1}, {1, 1}     // Diagonal directions
    };
};
