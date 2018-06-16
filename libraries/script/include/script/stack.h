#pragma once

#include <stdint.h>
#include <vector>

namespace script {

class Stack {
    protected:
        std::vector<std::vector<uint8_t>> mStack;
        std::vector<std::vector<uint8_t>> mLocalStack;

        static std::vector<uint8_t> _PopStack(std::vector<std::vector<uint8_t>> &stack);
        
        inline bool _IsValidIndex(int idx) {
            return idx < 0 && (mStack.size() >= (std::vector<uint8_t>::size_type)-idx);
        }
    public:
        Stack();
        ~Stack();
        Stack(const Stack &);
        Stack(Stack &&);

        inline std::vector<uint8_t>::size_type Size() const { return mStack.size() + mLocalStack.size(); }
        inline void Clear() { mLocalStack.clear(); mStack.clear(); }
        inline bool Empty() const { return mLocalStack.empty() && mStack.empty(); }

        inline void Push(const std::vector<uint8_t> &data) { mStack.push_back(data); }
        inline void Push(std::vector<uint8_t> &&data) { mStack.push_back(data); }
        inline std::vector<uint8_t> Pop() { return _PopStack(mStack); }
        std::vector<uint8_t> &Top(int idx = -1);

        inline void ClearLocal() { mLocalStack.clear(); }
        inline void ToLocal() { mLocalStack.push_back(std::move(_PopStack(mStack))); }
        inline void FromLocal() { mStack.push_back(std::move(_PopStack(mLocalStack))); }

        void Erase(int start, int end);
        void Swap(int idx1, int idx2);
};

}