

#include "../../../logger/Logging.h"
#include "../FSUtils.h"

namespace Vq {


Result< bool > FSUtils::deleteFile( const File &file )
{
    return R::failure< bool >();
}


Result< FSUtils::FileList > FSUtils::listFiles(
        const File &dir,
        FSUtils::FilterFunction filter,
        FileListResultFunc resultCallback )
{
    return R::failure< FSUtils::FileList >();
}


Result< bool > FSUtils::createSoftLink( const std::string &targetPath,
                                        const std::string &linkPath )
{
    return R::failure< bool >();
}



Result< bool > FSUtils::deleteDir( const std::string &path,
                                   const bool force )
{
    return R::failure< bool >();
}


Result< bool > FSUtils::copyFileImpl( const File &src,
                                      const File &dst,
                                      ProgressFunction progCallback )
{
    return R::failure< bool >();
}


Result< bool > FSUtils::mkdirImpl( const std::string &path )
{
    return R::failure< bool >();
}

}
