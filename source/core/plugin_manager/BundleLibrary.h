#pragma once

#include <memory>

#include "../Vq.h"

namespace Vq {

class PluginBundle;
class SharedLibrary;

class VQ_API BundleLibrary
{
public:
    BundleLibrary();

    BundleLibrary( std::unique_ptr< SharedLibrary > library,
                   std::unique_ptr< PluginBundle > bundle );

    SharedLibrary * library() const;

    PluginBundle * bundle() const;

    bool isValid() const;

    ~BundleLibrary();

private:
    class Impl;
    std::unique_ptr< Impl > m_impl;


};


}
