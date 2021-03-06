 

#include <memory>

#include "../../common/STLUtils.h"
#include "../../common/StringUtils.h"
#include "../../logger/Logging.h"
#include "../../common/Result.h"
#include "Path.h"

namespace Vq {

class Path::Data
{
public:
    Data( const Path *container )
        : m_components()
        , m_absolute( false )
        , m_str( "" )
        , m_container( container )
    {

    }

    Data( const Path *container,
          const std::vector< std::string > &components,
          bool absolute )
        : m_components( components )
        , m_absolute( absolute )
        , m_str( "" )
        , m_container( container )
    {

    }

    Data( const Path *container,
          const std::vector< std::string > &&components,
          bool absolute )
        : m_components( components )
        , m_absolute( absolute )
        , m_str( "" )
        , m_container( container )
    {

    }

    Data( Data &&other )
        : m_components( std::move( other.m_components ))
        , m_absolute( other.m_absolute )
        , m_str( std::move( other.m_str ))
        , m_container( other.m_container )
    {
//        other.m_components.clear();
//        other.m_str = "";
    }

    Data( const Data &other )
        : m_components( other.m_components )
        , m_absolute( other.m_absolute )
        , m_str( other.m_str )
        , m_container( other.m_container )
    {

    }

    Data & operator = ( const Data & other )
    {
        m_components = other.m_components;
        m_absolute = other.m_absolute;
        return *this;
    }

    Data & operator = ( Data &&other )
    {
        m_components = std::move( other.m_components );
        m_absolute = other.m_absolute;
        other.m_components.clear();
        return *this;
    }

    bool isAbsolute() const
    {
        return m_absolute;
    }

    const std::vector< std::string > & components() const
    {
        return m_components;
    }

    std::vector< std::string > & components()
    {
        return m_components;
        m_str = m_container->collapse();
    }

    void assign( std::vector< std::string > &&comp, bool isAbs )
    {
        m_components = comp;
        m_absolute = isAbs;
        m_str = m_container->collapse();
    }

    const std::string & toString() const
    {
        return m_str;
    }

//    std::string toString()
//    {
//        return m_str;
//    }

private:
    std::vector< std::string > m_components;

    bool m_absolute;

    std::string m_str;

    const Path *m_container;
};


Path::Path()
    : m_data( std::make_unique< Data >( this ) )
{

}


Path::Path( const std::vector< std::string > &components, bool absolute )
    : m_data( std::make_unique< Path::Data >( this, components, absolute ))
{
}


Path::Path( const std::vector< std::string > &&components, bool absolute )
    : m_data( std::make_unique< Path::Data >( this, components, absolute ))
{
}


Path::Path( const Path &other )
    : m_data( std::make_unique< Path::Data >( *other.m_data ))
{

}


Path::Path( Path &&other )
    : m_data( std::make_unique< Path::Data >( std::move( *other.m_data )))
{

}


Path::~Path()
{

}


Path & Path::operator = ( const Path &other )
{
    ( * this->m_data ) = ( *other.m_data );
    return *this;
}


Path & Path::operator = ( Path &&other )
{
    ( *this->m_data ) = ( *other.m_data );
    return *this;
}


bool Path::operator == ( const Path &other ) const
{
    bool result = this->m_data->components() == other.m_data->components();
    return result;
}


bool Path::operator ==( const std::string &strPath ) const
{
    bool result = ( toString() == strPath );
    return result;
}

bool Path::operator != ( const Path &other ) const
{
    return ! ( ( *this ) == other );
}


bool Path::operator != ( const std::string strPath ) const
{
    return ! ( ( *this ) == strPath );
}


std::string Path::fileName() const
{
    std::string name{ "" };
    if( isValid() ) {
        auto it = m_data->components().end();
        name = * ( --it );
    }
    return name;
}


std::string Path::extension() const
{
    auto name = fileName();
    auto pos = name.find_last_of( "." );
    if( pos != std::string::npos ) {
        auto ext = name.substr( pos, name.size() - pos );
        return ext;
    }
    return "";
}

std::string Path::baseName() const
{
    auto name = fileName();
    auto pos = name.find_last_of( "." );
    if( pos != std::string::npos ) {
        auto baseName = name.substr( 0, pos );
        return baseName;
    }
    return name;
}


bool Path::isAbsolute() const
{
    return m_data->isAbsolute();
}


const std::vector< std::string > & Path::components() const
{
    return m_data->components();
}


const std::vector<std::string> &Path::components()
{
    return m_data->components();
}


Path & Path::append( const std::string &relative )
{
    if( relative.empty() ) {
        return *this;
    }
    auto result = parse( relative );
    if( result.value() ) {
        for( auto &cmp : result.data() ) {
            m_data->components().emplace_back( std::move( cmp ));
        }
    }
    else {
        VQ_ERROR( "Vq:Core:FS" ) << "Failed to append path " << relative
                                 << ", could not parse the given path";
    }
    return *this;
}

Path & Path::append( const Path &relative )
{
    for( const auto &cmp : relative.components() ) {
        m_data->components().push_back( cmp );
    }
    return *this;
}


Path Path::appended( const Path &other ) const
{
    std::vector< std::string > comps{ this->components() };
    for( const auto &comp : other.components() ) {
        comps.push_back( comp );
    }
    return Path{ comps, this->isAbsolute() };
}


Result< Path > Path::pathOfChild( const std::string &relative ) const
{
    auto compRes = parse( relative );
    auto result = R::success< Path >( Path{} );
    if( compRes.value() ) {
        auto newComps = this->components();
        for( auto &cmp : compRes.data() ) {
            newComps.emplace_back( std::move( cmp ));
        }
        result = R::success< Path >( Path{ newComps, this->isAbsolute() });
    }
    else {
        auto error = R::stream< Path >( Path{} )
                << "Given relative path " << relative << "is invalid "
                << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << error;
    }
    return result;
}


Path Path::parent() const
{
    std::vector< std::string > pcomps;
    auto &comps = m_data->components();
    if( comps.size() > 2 ) {
        std::copy( m_data->components().begin(),
                   m_data->components().end() - 2,
                   pcomps.begin() );

    }
    return Path{ pcomps, isAbsolute() };
}


std::vector< std::string > & Path::mutableComponents()
{
    return m_data->components();
}


void Path::assign( std::vector< std::string > && comp, bool isAbs )
{
    m_data->assign( std::move( comp ), isAbs );
}


Result< Path > Path::mergeWith( const Path &other )
{
    using CmpVec = std::vector< std::string >;
    auto matchMove = []( CmpVec main, CmpVec other ) -> CmpVec::iterator
    {
        auto matchStarted = false;
        auto mit = std::begin( main );
        auto oit = std::begin( other );
        for( ; mit != std::end( main ); ++ mit ) {
            if( *mit == *oit ) {
                ++ oit;
                matchStarted = true;
            }
            else if( matchStarted && *mit != *oit ) {
                oit = std::begin( other );
                matchStarted = false;
            }
        }
        return oit;
    };

    auto res = R::success< Path >( *this );
    if( this->isAbsolute() && other.isAbsolute() ) {
        auto &main = STLUtils::largestOf( components(), other.components() );
        auto &slv = STLUtils::smallestOf( components(), other.components() );
        auto mit = std::begin( main );
        for( auto sit = std::begin( slv ); sit != std::end( slv ); ++ sit ) {
            if( *sit != *mit ) {
                res = R::stream< Path >( *this )
                        << "Failed to merge path, size given paths are absolute"
                           "and are different withing the merge range"
                        << R::fail;
                VQ_ERROR( "Vq:Core:FS" ) << res;
                break;
            }
            ++ mit;
        }
        if( res.value() && ( &main == &( other.components() ))) {
            for( ; mit != std::end( main ); ++ mit ) {
                this->mutableComponents().push_back( *mit );
            }
        }
    }
    else if( other.isAbsolute() ) {
        auto comps = other.components();
        auto oit = matchMove( comps, this->components() );
        for( ; oit != std::end( this->components()); ++ oit ) {
            comps.push_back( *oit );
        }
        this->assign( std::move( comps ), true );
    }
    else {
        //If 'this' is absolute or both are relative 'this''s component is
        //taken as main
        auto oit = matchMove( this->components(), other.components() );
        for( ; oit != std::end( other.components()); ++ oit ) {
            this->mutableComponents().push_back( *oit );
        }
    }
    return res;
}


Result< Path > Path::relativeTo( const Path & other ) const
{
    if( ! ( this->isAbsolute() && other.isAbsolute() )) {
        auto result = R::stream( Path{ std::vector< std::string >{}, false })
                << "Both path should be absolute inorder to create relative "
                   "path" << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << result;
        return result;
    }
    std::vector< std::string > relComps;
    auto sit = std::begin( other.components() );
    auto oit = std::end( this->components() );
    bool mismatched = false;
    for( ; sit != std::end( other.components() ); ++ sit ) {
        if( ! mismatched
                && ( *sit == *oit )
                && oit != std::end( components() )) {
            ++ oit;
        }
        else {
            mismatched = true;
            relComps.emplace_back( ".." );
        }
    }
    for( ; oit != std::end( components() ); ++ oit ) {
        relComps.push_back( *oit );
    }
    return R::success( Path{ relComps, false });
}

const std::string & Path::toString() const
{
    return m_data->toString();
}


std::ostream & operator<<( std::ostream &stream, const Path &path )
{
    stream << path.toString();
    return stream;

}

}
