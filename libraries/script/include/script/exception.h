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
BK_DEFINE_ERROR_INFO(script_assemble_error, const char *);
BK_DEFINE_ERROR_INFO(script_assemble_error_position, size_t);

#define BK_THROW_SCRIPT(code) BK_THROW(bkbase::Exception() << script_execute_error(code))
#define BK_THROW_ASSEMBLE(pos, msg) BK_THROW(bkbase::Exception() << \
                script_assemble_error_position(pos) << script_assemble_error(msg))

} // bkbase
