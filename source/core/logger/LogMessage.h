#pragma once

#include <cstdint>
#include <memory>




namespace Vq {

class Timestamp;

namespace Logger {

enum class VqLogLevel : int;

class LogMessage
{
public:
    LogMessage( const Timestamp &&time,
                VqLogLevel level,
                std::uint64_t threadId,
                const std::string &module,
                const std::string &&method,
                int lineNum,
                const std::string &message );

    LogMessage( const Timestamp &&time,
                VqLogLevel level,
                std::uint64_t threadId,
                const std::string &module,
                const std::string &&method,
                int lineNum,
                std::string &&message );

    LogMessage( const Timestamp &&time,
                VqLogLevel level,
                std::uint64_t threadId,
                const std::string &&module,
                const std::string &&method,
                int lineNum,
                std::string &&message );

    const VqLogLevel & logLevel() const;

    const Timestamp & time() const;

    const std::uint64_t & threadId() const;

    const std::string & moduleName() const;

    const std::string & methodName() const;

    const int & lineNum() const;

    const std::string & message() const;

    void setMessage( const std::string &message );

    void setMessage( std::string &&message );

    ~LogMessage();

private:
    class Impl;
    std::unique_ptr< Impl > m_impl;
};

} }
