//
// Created by cew05 on 23/08/2024.
//

#ifndef UNTITLED7_CHUNKTHREADS_H
#define UNTITLED7_CHUNKTHREADS_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include <queue>
#include <functional>
#include <chrono>

#include "Chunk.h"

/*
 * Simple struct to house a position of the chunk that the given function will be applied to.
 */

struct ThreadAction {
    std::function<void(const glm::ivec2&, const glm::vec3&)> function {};
    glm::ivec2 chunkPos {0, 0};
    glm::vec3 chunkBlock {0, 0, 0};

    // Functionality
    void DoAction() const { function(chunkPos, chunkBlock); }
};

/*
 *
 */

class ChunkThreads {
    protected:
        std::deque<ThreadAction> actionQueue {};
        std::mutex queueMutex;

        // Thread management
        std::condition_variable threadCV;
        std::mutex threadMutex;
        bool enabled = false;
        bool finished = true;

        // Thread functionality
        void ThreadLoop();
        std::thread chunkThread;

        // action time measurements
        int actionsCompleted = 0;
        Uint64 avgNStaken = 0.0;
        Uint64 sumNStaken = 0.0;

    public:
        ChunkThreads();
        ~ChunkThreads();

        void StartThread();
        void EndThread();

        void AddActions(const std::vector<ThreadAction>& _actions);
        void AddPriorityActions(const std::vector<ThreadAction>& _actions);
        void AddActionRegion(const ThreadAction& _originAction, int _radius);
        void AddPriorityActionRegion(const ThreadAction& _originAction, int _radius);

        [[nodiscard]] bool HasActions() const {
            return !actionQueue.empty();
        }
};

#endif //UNTITLED7_CHUNKTHREADS_H
