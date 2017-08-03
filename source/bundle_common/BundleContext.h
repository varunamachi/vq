#pragma once

#include <memory>

#include "AppContext.h"

class QString;

namespace Vq {

class QzAppContext;
class AbstractPluginBundle;
class BundleEnv;
class ConfigManager;
namespace Logger {
    class Logger;
}

namespace Plugin {

class BundleContext
{
public:
    ~BundleContext();

    QzAppContext * appContext() const;

    AbstractPluginBundle * pluginBundle() const;

    BundleEnv * bundleEnv() const;

    static void destroy();

    static void init( std::unique_ptr< AbstractPluginBundle > pluginBundle,
                      std::unique_ptr< BundleEnv > env,
                      QzAppContext *appContext );

    static BundleContext * instance();

private:
    struct Data;
    std::unique_ptr< Data > m_data;

    static std::unique_ptr< BundleContext > s_instance;

    BundleContext( std::unique_ptr< AbstractPluginBundle > pluginBundle,
                   std::unique_ptr< BundleEnv > env,
                   QzAppContext *appContext );
};

}

}


//inline Vq::Plugin::BundleContext * bundleContext()
//{
//    return Vq::Plugin::BundleContext::instance();
//}

//inline Vq::QzAppContext * appContext()
//{
//    return Vq::Plugin::BundleContext::instance()->appContext();
//}

//inline Vq::BundleEnv * bundleEnv()
//{
//    return Vq::Plugin::BundleContext::instance()->bundleEnv();
//}

//inline Vq::AbstractPluginBundle * pluginBundle()
//{
//    return bundleContext()->pluginBundle();
//}

//inline Vq::ConfigManager * confman()
//{
//    return bundleContext()->appContext()->configManager();
//}
