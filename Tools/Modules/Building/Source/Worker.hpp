#pragma once
#include <mutex>
#include <queue>

class WorkerTask
{
public:
};

class WorkerPool
{
public:
    void start();
    void stop();

    bool isDone();
    void waitForCompletion();

    void addTask(WorkerTask* task);

private:
    void threadEntry();

    std::mutex taskListLock;
    std::queue<WorkerTask*> taskList;
    std::vector<WorkerTask*> tasksInProgress;

    std::mutex statusLock;
    bool runThreads = false;
    std::vector<std::thread> threads;
};
