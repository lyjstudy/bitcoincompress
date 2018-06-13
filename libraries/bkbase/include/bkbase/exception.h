#pragma once
#include <boost/exception/all.hpp>

namespace bkbase {

/*
    BK_DEFINE_ERROR_INFO(detail_info, int);
    BK_THROW(Exception() << detail_info(0));
    catch(Exception &x)
        std::cerr << boost::diagnostic_information(x) << std::endl;
*/
struct Exception : virtual boost::exception, virtual std::exception {};

#define BK_THROW BOOST_THROW_EXCEPTION

#define BK_DEFINE_ERROR_INFO(name, type) \
    using name = boost::error_info<struct tag_##name, type>

} // bkbase
