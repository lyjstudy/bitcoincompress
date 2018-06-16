#pragma once

#include <boost/thread.hpp>
#include <bkbase/logging.h>
#include <bkbase/exception.h>
#include <bkbase/stream.h>
#include <script/type.h>
#include <script/parser.h>
#include <core/block.h>
#include "block_file.h"

template<typename POOL>
class Scan {
protected:
    POOL mPool;
    BlockFile mLoader;

    void _ThreadProc(std::string name) {
        bkbase::LoggingThreadName(name);
        BKLOGSCOPE("entry");

        std::vector<uint8_t> blockBuffer;

        core::Block block;
        while (mLoader.NextBlock(blockBuffer)) {
            try {
                bkbase::StreamVector stream(bkbase::SER_DISK, 0, blockBuffer);
                stream >> block;
                auto &txs = block.Transactions();
                for (auto i = txs.begin(); i != txs.end(); ++i) {
                    auto tx = *i;
                    bkbase::Hash256 txHash = tx->CalcHash();
                    auto &outputs = tx->Outputs();
                    for (size_t i = 0; i < outputs.size(); i++) {
                        mPool.AddOutput(txHash, (uint16_t)i, outputs[i].LockScript(), outputs[i].Amount());
                    }
                    // ignore coinbase
                    if (i != txs.begin()) {
                        auto &inputs = tx->Inputs();
                        for (auto i = inputs.begin(); i != inputs.end(); ++i) {
                            mPool.AddInput(i->TxInID(), (uint16_t)i->TxInIndex(), i->UnlockScript());
                        }
                    }
                }
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
public:
    Scan(const char *path) : mLoader(path) {
    }

    void Start() {
        boost::thread_group group;
        for (size_t i = 0; i < boost::thread::hardware_concurrency() + 1; i++) {
            std::stringstream ss;
            std::string threadName;
            ss << "scan";
            ss << i;
            ss >> threadName;

            group.create_thread(boost::bind(&Scan::_ThreadProc, this, threadName));
        }
        mLoader.LoadLoop();
        group.join_all();
        mPool.End();
    }
};
