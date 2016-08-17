

#include <mutex>

#include "../common/Threading.h"
#include "JobContext.h"



namespace Vq {

class JobContext::Impl
{
public:
    explicit Impl( JobContext::JobProgressCallback callback )
        : m_callback( callback )
        , m_cancelRequested( false )
    {

    }

    inline bool cancelRequested() const
    {
        std::lock_guard< std::mutex > guard( m_lock );
        return m_cancelRequested;
    }

    inline void requestCancel()
    {
        VQ_LOCK( m_lock );
        m_cancelRequested = true;
    }

    JobContext::JobProgressCallback progressCallback()
    {
        return m_callback;
    }

private:
    JobContext::JobProgressCallback m_callback;

    bool m_cancelRequested;

    mutable std::mutex m_lock;
};



JobContext::JobContext( JobContext::JobProgressCallback callback )
    : m_impl( std::make_unique< JobContext::Impl >( callback ))
{

}


bool JobContext::cancelRequested() const
{
    return m_impl->cancelRequested();
}


void JobContext::requestCancel()
{
    return m_impl->requestCancel();
}


void JobContext::setProgress( int totalMilestones,
                              int milestonesCompleted,
                              int currentMilestoneProgress )
{
    auto progFunc = m_impl->progressCallback();
    progFunc( totalMilestones, milestonesCompleted, currentMilestoneProgress );
}


}
