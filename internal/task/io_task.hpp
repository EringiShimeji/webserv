#ifndef INTERNAL_TASK_IO_TASK_HPP
#define INTERNAL_TASK_IO_TASK_HPP

#include "utils/result.hpp"
#include <string>

enum IOTaskResult {
    kTaskSuspend,
    kTaskComplete,
};

class IOTaskManager;

class IOTask {
public:
    IOTask(IOTaskManager &m, int fd);
    virtual ~IOTask();
    virtual Result<IOTaskResult, std::string> execute() = 0;

protected:
    int const fd_;
    IOTaskManager &manager_; // NOLINT(*-avoid-const-or-ref-data-members)
};

#endif //INTERNAL_TASK_IO_TASK_HPP
