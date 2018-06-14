#include <boost/thread.hpp>
#include <bkbase/logging.h>
#include <bkbase/exception.h>
#include <bkbase/stream.h>
#include <core/block.h>
#include "block_file.h"

BlockFile *GLoader = nullptr;

void pubkeyMain(std::string name) {
    bkbase::LoggingThreadName(name);
    BKLOGSCOPE("entry");

    std::vector<uint8_t> blockBuffer;
    bkbase::StreamVector stream(0, 0, blockBuffer);

    core::Block block;
    while (GLoader->NextBlock(blockBuffer)) {
        try {
            stream >> block;
            // BKINFO() << block.CalcHash().GetReverseHex() << " : " << block.Transactions()[0]->CalcHash().GetReverseHex();
            // 需要查找一下P2PK和MS和P2PKH的非标准输入有多少
            // 需要查找一下P2SH子类型非标准有多少
            break;
        } catch (bkbase::Exception &ex) {
            BKERROR() << boost::diagnostic_information(ex);
        } catch (std::exception &ex) {
            BKERROR() << ex.what();
        } catch (...) {
            BKERROR() << "Unknown Excpetion";
        }
    }

    BKINFO() << name << " Exited";
}

int main(int argc, char *argv[]) {
    bkbase::LoggingInitialize(LOGCRITICAL, LOGTRACE);
    bkbase::LoggingThreadName("main");

    BlockFile blockFile("/home/liuyujun/.bitcoin/blocks");

    GLoader = &blockFile;

    boost::thread_group group;
    for (size_t i = 0; i < boost::thread::hardware_concurrency() + 1; i++) {
        std::stringstream ss;
        std::string threadName;
        ss << "pubkey";
        ss << i;
        ss >> threadName;

        group.create_thread(boost::bind(pubkeyMain , threadName));
    }
    group.join_all();
    return 0;
}