#pragma once

#include <script/stack.h>
#include <script/condition.h>
#include <script/parser.h>
#include <script/exception.h>

namespace script {

class Machine {
protected:
    Stack mStack;
    Condition mCondition;
    Parser mParser;
    ExecuteError mError;
    size_t mOpCounter;

    inline void _Init(bool clearStack) {
        if (clearStack)
            mStack.Clear();
        else
            mStack.ClearLocal();
        mCondition.Clear();
        mError = ExecuteError::Success;
        mOpCounter = 0;
    }
    inline ExecuteError _SetError(ExecuteError err) {
        mError = err;
        return err;
    }
protected:
    virtual ExecuteError ExecuteStart(uint8_t opcode, const std::vector<uint8_t> &data) {
        return ExecuteError::Success;
    }
public:
    Machine();
    ~Machine();

    void SetScript(const std::vector<uint8_t> &program, bool clearStack = true);
    void SetScript(std::vector<uint8_t> &&program, bool clearStack = true);

    ExecuteError Step();
    ExecuteError Continue();

};

}