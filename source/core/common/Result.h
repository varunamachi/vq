#pragma once

#include <string>
#include <sstream>

#include "Macros.h"

namespace Vq {

using ErrorCodeType = std::int64_t;

#define VQ_TO_ERR( code ) static_cast< ErrorCodeType >(( code ))

template< typename ReturnType >
class Result
{
public:

    Result()
        : m_result( false )
        , m_data()
        , m_reason( "Unknown error!" )
        , m_errorCode( 0 )
    {

    }

    Result( Result< ReturnType > &&other )
        : m_result( other.value() )
        , m_data( std::move( other.data() ))
        , m_reason( std::move( other.reason() ))
        , m_errorCode( std::move( other.errorCode() ))
    {

    }

    Result( const Result< ReturnType > &other )
        : m_result( other.value() )
        , m_data( other.data() )
        , m_reason( other.reason() )
        , m_errorCode( other.errorCode() )
    {

    }

    Result( bool result,
            ReturnType data,
            std::string &&reason,
            ErrorCodeType errorCode )
        : m_result( result )
        , m_data( std::move( data ))
        , m_reason( std::move( reason ))
        , m_errorCode( errorCode )
    {

    }

    Result( bool result,
            ReturnType data,
            const std::string &reason,
            ErrorCodeType errorCode )
        : m_result( result )
        , m_data( data )
        , m_reason( reason )
        , m_errorCode( errorCode )
    {

    }

    Result( bool result,
            ReturnType &&data,
            const std::string &reason,
            ErrorCodeType errorCode )
        : m_result( result )
        , m_data( std::move( data ))
        , m_reason( reason )
        , m_errorCode( errorCode )
    {

    }

    Result( bool result,
            ReturnType &&data,
            const std::string &&reason,
            ErrorCodeType errorCode )
        : m_result( result )
        , m_data( std::move( data ))
        , m_reason( std::move( reason ))
        , m_errorCode( errorCode )
    {

    }

    ~Result() { }

    bool operator==( const Result< ReturnType > &other ) const
    {
        bool same = this == &other
                || ( other.value() == this->m_result
                     && other.data() == this->m_data
                     && other.reason() == this->m_reason );
        return same;
    }

    bool operator != ( const Result<ReturnType > &other ) const
    {
        return ! ( *this == other );
    }

    bool operator == ( const ReturnType &other ) const
    {
        bool same = ( m_data == other );
        return same;
    }

    bool operator != ( const ReturnType &other ) const
    {
        return ! ( *this == other );
    }


    operator ReturnType() const
    {
        return m_data;
    }


    Result & operator = ( const Result< ReturnType > &other )
    {
        if( this != &other ) {
            this->m_data = other.data();
            this->m_result = other.value();
            this->m_reason = other.reason();
        }
        return *this;
    }

    Result & operator = ( Result< ReturnType > &&other )
    {
        if( this != &other ) {
            this->m_data = std::move( other.data() );
            this->m_result = other.value();
            this->m_reason = std::move( other.reason() );
        }
        return *this;
    }

//    explicit operator bool() const
//    {
//        return m_result;
//    }

//    bool operator ! () const
//    {
//        return ! m_result;
//    }

    bool value() const
    {
        return m_result;
    }

    const std::string & reason() const
    {
        return m_reason;
    }

    std::string & reason()
    {
        return m_reason;
    }

    const ReturnType & data() const
    {
        return m_data;
    }

    ReturnType & data()
    {
        return m_data;
    }

    ErrorCodeType errorCode() const
    {
        return m_errorCode;
    }

private:
    bool m_result;

    ReturnType m_data;

    std::string m_reason;

    ErrorCodeType m_errorCode;
};



template< typename ReturnType >
std::ostream & operator << ( std::ostream &stream,
                             Result< ReturnType > &result )
{
    stream << result.reason();
    if( result.errorCode() != 0 ) {
        stream << " - ErrorCode: " << result.errorCode();
    }
    return stream;
}


template< typename ReturnType > class RStream;

struct R {
    VQ_MAKE_STATIC( R );

//################# Success
    template< typename ReturnType >
    static Result< ReturnType > success( const ReturnType &data )
    {
        return Result< ReturnType >( true, data, "", 0 );
    }

    template< typename ReturnType >
    static Result< ReturnType > success( ReturnType &&data )
    {
        return Result< ReturnType >( true, std::move( data ), "",  0 );
    }


//################# Failure
    template< typename ReturnType >
    static Result< ReturnType > failure()
    {
        return Result< ReturnType >( false,
                                     ReturnType{},
                                     "",
                                     0 );
    }


    template< typename ReturnType >
    static Result< ReturnType > failure( const ReturnType &data,
                                         std::string &&reason,
                                         ErrorCodeType errorCode = 0 )
    {
        return Result< ReturnType >( false,
                                     data,
                                     std::move( reason ),
                                     errorCode );
    }

    template< typename ReturnType >
    static Result< ReturnType > failure( ReturnType &&data,
                                         std::string &&reason,
                                         ErrorCodeType errorCode = 0 )
    {
        return Result< ReturnType >( false,
                                     std::move( data ),
                                     std::move( reason ),
                                     errorCode );
    }


//################# Forward
    template< typename ReturnType, typename CauseReturnType >
    static Result< ReturnType > failure( ReturnType &data,
                                         Result< CauseReturnType > &&cause )
    {
        return R::failure< ReturnType >( data,
                                         std::move( cause.reason() ),
                                         cause.errorCode() );
    }

    template< typename ReturnType, typename CauseReturnType >
    static Result< ReturnType > failure( ReturnType &&data,
                                         Result< CauseReturnType > &&cause )
    {
        return R::failure< ReturnType >( std::move( data ),
                                         std::move( cause.reason() ),
                                         cause.errorCode() );
    }


//################# Stream
    template< typename ReturnType >
    static RStream< ReturnType > stream( ReturnType r,
                                         ErrorCodeType errorCode = 0 )
    {
        return RStream< ReturnType >( std::move( r ), errorCode );
    }

    struct FailureTag { };

    struct SuccessTag { };

    static const FailureTag fail;

    static const SuccessTag succeed;
};





template< typename ReturnType >
class RStream
{
public:
    VQ_NO_COPY( RStream );

    typedef std::ostream& ( Manip )( std::ostream& );

    RStream( const ReturnType &data, ErrorCodeType errorCode = 0 )
        : m_data( data )
        , m_errorCode( errorCode )
    {

    }

    RStream( ReturnType &&data, ErrorCodeType errorCode = 0 )
        : m_data( std::move( data ))
        , m_errorCode( errorCode )
    {

    }

    RStream( RStream &&other )
        : m_data( std::move( other.m_data ))
        , m_errorCode( other.m_errorCode )
        , m_stream( std::move( other.m_stream ))
    {
        other.m_errorCode = 0;
    }

    template< typename T >
    RStream & operator << ( const T &obj )
    {
        m_stream << obj;
        return  *this;
    }

    RStream & operator << ( Manip &manip )
    {
        m_stream << manip;
        return *this;
    }

    Result< ReturnType > operator << ( R::FailureTag )
    {
        return Result< ReturnType >( false,
                                     std::move( m_data ),
                                     m_stream.str(),
                                     m_errorCode );
    }

    Result< ReturnType > operator << ( R::SuccessTag )
    {
        return Result< ReturnType >( true,
                                     std::move( m_data ),
                                     m_stream.str(),
                                     m_errorCode );
    }


private:
    ReturnType m_data;

    ErrorCodeType m_errorCode;

    std::stringstream m_stream;
};




}


