/*
cd /Users/moormkf1/Repositories/game-engine
clang++ tests/collision_pair_race_condition_demo.cpp \
  -std=c++20 \
  -pthread \
  -fsanitize=thread \
  -g \
  -O1 \
  -o /tmp/collision_pair_race_condition_demo \
&& /tmp/collision_pair_race_condition_demo
*/

#include <iostream>
#include <thread>
#include <vector>
#include <utility>

struct Position
{
    float x;
    float y;
};

bool areNearby(const Position& a, const Position& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return (dx * dx + dy * dy) < 25.0f;
}

int main()
{
    std::vector<Position> positions = {
        {0, 0},
        {1, 1},
        {10, 10},
        {2, 2},
        {20, 20},
        {3, 3},
        {4, 4},
        {30, 30}
    };

    // All worker threads share this.
    std::vector<std::pair<int, int>> candidatePairs;

    const int workerCount = 4;
    std::vector<std::thread> workers;

    const int entitiesPerWorker =
        static_cast<int>(positions.size()) / workerCount;

    for (int worker = 0; worker < workerCount; ++worker)
    {
        int start = worker * entitiesPerWorker;

        int end = (worker == workerCount - 1)
            ? static_cast<int>(positions.size())
            : start + entitiesPerWorker;

        workers.emplace_back(
            [&positions, &candidatePairs, start, end]()
            {
                for (int i = start; i < end; ++i)
                {
                    for (int j = i + 1;
                         j < static_cast<int>(positions.size());
                         ++j)
                    {
                        if (areNearby(positions[i], positions[j]))
                        {
                            candidatePairs.push_back({i, j});
                        }
                    }
                }
            });
    }

    for (std::thread& worker : workers)
    {
        worker.join();
    }

    std::cout << "Candidate pairs: "
              << candidatePairs.size()
              << std::endl;
}