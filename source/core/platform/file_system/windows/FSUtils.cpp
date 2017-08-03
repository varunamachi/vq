#include <windows.h>
#include <winbase.h>

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
        result = R::stream( false, VQ_TO_ERR( ::GetLastError() ))
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

    auto &path = dir.path();
    WIN32_FIND_DATA fdata;
    auto handle = ::FindFirstFile( path.toString().c_str(), &fdata );
    if( handle == INVALID_HANDLE_VALUE ) {
        auto err = R::stream( false, VQ_TO_ERR( ::GetLastError() ))
                << "List Files: Could not get handle to " << dir.path() \
                << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << err;
    }
    do {
        std::string fileName{ fdata.cFileName };
        if( fileName != "." && fileName != ".." ) {
            auto filePath = path.pathOfChild( fileName );
            File newFile{ filePath };
            if( recursive && newFile.type() == File::Type::Dir ) {
                auto res = list( newFile, recursive, filter, filesOut );
                if( ! res ) {
                    VQ_WARN( "Vq:Core:FS" )
                            << "List Files: Failed to read contents of "
                               "directory" << filePath;
                    continue;
                }
            }
            if( filter == nullptr || filter( newFile )) {
                filesOut.emplace_back( std::move( newFile ));
            }
        }
    } while( ::FindNextFileA( handle, &fdata ));
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


DWORD CALLBACK copyProgressRoutine( LARGE_INTEGER totalFileSize,
                                    LARGE_INTEGER totalBytesTransferred,
                                    LARGE_INTEGER /*StreamSize*/,
                                    LARGE_INTEGER /*StreamBytesTransferred*/,
                                    DWORD         /*dwStreamNumber*/,
                                    DWORD         /*dwCallbackReason*/,
                                    HANDLE        /*hSourceFile*/,
                                    HANDLE        /*hDestinationFile*/,
                                    LPVOID        lpData )
{
    auto pfuc = reinterpret_cast< ProgressFunction *>( lpData );
    std::int64_t total = totalFileSize.QuadPart;
    std::int64_t txd = totalBytesTransferred.QuadPart;
    auto fprog = double( txd ) / total;
    auto contineue = ( *pfuc )( static_cast< std::uint8_t >( fprog ));
    return contineue ? PROGRESS_CONTINUE : PROGRESS_CANCEL;
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
    BOOL cancel = FALSE;
    DWORD flag = COPY_FILE_FAIL_IF_EXISTS;
    auto res = ::CopyFileEx( src.path().toString().c_str(),
                             dst.path().toString().c_str(),
                             &copyProgressRoutine,
                             &progCallback,
                             &cancel,
                             flag );
    auto result = R::success( true );
    if( res == FALSE) {
        result = R::stream( false, VQ_TO_ERR( ::GetLastError() ))
                << "Failed to copy " << src.path() << " to "
                << dst.path() << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << result;
    }
    return result;
}


Result< bool > FSUtils::mkdirImpl( const std::string &path )
{
    auto result = R::success( true );
    BOOL res = ::CreateDirectory( path.c_str(), nullptr );
    if( res != TRUE ) {
        result = R::stream( false, VQ_TO_ERR( ::GetLastError() ))
                << "Failed to create directory at " << path << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << result;
    }
    return result;
}


Result< bool > FSUtils::createSoftLinkImpl( const std::string &target,
                                            const std::string &link )
{
    auto res = R::stream( false )
            << "Symbolic link creation is not implemented for windows platform "
            << target << " --> " << link << R::fail;
    VQ_ERROR( "Vq:Core:FS " ) << res;
    return res;
#if 0
    auto res = R::success( false );
    DWORD flag = 0x00;
    if( File{ target }.type() == File::Type::Dir ) {
        flag = 0x01;
    }
    if( ::CreateSymbolicLink( link.c_str(), target.c_str(), flag ) != TRUE ) {
        res = R::stream( false, VQ_TO_ERR( ::GetLastError() ))
                << "Failed to create symlink to " << target << " at "
                << link << R::fail;
        VQ_ERROR( "Vq:Core:FS" ) << res;
    }
    return res;
#endif
}

}
