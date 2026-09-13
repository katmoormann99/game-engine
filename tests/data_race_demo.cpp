//============================================================================
// Author: Kat Moormann
// File: data_race_demo.cpp
// Purpose: I need to visulize the error with parallelzing threads without thinking 
// about the read and write
// Date: August 15 2025
//============================================================================


#include <iostream> 
#include <thread>
#include <vector>

/*
    CONCEPTUALLY
    Each thread does: 

    read sharedCounter
    add 1
    write sharedCounter

    WITH 8 THREADS DOING THAT AT THE SAME TIME, THEIR OPERATIONS INTERLEAVE
    Two threads can both read the same old value before either writes the new one! 

    For Example:
    sharedCounter = 100

    Thread A             Thread B
    read 100             read 100
    add 1 → 101          add 1 → 101
    write 101            write 101

    WE EXPECTED THE COUNTER TO BECOME 102, BUT IT ONLY BECAME 101 - ONE INCREMENT WAS LAST 

    Solution: MUTEX
    A mutex protects shared data by allowing only one thread at a time to enter a critical
    section and modifiy that data. Other threads trying to acquire the same mutex must wait until
    the current thread unlocks it, preventing concurrent writes and data races!
*/

int main() 
{
    int sharedCounter = 0;

    std::mutex counterMutex;

    const int workerCount = 8;
    const int incrementsPerWorker = 1'000'000;

    std::vector<std::thread> workers;

    for (int worker = 0; worker < workerCount; ++worker)
    {
        // workers.emplace_back([&sharedCounter, &counterMutex, incrementsPerWorker]()
        workers.emplace_back([&sharedCounter, incrementsPerWorker]()
        {
            for (int i = 0; i < incrementsPerWorker; ++i)
            {
                // std::lock_guard<std::mutex> lock(counterMutex);
                ++sharedCounter; //INTENTIONALLY UNSAFE
            }
        });
    }

    for (std::thread &worker : workers)
    {
        worker.join();
    }

    const int expected = workerCount * incrementsPerWorker;

    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Actual: " << sharedCounter << std::endl;
    return 0;
}