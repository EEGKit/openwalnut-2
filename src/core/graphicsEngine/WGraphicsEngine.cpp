//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <stdlib.h>

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>
#include <osgViewer/Viewer>

#include "../common/WColor.h"
#include "../common/WLogger.h"
#include "../common/WPathHelper.h"
#include "../common/exceptions/WNameNotUnique.h"
#include "WGEViewer.h"
#include "exceptions/WGEInitFailed.h"
#include "exceptions/WGESignalSubscriptionFailed.h"
#include "WGraphicsEngineMode.h"
#include "postprocessing/WGEPostprocessor.h"

#include "WStaticOSGSetup.h"

#include "WGraphicsEngine.h"

// graphics engine instance as singleton
boost::shared_ptr< WGraphicsEngine > WGraphicsEngine::m_instance = boost::shared_ptr< WGraphicsEngine >();

WGraphicsEngine::WGraphicsEngine():
    WThreadedRunner()
{
    WLogger::getLogger()->addLogMessage( "Initializing Graphics Engine", "GE", LL_INFO );

#ifdef WGEMODE_MULTITHREADED
    // initialize OSG render window
    m_viewer = osg::ref_ptr<osgViewer::CompositeViewer>( new osgViewer::CompositeViewer() );
    m_viewer->setThreadingModel( osgViewer::ViewerBase::SingleThreaded );
#endif

    m_viewer->setRunMaxFrameRate( 60.0 );

    // initialize members
    m_rootNode = new WGEScene();

    m_viewersUpdate = false;

    setThreadName( "WGE" );
}

WGraphicsEngine::~WGraphicsEngine()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Graphics Engine", "GE", LL_INFO );
}

void WGraphicsEngine::setMultiThreadedViews( bool enable )
{
#ifdef  WGEMODE_SINGLETHREADED
    if( enable )
    {
        WLogger::getLogger()->addLogMessage( "WGraphicsEngine::setMultiThreadedViews not implemented for single threaded mode", "GE", LL_INFO );
    }
#else
    // ThreadingModel: enum with the following possibilities
    //
    //  SingleThreaded
    //  CullDrawThreadPerContext
    //  ThreadPerContext
    //  DrawThreadPerContext
    //  CullThreadPerCameraDrawThreadPerContext
    //  ThreadPerCamera
    //  AutomaticSelection
    if( !enable )
    {
        m_viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );
    }
    else
    {
        m_viewer->setThreadingModel( osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext );
    }
#endif
}

bool WGraphicsEngine::isMultiThreadedViews() const
{
#ifdef WGEMODE_MULTITHREADED
    return ( osgViewer::Viewer::SingleThreaded != m_viewer->getThreadingModel() );
#endif
    // on mac, this always is false currently
    return false;
}

boost::shared_ptr< WGraphicsEngine > WGraphicsEngine::getGraphicsEngine()
{
    if( !m_instance )
    {
        m_instance = boost::shared_ptr< WGraphicsEngine >( new WGraphicsEngine() );
    }

    return m_instance;
}

osg::ref_ptr<WGEScene> WGraphicsEngine::getScene()
{
    return m_rootNode;
}

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer( std::string name, osg::ref_ptr<osg::Referenced> wdata, int x, int y,
                                                            int width, int height, WGECamera::ProjectionMode projectionMode,
                                                            WColor bgColor )
{
    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>(
        new WGEViewer( name, wdata, x, y, width, height, projectionMode ) );
    viewer->setBgColor( bgColor );
    viewer->setScene( getScene() );

    // store it in viewer list
    boost::unique_lock< boost::shared_mutex > lock( m_viewersLock );
    bool insertSucceeded = m_viewers.insert( make_pair( name, viewer ) ).second;
    if( !insertSucceeded ) // if false, viewer with same name already exists
    {
        throw WNameNotUnique( "Viewer names need to be unique." );
    }

#ifdef WGEMODE_MULTITHREADED
    WCondition::SPtr viewerUpdateCondition( new WConditionOneShot() );
    // finally add view (in the GE thread)
    m_addViewers.push_back( viewer );
    m_viewersUpdate = true;
#else
    m_viewer->addView( viewer->getView() );
#endif
    lock.unlock();

    return viewer;
}

void WGraphicsEngine::closeViewer( boost::shared_ptr< WGEViewer > viewer )
{
    boost::unique_lock< boost::shared_mutex > lock( m_viewersLock );

    // close and erase
    viewer->close();
    if( m_viewers.count( viewer->getName() ) > 0 )
    {
        m_viewers.erase( viewer->getName() );
    }

#ifdef WGEMODE_MULTITHREADED
    WCondition::SPtr viewerUpdateCondition( new WConditionOneShot() );
    m_viewerUpdateNotifiers.push_back( viewerUpdateCondition );
    // finally remove view (in the GE thread)
    m_removeViewers.push_back( viewer );
    m_viewersUpdate = true;
#else
    m_viewer->removeView( viewer->getView() );
#endif

    lock.unlock();

#ifdef WGEMODE_MULTITHREADED
    // wait until the GE thread processed our request.
    viewerUpdateCondition->wait();
#endif
}

void WGraphicsEngine::closeViewer( const std::string name )
{
    boost::unique_lock< boost::shared_mutex > lock( m_viewersLock );
    if( m_viewers.count( name ) > 0 )
    {
        m_viewers[name]->close();
        m_viewers.erase( name );

#ifdef WGEMODE_MULTITHREADED
        WCondition::SPtr viewerUpdateCondition( new WConditionOneShot() );
        m_viewerUpdateNotifiers.push_back( viewerUpdateCondition );
        // finally remove view (in the GE thread)
        m_removeViewers.push_back( m_viewers[name] );
        m_viewersUpdate = true;
#else
        m_viewer->removeView( m_viewers[name]->getView() );
#endif
        lock.unlock();

#ifdef WGEMODE_MULTITHREADED
        // wait until the GE thread processed our request.
        viewerUpdateCondition->wait();
#endif
    }
    else
    {
        lock.unlock();
    }
}

boost::shared_ptr< WGEViewer > WGraphicsEngine::getViewerByName( std::string name )
{
    boost::shared_lock< boost::shared_mutex > lock( m_viewersLock );
    boost::shared_ptr< WGEViewer > out = m_viewers.count( name ) > 0 ?
        m_viewers[name] :
        boost::shared_ptr< WGEViewer >();
    lock.unlock();
    return out;
}

boost::shared_ptr< WGEViewer > WGraphicsEngine::getViewer()
{
    boost::shared_lock< boost::shared_mutex > lock( m_viewersLock );
    boost::shared_ptr< WGEViewer > result = m_viewers[ "Main View" ];
    lock.unlock();
    return result;
}

bool WGraphicsEngine::isRunning()
{
    if( !m_instance )
    {
        return false;
    }

    return m_instance->m_running;
}

bool WGraphicsEngine::waitForStartupComplete()
{
    if( !m_instance )
    {
        WLogger::getLogger()->addLogMessage( "Not Graphics Engine running", "GE", LL_INFO );
        return false;
    }

    WLogger::getLogger()->addLogMessage( "Blocking for graphics engine startup", "GE", LL_INFO );
    // this ensures that the startup is completed if returning.
    m_instance->m_startThreadingCondition.wait();

    WLogger::getLogger()->addLogMessage( "Done blocking for graphics engine startup, maybe running now", "GE", LL_INFO );
    // did something went wrong? Ensure by checking if really running.
    return isRunning();
}

void WGraphicsEngine::applyViewerListUpdates()
{
    // any new or removed views?
    if( m_viewersUpdate )
    {
        boost::unique_lock< boost::shared_mutex > lock( m_viewersLock );

        // add all views
        for( std::vector< WGEViewer::SPtr >::iterator it = m_addViewers.begin(); it != m_addViewers.end(); ++it )
        {
            m_viewer->addView( ( *it )->getView() );
        }
        m_addViewers.clear();
        for( std::vector< WGEViewer::SPtr >::iterator it = m_removeViewers.begin(); it != m_removeViewers.end(); ++it )
        {
            m_viewer->removeView( ( *it )->getView() );
        }
        m_removeViewers.clear();
        m_viewersUpdate = false;

        // notify all of them
        for( std::vector< WCondition::SPtr >::iterator it = m_viewerUpdateNotifiers.begin(); it != m_viewerUpdateNotifiers.end(); ++it )
        {
            ( *it )->notify();
        }
        m_viewerUpdateNotifiers.clear();
        lock.unlock();
    }
}

void WGraphicsEngine::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting Graphics Engine", "GE", LL_INFO );

    // initialize all available postprocessors
    WGEPostprocessor::initPostprocessors();

#ifdef WGEMODE_MULTITHREADED
    // NOTE: this is needed here since the viewer might start without any widgets being initialized properly.
    m_startThreadingCondition.wait();
    m_running = true;
    m_viewer->startThreading();

    // do this before calling realize to ensure the initial views where added
    applyViewerListUpdates();

    if( !m_viewer->isRealized() )
    {
        m_viewer->realize();
    }
    while( !m_viewer->done() )
    {
        // added or removed views?
        applyViewerListUpdates();
        m_viewer->frame();
    }
    m_viewer->stopThreading();
    m_running = false;
#else
    //m_startThreadingCondition.wait();
    m_running = true; // we have to make sure, that we are "running"
#endif
}

void WGraphicsEngine::notifyStop()
{
    // when stopping the system while the GE is still waiting.
    m_startThreadingCondition.notify();
    WLogger::getLogger()->addLogMessage( "Stopping Graphics Engine", "GE", LL_INFO );
#ifdef WGEMODE_MULTITHREADED
    m_viewer->setDone( true );
#endif
}

void WGraphicsEngine::finalizeStartup()
{
    m_startThreadingCondition.notify();
}

void WGraphicsEngine::waitForFinalize()
{
    m_startThreadingCondition.wait();
}

void WGraphicsEngine::requestShaderReload()
{
    m_reloadShadersSignal();
}

boost::signals2::connection WGraphicsEngine::subscribeSignal( GE_SIGNAL signal, t_GEGenericSignalHandlerType notifier )
{
    switch( signal )
    {
        case GE_RELOADSHADERS:
            return m_reloadShadersSignal.connect( notifier );
        case GE_STARTUPCOMPLETE:
            return m_startThreadingCondition.subscribeSignal( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WGESignalSubscriptionFailed( s.str() );
            break;
    }
}
