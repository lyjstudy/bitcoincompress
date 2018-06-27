#include <helper/scan.h>
#include <helper/output_pool.h>
#include <script/opcode.h>
#include <script/type.h>
#include <atomic>
#include <thread>
#include <unordered_set>
#include <helper/statistics_pool.h>
#include "pubkey_pool.h"


int main(int argc, char *argv[]) {
    bkbase::LoggingInitialize(LOGCRITICAL, LOGTRACE);
    bkbase::LoggingThreadName("main");
    BKLOGSCOPE("entry");

    PubKeyPool pool("/home/liuyujun/test/pubkeydb");
    helper::ScanFast scan("/home/liuyujun/.bitcoin/blocks", &pool);
    scan.Start();

    return 0;
}
