#pragma once
#include <mutex>
#include <queue>

class WorkerTask
{
public:
    virtual ~WorkerTask() = default;
    virtual void runTask() = 0;
};

class WorkerPool
{
public:
    void start();
    void stop();

    bool isDone();
    void waitForCompletion();

    template<typename T, typename... Args>
    void addTask(Args... args)
    {
        T* newTask = new T(args...);
        scheduleTask(newTask);
    }


private:
    void scheduleTask(WorkerTask* task);

private:
    void threadEntry();

    std::mutex taskListLock;
    std::queue<WorkerTask*> taskList;
    unsigned int tasksInProgress = 0;

    std::mutex statusLock;
    bool runThreads = false;
    std::vector<std::thread> threads;
};
