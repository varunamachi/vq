#include <windows.h>

#include "../../../logger/Logging.h"
#include "../FSUtils.h"

namespace Vq {

Result< bool > FSUtils::deleteFile( const File &file )
{
    const auto &path = file.path();
    File parent{ path.parent() };
    if( ! file.exists() ) {
        auto error = R::stream( false )
                << "Could not delete file at " << path
                << ", file does not exist" << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << error;
        return error;
    }
    else if( ! parent.isWritable() ) {
        auto error = R::stream( false )
                << "Parent of file " << parent.path().fileName()
                << "that is to be deleted is not writable" << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << error;
        return error;
    }
    auto result = R::success( true );
    if( unlink( path.toString().c_str() ) != 0 )  {
        result = R::stream( false, errno )
                << "Could not delete file at " << path
                << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << result;
    }
    return result;
}


static Result< bool > list( const File &dir,
                            bool recursive,
                            FSUtils::FilterFunction filter,
                            VQ_OUT FSUtils::FileList &filesOut )
{
    auto result = R::success( true );
    if( ! dir.exists() ) {
        result = R::stream( false )
                << "List files: directory at " << dir
                << " does not exist" << R::fail;
    }
    else if( dir.type() != File::Type::Dir ) {
        result = R::stream( false )
                << "List files: File at " << dir
                << " is not a directory" << R::fail;
    }
    else if( ! dir.isReadable() ) {
        result = R::stream( false )
                << "List files: directory at " << dir
                << " is not readable" << R::fail;
    }
    if( ! result ) {
        VQ_ERROR( "Vq:Core:FS" ) << result;
        return result;
    }

//    auto &path = dir.path();
//    auto dirp = ::opendir( dir.path().toString().c_str() );
//    struct dirent *dirPtr;
//    if( dirp != nullptr ) {
//        //should it be readdir64?
//        while(( dirPtr = ::readdir( dirp )) != nullptr ) {
//            auto newPath = path.pathOfChild( dirPtr->d_name ).data();
//            File newFile{ newPath };
//            if( recursive && newFile.type() == File::Type::Dir ) {
//                auto res = list( newFile, recursive, filter, filesOut );
//                if( ! res ) {
//                    //Or we could return
//                    continue;
//                }
//            }
//            if( filter == nullptr || filter( newFile )) {
//                filesOut.emplace_back( std::move( newFile ));
//            }
//        }
//    }
//    else {
//        result = R::stream( false, errno )
//                << "Failed to stat the directory at " << dir
//                << R::fail;
//    }
    return result;
}


Result< FSUtils::FileList > FSUtils::listFiles(
        const File &dir,
        bool recursive,
        FSUtils::FilterFunction filter,
        FileListResultFunc resultCallback )
{
    FileList files;
    auto rz = list( dir, recursive, filter, files );
    auto result = R::success< FileList >( std::move( files ));
    if( ! rz ) {
        result = R::failure< FileList >( FileList{ }, std::move( rz ));
    }
    resultCallback( result );
    return result;
}

Result< bool > FSUtils::copyFileImpl( const File &src,
                                      const File &dst,
                                      ProgressFunction progCallback )
{
    auto destParentPath = dst.path().parent();
    //Create directories in destination path if they dont exist
    auto dirCreateRes = createDirecties( destParentPath.toString() );
    if( ! dirCreateRes ) {
        auto err = R::stream( false )
                << "Could not copy " << src << " to "
                << dst << " due to error while creating "
                << " directories in destination path, Error: "
                << dirCreateRes.reason() << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << err;
        return err;
    }
    return R::failure< bool >();
}


Result< bool > FSUtils::mkdirImpl( const std::string &path )
{
    auto result = R::success( true );
//    auto res = ::mkdir( path.c_str(), 0777 );
//    if( res != 0 ) {
//        result = R::stream( false, errno )
//                << "Failed to create directory at " << path << R::fail;
//        VQ_ERROR( "Vq:Core:FS" ) << result;
//    }
    return result;
}


Result< bool > FSUtils::createSoftLinkImpl( const std::string &target,
                                            const std::string &link )
{
    auto res = R::success( false );
    return res;
}

}
