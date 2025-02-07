#ifndef TIME_HPP
#define TIME_HPP

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace Time
{

inline std::string getCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

} // namespace Time

#endif // TIME_HPP
