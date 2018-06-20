#include <helper/scan.h>
#include <helper/output_pool.h>
#include <script/opcode.h>
#include <script/type.h>
#include <atomic>
#include <helper/statistics_pool.h>


int main(int argc, char *argv[]) {
    bkbase::LoggingInitialize(LOGCRITICAL, LOGTRACE);
    bkbase::LoggingThreadName("main");
    BKLOGSCOPE("entry");
    helper::StatisticsPool pool;
    helper::ScanOrder scan("/home/liuyujun/.bitcoin/blocks", &pool);

    scan.Start();
    return 0;
}