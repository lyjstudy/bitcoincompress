#include <script/stack.h>
#include <script/exception.h>

namespace script {

std::vector<uint8_t> Stack::_PopStack(std::vector<std::vector<uint8_t>> &stack) {
        if (stack.empty()) {
            BK_THROW_SCRIPT(ExecuteError::Stack);
        }
        std::vector<uint8_t> result = std::move(stack.back());
        stack.pop_back();
        return std::move(result);
}

Stack::Stack() {
}

Stack::~Stack() {
}

Stack::Stack(const Stack &other)
    : mStack(other.mStack), mLocalStack(other.mLocalStack) {
}

Stack::Stack(Stack &&other)
    : mStack(other.mStack), mLocalStack(other.mLocalStack) {
}

std::vector<uint8_t> &Stack::Top(int idx) {
    if (!_IsValidIndex(idx)) {
        BK_THROW_SCRIPT(ExecuteError::Stack);
    }
    return mStack.at((int)mStack.size() + idx);
}

void Stack::Erase(int start, int end) {
    if (!_IsValidIndex(start) || !_IsValidIndex(end) || start > end) {
        BK_THROW_SCRIPT(ExecuteError::Stack);
    }
    mStack.erase(mStack.end() + start, mStack.end() + end);
}

void Stack::Swap(int idx1, int idx2) {
    std::swap(Top(idx1), Top(idx2));
}

}
