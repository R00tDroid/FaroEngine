#include "Worker.hpp"
#include "Utility.hpp"

void WorkerTask::execute()
{
    runTask();

    statusLock.lock();
    complete = true;
    statusLock.unlock();
}

bool WorkerTask::isDone()
{
    statusLock.lock();
    bool done = complete;
    statusLock.unlock();

    return done;
}

void WorkerPool::start()
{
    Utility::PrintLineD("Start workers");

    statusLock.lock();
    runThreads = true;
    statusLock.unlock();

    unsigned int coreCount = std::thread::hardware_concurrency();
    Utility::PrintLineD("Core count: " + std::to_string(coreCount));
    if (coreCount == 0)
    {
        coreCount = 4;
        Utility::PrintLineD("Falling back to core count: " + std::to_string(coreCount));
    }

    for (unsigned int i = 0; i < coreCount; i++)
    {
        threads.push_back(std::thread(&WorkerPool::threadEntry, this));
    }
}

void WorkerPool::stop()
{
    Utility::PrintLineD("Stop workers");

    statusLock.lock();
    runThreads = false;
    statusLock.unlock();

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    threads.clear();

    Utility::PrintLineD("Stopped workers");
}

bool WorkerPool::isDone()
{
    taskListLock.lock();
    bool done = taskList.empty() && tasksInProgress == 0;
    taskListLock.unlock();
    return done;
}

void WorkerPool::waitForCompletion()
{
    while (true)
    {
        if (isDone()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void WorkerPool::scheduleTask(WorkerTask* task)
{
    taskListLock.lock();
    taskList.push(task);
    taskListLock.unlock();
}

void WorkerPool::threadEntry()
{
    Utility::PrintLineD("Start worker");
    while (true)
    {
        statusLock.lock();
        bool stop = !runThreads;
        statusLock.unlock();

        if (stop) break;

        WorkerTask* task = nullptr;

        taskListLock.lock();
        if (!taskList.empty())
        {
            task = taskList.front();
            taskList.pop();
            tasksInProgress++;
        }
        taskListLock.unlock();

        if (task != nullptr)
        {
            task->execute();

            taskListLock.lock();
            tasksInProgress--;
            taskListLock.unlock();
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    Utility::PrintLineD("Stop worker");
}

WorkerGroup::WorkerGroup(WorkerPool& pool) : pool(pool) {}

WorkerGroup::~WorkerGroup()
{
    for (WorkerTask* task : tasks)
    {
        delete task;
    }
    tasks.clear();
}

bool WorkerGroup::isDone() const
{
    for (WorkerTask* task : tasks)
    {
        if (!task->isDone()) return false;
    }
    return true;
}
