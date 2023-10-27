#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

/*
    state enum is used to facilitate the process of determining whether it is a path, 
    obstcle, or an empty cell, for later use of visualization when printing the result
*/
enum class State {KEmpty, KObstacle, KPath, KClosed, KFinalPath, KStart, KGoal};

typedef struct node
{
    int x;
    int y;
    int h_val;
    int g_val;
};

// visualization of our map depending on the state value
std::string CellString (State cell) {

    if (cell == State::KObstacle) {
        return "🌳  ";
    } else if (cell == State::KPath) {
        return "🚶  ";
    } else if (cell == State::KStart) {
        return "🚦  ";
    } else if (cell == State::KGoal) {
        return "🏁 ";
    } else if (cell == State::KEmpty || cell == State::KClosed) {
        return "0   ";
    } 

}


// processing the input file line by line & marking them with empty or obstcle
std::vector<State> ParseLine(const std::string& line) {

    std::istringstream stream(line);
    int material;
    char coma;

    std::vector<State> row;
    while (stream >> material >> coma)
    {
        if (material == 0) {
            row.emplace_back(State::KEmpty);
        } else if (material == 1) {
            row.emplace_back(State::KObstacle);
        }
    }
    
    return row;
}


// reading the grid from our file
std::vector<std::vector<State>> ReadBoardFile(const std::string& path) {

    std::vector<std::vector<State>> board;
    std::ifstream maze;
    maze.open(path);

    if(maze) {
        std::string line;
        while (std::getline(maze, line))
        {
            board.emplace_back(ParseLine(line));
        }
        
    } else {
        std::cout << "Not a valid file name!" << '\n';
    }

    maze.close();
    return board;
}


// printing the visualized grid with the shortest path to the goal
void PrintSolution(const std::vector<std::vector<State>>& board) {

    for (auto & row : board) {
        for (auto column : row) {
            std::cout << CellString(column);
        }
        std::cout << '\n';
    }

}   

int hueristic(node current, node goal) {
    return std::abs(goal.x - current.x) + std::abs(goal.y - current.y);
}


bool isVaildCell(const std::vector<std::vector<State>>& grid, node cell) {
    if (cell.x < grid.size() && cell.x >= 0 && cell.y < grid[0].size() && cell.y >= 0) {
        return grid[cell.x][cell.y] == State::KEmpty;
    }
    return false;
}


bool Compare(node u, node v) {
    return u.g_val + u.h_val > v.g_val + v.h_val;
}

void CellSort(std::vector<node> &v) {
    std::sort(v.begin(), v.end(), Compare);
}

void AddToOpen(node cell, std::vector<std::vector<State>>& grid, std::vector<node>& openlist) {
    openlist.emplace_back(cell);
    grid[cell.x][cell.y] = State::KClosed;
}


void ExpandNeighbours(std::vector<std::vector<State>>& grid, std::vector<node>& openlist, node current, node goal) {
    int directions[][4] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    for (auto pos : directions) {
        node temp = current;
        temp.x += pos[0];
        temp.y += pos[1];
        if (isVaildCell(grid, temp)) {
            temp.g_val = current.g_val + 1;
            temp.h_val = hueristic(temp, goal);
            AddToOpen(temp, grid, openlist);
        }
    }
}

// search the shortest path to the goal
std::vector<std::vector<State>> Search(std::vector<std::vector<State>>& grid, node start, node goal) {
    std::vector<node> openlist;
    node current = {start.x, start.y, 0, hueristic(start, goal)};
    AddToOpen(current, grid, openlist);

    while (!openlist.empty()) {
        CellSort(openlist);
        node current = openlist.back();
        openlist.pop_back();

        if (current.x == start.x && current.y == start.y) {
            grid[current.x][current.y] = State::KStart;
        } else {
            grid[current.x][current.y] = State::KPath;
        }
        if (current.x == goal.x && current.y == goal.y) {
            grid[current.x][current.y] = State::KGoal;
            return grid;
        }

        ExpandNeighbours(grid, openlist, current, goal);
    }

    return std::vector<std::vector<State>>{};
}


int main() {

    node start = {0, 0};
    node goal  = {4, 5};
    std::vector<std::vector<State>> board = ReadBoardFile("maze.txt");
    std::vector<std::vector<State>> solution = Search(board, start, goal);
    PrintSolution(solution);

    return 0;
}
