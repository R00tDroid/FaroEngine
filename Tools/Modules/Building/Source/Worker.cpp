#include "Worker.hpp"
#include "Utility.hpp"

void WorkerPool::start()
{
    Utility::PrintLineD("Start workers");

    statusLock.lock();
    runThreads = true;
    statusLock.unlock();

    for (int i = 0; i < 8; i++)
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
            task->runTask();

            delete task;

            taskListLock.lock();
            tasksInProgress--;
            taskListLock.unlock();
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    Utility::PrintLineD("Stop worker");
}
