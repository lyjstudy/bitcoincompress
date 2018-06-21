#include <bkbase/logging.h>
#include <core/transaction.h>
#include <script/opcode.h>


int main(int argc, char *argv[]) {
    core::Transaction tx;

/*
    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        47,
        { OP_1, OP_1, 1, OP_CAT, }
    ));
    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        49,
        { OP_1, OP_1, 1, OP_NUM2BIN, }
    ));
    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        53,
        { OP_1, OP_1, 1, OP_AND, }
    ));
    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        54,
        { OP_1, OP_1, 1, OP_OR, }
    ));
    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        55,
        { OP_1, OP_2, 1, OP_XOR, }
    ));
    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        50,
        { OP_1, OP_1NEGATE, }
    ));
    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        72,
        { OP_3, OP_2, 1, OP_MOD, }
    ));
*/

    tx.AddInput(core::TransactionInput(
        bkbase::Hash256::From<bkbase::Hash256>("38fe36c532c7336c6db407268803787f168781660b64b7a62a63c83168200618"),
        71,
        { OP_1, OP_1, 1, OP_DIV, }
    ));

    tx.AddOutput(core::TransactionOutput(
        5400,
        {
            OP_DUP, OP_HASH160, 20,
            0x1c, 0x9b, 0xcb, 0xbc, 0x41, 
            0x68, 0x95, 0xbd, 0x5b, 0xd8,
            0xd0, 0x28, 0x04, 0xdd, 0x88,
            0x4b, 0x66, 0x4c, 0x51, 0x90,
            OP_EQUALVERIFY, OP_CHECKSIG
        }
    ));

    bkbase::StreamData stream(bkbase::SER_DISK, 0);
    stream << tx;
    BKINFO() << tx.CalcHash().GetReverseHex();
    BKINFO() << bkbase::HexFromBin(stream.Buffer());

    return 0;
}