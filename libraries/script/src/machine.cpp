#include <script/machine.h>
#include <script/logging.h>

namespace script {

Machine::Machine()
    : mError(ExecuteError::Success), mOpCounter(0) {

}

Machine::~Machine() {
}

void Machine::SetScript(const std::vector<uint8_t> &program, bool clearStack) {
    _Init(clearStack);
    mParser.Reset(program);
}

void Machine::SetScript(std::vector<uint8_t> &&program, bool clearStack) {
    _Init(clearStack);
    mParser.Reset(program);
}

ExecuteError Machine::Step() {
    if (mError != ExecuteError::Success)
        return mError;

    uint8_t opcode;
    std::vector<uint8_t> data;
    if (!mParser.Fetch(opcode, &data)) {
        if (opcode == OP_INVALIDOPCODE)
            return _SetError(ExecuteError::ReachEnd);
        return _SetError(ExecuteError::BadDataOpCode);
    }

    if (_SetError(ExecuteStart(opcode, data)) != ExecuteError::Success) {
        return mError;
    }

    try {
        
    } catch (bkbase::Exception &ex) {
        BKERROR() << boost::diagnostic_information(ex);
        if (boost::get_error_info<script_execute_error>(ex)) {
            _SetError(*boost::get_error_info<script_execute_error>(ex));
        } else {
            _SetError(ExecuteError::Unknown);
        }
    } catch (...) {
        BKERROR() << "Unknown Execution Exception";
        _SetError(ExecuteError::Unknown);
    }
    return mError;
}

}