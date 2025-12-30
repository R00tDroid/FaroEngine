#pragma once
#include <mutex>
#include <queue>
#include <thread>

class WorkerTask
{
public:
    virtual ~WorkerTask() = default;

    virtual void execute();
    bool isDone();

protected:
    virtual void runTask() = 0;

private:
    std::mutex statusLock;
    bool complete = false;
};

class WorkerPool
{
public:
    void start();
    void stop();

    bool isDone();
    void waitForCompletion();

    template<typename T, typename... Args>
    WorkerTask* addTask(Args... args)
    {
        T* newTask = new T(args...);
        scheduleTask(newTask);
        return newTask;
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

class TaskGroup
{
public:
    TaskGroup(WorkerPool& pool);
    ~TaskGroup();

    bool isDone() const;

    template<typename T, typename... Args>
    void addTask(Args... args)
    {
        WorkerTask* task = pool.addTask<T>(args...);
        tasks.push_back(task);
    }

private:
    WorkerPool& pool;
    std::vector<WorkerTask*> tasks;
};

