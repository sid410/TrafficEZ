#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp> // Needed for timestamps
#include <boost/log/utility/setup/console.hpp>
#include <string>

namespace Logging
{

inline void setupLogging()
{
    static bool initialized = false;
    if(initialized)
        return; // Prevent multiple initializations
    initialized = true;

    boost::log::add_console_log(std::cout,
                                boost::log::keywords::format =
                                    "[%TimeStamp%] [%Severity%] %Message%");

    boost::log::
        add_common_attributes(); // Add timestamps and thread ID attributes
}

} // namespace Logging

#endif // LOGGING_HPP
