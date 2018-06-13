#include <bkbase/logging.h>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <string>
#include <boost/thread/thread.hpp>

BOOST_AUTO_TEST_SUITE(log_tests)

void threadLog(const std::string &name) {
    bkbase::LoggingThreadName(name);
    BKLOGSCOPEFUNC();
    BKINFO() << "I'm " << name;
    BKWARN() << "Working...";
    BKTRACE() << "Exiting...";
}

BOOST_AUTO_TEST_CASE(simple) {
    bkbase::LoggingInitialize(LOGTRACE, LOGINFO);
    {
        BKLOGSCOPE("test");
        BKTRACE() << "Trace Message";
        BKDEBUG() << "Debug Message";
    }
    BKINFO() << "Info Message";
    bkbase::LoggingThreadName("main");
    BKWARN() << "Warn Message";
    BKLOGSCOPEFUNC();
    BKERROR() << "Error Message";
    BKCRITICAL() << "Critical Message";

    std::string names[] = {
        "loader", "network", "rpc", "worker", "miner"
    };
    boost::thread_group group;
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        group.create_thread(boost::bind(threadLog , names[i]));
        group.create_thread(boost::bind(threadLog , names[i]));
    }
    group.join_all();
}

BOOST_AUTO_TEST_SUITE_END()
