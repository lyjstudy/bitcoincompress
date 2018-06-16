#pragma once

#include <vector>
#include <algorithm>

namespace script
{
    
class Condition {
protected:
    std::vector<bool> mConditions;
    bool mCachedResult;

    inline void _Invalidate() {
        mCachedResult = (std::find(mConditions.begin(), mConditions.end(), false) == mConditions.end());
    }
public:
    Condition();
    ~Condition();
    // OP_IF
    void Push(bool condition);
    // OP_NOTIF
    inline void PushNot(bool condition) { Push(!condition); }
    // OP_ELSE
    void Not();
    // OP_ENDIF
    void Pop();
    inline bool Result() const { return mCachedResult; }
    inline bool Empty() const { return mConditions.empty(); }
    inline void Clear() { mConditions.clear(); mCachedResult = true; }
};

} // script
