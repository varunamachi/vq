#pragma once

#include <vector>

#include "../../common/Macros.h"
#include "../../common/Result.h"

#include "File.h"
#include "Path.h"

namespace Vq {

class FSUtils
{
public:
    VQ_MAKE_STATIC( FSUtils );

    using FilterFunction = std::function< bool( const File & )>;

    static Result< File > fileAt( const std::string &path );

    static Result< File > fileAt( const Path &path );

    static Result< bool > createRegularFile( const File &file );

    static Result< bool > deleteFile( const File &file );

    static Result< std::vector< File >> listFiles( const File &dir,
                                                   FilterFunction filter );


private:
};

}
