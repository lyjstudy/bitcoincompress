#include <script/condition.h>
#include <script/exception.h>

namespace script {

Condition::Condition()
    : mCachedResult(true) {
}

Condition::~Condition() {
}

void Condition::Push(bool condition) {
    mConditions.push_back(condition);
    if (!condition) {
        mCachedResult = false;
    }
}

void Condition::Not() {
    if (mConditions.empty()) {
        BK_THROW_SCRIPT(ExecuteError::Condition);
    }
    mConditions.back() = !mConditions.back();
    _Invalidate();
}

void Condition::Pop() {
    if (mConditions.empty()) {
        BK_THROW_SCRIPT(ExecuteError::Condition);
    }
    mConditions.pop_back();
    _Invalidate();
}

}