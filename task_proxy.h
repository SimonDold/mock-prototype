#ifndef TASK_PROXY_H
#define TASK_PROXY_H

class AbstractTask {
};

class TaskProxy {
    const AbstractTask &task;
public:
    TaskProxy(const AbstractTask &task) : task(task) {
    }
};

#endif
