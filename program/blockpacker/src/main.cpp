#include <stdio.h>
#include <unordered_map>
#include <block.h>
#include <block_loader.h>
#include "block_pool.h"


int main(int argc, char *argv[]) {
    
    utils::BlockLoader loader("/home/liuyujun/.bitcoin/blocks/");

    BlockPool pool;

    for (size_t i = 0; i < 600000; i++) {
        if (i % 50000 == 0) {
            printf("processing: %d\n", (int)i);
            pool.Dump();
        }
        if (!loader.NextBlock([&]() {
            core::Block *blk = new core::Block();
            loader.ReadBlock(*blk);
            pool.AddBlock(blk);
        })) {
            break;
        }
    }
    return 0;
}