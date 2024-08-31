//
// Created by cew05 on 23/08/2024.
//

#include "ChunkThreads.h"

/*
 * Only one thread is allowed to create chunks. This should be the chunkBuilder thread, however other threads could
 * instead have this feature enabled.
 */

ChunkThreads::ChunkThreads() = default;

/*
 * Ensures thread rejoins the main thread before program closure.
 */

ChunkThreads::~ChunkThreads() {
    // Stop the thread from running if it is currently enabled and wait for any last processes to finish
    EndThread();
    chunkThread.join();
}



/*
 * Enables and Starts the thread. The thread will be opened until disabled or the ChunkThread class is destroyed.
 * If there is already an ongoing process in the thread, ensures that the thread is disabled and waits for it to
 * complete before restarting.
 */

void ChunkThreads::StartThread() {
    if (!finished) {
        // Update enabled and notify the thread if it is paused
        enabled = false;
        threadCV.notify_one();
        chunkThread.join();
    }

    enabled = true;
    chunkThread = std::thread(&ChunkThreads::ThreadLoop, this);
}



/*
 * Disables the thread. Disabling the thread will require the thread to be resumed via function call later.
 * Disabling the thread will not wait on the thread to finish before proceeding.
 */

void ChunkThreads::EndThread() {
    // Update enabled and notify the thread if it is paused
    enabled = false;
    threadCV.notify_one();

    // Remove remaining tasks
    queueMutex.lock();
    actionQueue.clear();
    queueMutex.unlock();

    // thread will perform final operations and end
}



/*
 * Function provides the constant-running of the thread in the background. However, the thread will pause running when
 * there are no more actions in the queue. Hence, the thread requires notifying of any changes in order to resume
 * operations.
 */

void ChunkThreads::ThreadLoop() {
    finished = false;

    while (enabled) {
        // Prevent thread from running when there are no actions to complete
        std::unique_lock threadLock(threadMutex);
        threadCV.wait(threadLock, [&]{return !enabled || !actionQueue.empty();});

        // Thread has been disabled, so exit loop
        if (!enabled) break;

        // Take and remove chunk at front of queue and prevent queue being updated whilst doing so
        queueMutex.lock();
        ThreadAction currentAction = actionQueue.front();
        actionQueue.pop_front();
        queueMutex.unlock();

        auto st = std::chrono::high_resolution_clock::now();
        currentAction.DoAction();
        auto et = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(et - st).count();
        if (duration > 500000) {
            actionsCompleted++;
            sumNStaken += duration;
        }

        // next action ...

//        if (duration > 50'000000) { // greater than 50ms
//            avgNStaken = sumNStaken / actionsCompleted;
//            printf("NOITCE: LAST ACTION TOOK %llu MS\n", duration / 1000000);
//            printf("%d ACTIONS COMPLETED, %zu REMAIN IN QUEUE | CURRENT AVERAGE MS PER ACTION %llu\n",
//                   actionsCompleted, actionQueue.size(), avgNStaken / 1000000);
//        }

        if (actionQueue.empty() && actionsCompleted > 0) {
            avgNStaken = sumNStaken / actionsCompleted;
            printf("SINCE LAST ACTIONS . . .");
            printf("%d ACTIONS COMPLETED IN %llu MS | AVG MS PER ACTION %llu\n",
                   actionsCompleted, sumNStaken / 1000000, avgNStaken / 1000000);

            actionsCompleted = 0;
            sumNStaken = 0;
        }
    }

    finished = true;
}



/*
 * Actions (which may be a list of 1 action) are added to the end of the thread's action queue.
 */

void ChunkThreads::AddActions(const std::vector<ThreadAction>& _actions) {
    queueMutex.lock();
    for (const auto& action : _actions) {
        actionQueue.push_back(action);
    }
    queueMutex.unlock();

    // Notify thread that chunks have been added if it is waiting on more chunks
    threadCV.notify_one();
}

/*
 * Actions (which may be a list of 1 action) are added to the front of the thread's action queue. Order of the actions
 * in the given vector is retained
 */

void ChunkThreads::AddPriorityActions(const std::vector<ThreadAction>& _actions) {
    queueMutex.lock();
    for (auto riter = _actions.rbegin(); riter != _actions.rend(); riter++) {
        actionQueue.push_front(*riter);
    }
    queueMutex.unlock();

    // Notify thread that chunks have been added if it is waiting on more chunks
    threadCV.notify_one();
}



/*
 * Action is applied to the action's chunk position, and a square radius of chunks around it. Actions are added to the
 * end of the queue
 */

void ChunkThreads::AddActionRegion(const ThreadAction& _originAction, int _radius) {
    queueMutex.lock();
    for (int x = -_radius; x < _radius + 1; x++) {
        for (int z = -_radius; z < _radius + 1; z++) {
            actionQueue.push_back(_originAction);
            actionQueue.back().chunkPos += glm::ivec2{x,z};
        }
    }
    queueMutex.unlock();

    // Notify thread that chunks have been added if it is waiting on more chunks
    threadCV.notify_one();
}



/*
 * Action is applied to the action's chunk position, and a square radius of chunks around it. Actions are added to the
 * front of the queue
 */

void ChunkThreads::AddPriorityActionRegion(const ThreadAction& _originAction, int _radius) {
    queueMutex.lock();
    for (int x = -_radius; x < _radius + 1; x++) {
        for (int z = -_radius; z < _radius + 1; z++) {
            actionQueue.push_front(_originAction);
            actionQueue.front().chunkPos += glm::ivec2{x,z};
        }
    }
    queueMutex.unlock();

    // Notify thread that chunks have been added if it is waiting on more chunks
    threadCV.notify_one();
}