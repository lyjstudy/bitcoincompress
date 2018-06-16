#pragma once
#include <bkbase/exception.h>

namespace script {

enum class ExecuteError {
    Success,
    Stack,
    Condition,
    BadDataOpCode,
    ReachEnd,
    Unknown,
};

BK_DEFINE_ERROR_INFO(script_execute_error, ExecuteError);

#define BK_THROW_SCRIPT(code) BK_THROW(bkbase::Exception() << script_execute_error(code))

} // bkbase
