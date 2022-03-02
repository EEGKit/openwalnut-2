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

#include <string>

#include <osgDB/ReadFile>

#include "core/graphicsEngine/WGEManagedGroupNode.h"

#include "WMVRCamera.h"
#include "WMVRCamera.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVRCamera )

WMVRCamera::WMVRCamera() : WModule()
{
}

WMVRCamera::~WMVRCamera()
{
}

std::shared_ptr<WModule> WMVRCamera::factory() const
{
    return std::shared_ptr<WModule>( new WMVRCamera() );
}

const char **WMVRCamera::getXPMIcon() const
{
    return open_vr_camera_xpm;
}

const std::string WMVRCamera::getName() const
{
    return "OpenVRCamera";
}

const std::string WMVRCamera::getDescription() const
{
    return "This module will start an OpenVR session"
           " right now it only adds an extra camera that renders to a texture";
}

void WMVRCamera::connectors()
{
    WModule::connectors();
}

void WMVRCamera::properties()
{
    m_propCondition = std::shared_ptr<WCondition>( new WCondition() );

    m_vrOn = m_properties->addProperty( "Submit Frames", "Toggle submitting frames to OpenVR", false );
    m_VR_fpsTrigger = m_properties->addProperty( "Log Fps", "Now", WPVBaseTypes::PV_TRIGGER_READY );
    m_VR_logCameraViewMatrix = m_properties->addProperty( "Log Camera View Matrix", "Now", WPVBaseTypes::PV_TRIGGER_READY );
    m_VR_resetHMDPosition = m_properties->addProperty( "Reset position", "Reset", WPVBaseTypes::PV_TRIGGER_READY );

    WModule::properties();
}

bool WMVRCamera::setupVRInterface()
{
    // Exit if we do not have an HMD present
    if( !vr::VR_IsHmdPresent() )
    {
        errorLog() << "No valid HMD present!" << std::endl;
        return false;
    }

    // Loading the OpenVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;
    m_vrSystem = vr::VR_Init( &eError, vr::VRApplication_Scene );

    if( eError != vr::VRInitError_None )
    {
        m_vrSystem = nullptr;
        errorLog()
            << "Unable to initialize the OpenVR library." << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return false;
    }

    if( !vr::VRCompositor() )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog() << "Compositor initialization failed" << std::endl;
        return false;
    }

    m_vrRenderModels =( vr::IVRRenderModels * )vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &eError );

    if( m_vrRenderModels == nullptr )
    {
        m_vrSystem = nullptr;
        vr::VR_Shutdown();
        errorLog()
            << "Unable to get render model interface!" << std::endl
            << "Reason: " << vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;
        return false;
    }

    vr::VRCompositor()->SetTrackingSpace( vr::TrackingUniverseSeated );
    m_vrSystem->GetRecommendedRenderTargetSize( &m_vrRenderWidth, &m_vrRenderHeight );

    m_grabber = vr::k_unTrackedDeviceIndexInvalid;

    // Check for connected controllers
    updateDeviceIDs();

    std::string driverName = getDeviceProperty( vr::Prop_TrackingSystemName_String );
    std::string deviceSerialNumber = getDeviceProperty( vr::Prop_SerialNumber_String );
    debugLog() << "HMD driver name: " << driverName;
    debugLog() << "HMD device serial number: " << deviceSerialNumber;

    return true;
}

void WMVRCamera::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // HMD Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    if( setupVRInterface() )
    {
        debugLog() << "OpenVR initialized successfully";
        m_vrIsInitialized = true;
    }
    else
    {
        errorLog() << "OpenVR failed to initialize";
        m_vrRenderWidth = 1736;
        m_vrRenderHeight = 1928;
        m_vrIsInitialized = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Render to Texture Setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    osg::View* mainView = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    osg::GraphicsContext* gc = mainView->getCamera()->getGraphicsContext();
    osg::ref_ptr< WGEGroupNode > scene = WKernel::getRunningKernel()->getGraphicsEngine()->getScene();

    osg::Matrix projLeft = convertHmdMatrixToOSG( m_vrSystem->GetProjectionMatrix( vr::Eye_Left, 1.0, 1000.0 ) );
    osg::Matrix projRight = convertHmdMatrixToOSG( m_vrSystem->GetProjectionMatrix( vr::Eye_Right, 1.0, 1000.0 ) );

    m_leftEyeCamera = new WRTTCamera( m_vrRenderWidth, m_vrRenderHeight, gc, projLeft, scene, m_localPath );
    m_rightEyeCamera = new WRTTCamera( m_vrRenderWidth, m_vrRenderHeight, gc, projRight, scene, m_localPath );

    mainView->addSlave( m_leftEyeCamera, false );
    mainView->addSlave( m_rightEyeCamera, false );

    // updatecallback for submitting frames
    gc->setSwapCallback( new WSwapUpdateCallback( this ) );

    // Now, we can mark the module ready.
    ready();
    debugLog() << "Starting...";

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        debugLog() << "Waiting...";
        m_moduleState.wait();
    }

    // remove update callback
    gc->setSwapCallback( nullptr );

    // Shut down OpenVR
    m_vrSystem = nullptr;
    vr::VR_Shutdown();

    debugLog() << "Shutting down openVRCamera";
}

void WMVRCamera::activate()
{
    if( m_active->get() )
    {
        debugLog() << "Activate.";
    }
    else
    {
        debugLog() << "Deactivate.";
    }

    WModule::activate();
}

std::string WMVRCamera::getDeviceProperty( vr::TrackedDeviceProperty prop )
{
    uint32_t bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, NULL, 0 );
    if( bufferLen == 0 )
    {
        return "";
    }

    char *buffer = new char[bufferLen];
    bufferLen = m_vrSystem->GetStringTrackedDeviceProperty( vr::k_unTrackedDeviceIndex_Hmd, prop, buffer, bufferLen );
    std::string result = buffer;
    delete[] buffer;
    return result;
}

void WMVRCamera::handleVREvent( vr::VREvent_t vrEvent )
{
    vr::ETrackedDeviceClass deviceClass = m_vrSystem->GetTrackedDeviceClass( vrEvent.trackedDeviceIndex );
    switch( deviceClass )
    {
    case vr::TrackedDeviceClass_Controller:
        handleControllerEvent( vrEvent );
        break;
    default:
        break;
    }
}

void WMVRCamera::handleControllerEvent( vr::VREvent_t vrEvent )
{
    switch( vrEvent.eventType )
    {
    case vr::VREvent_ButtonPress:
        // check for any changes regarding controller device IDs on any button press
        updateDeviceIDs();

        debugLog() << "Pressed:" << vrEvent.data.controller.button;
        if(
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_Grip ||
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_SteamVR_Trigger )
        {
            m_grabber = vrEvent.trackedDeviceIndex;
        }
        break;
    case vr::VREvent_ButtonUnpress:
        debugLog() << "Unpressed:" << vrEvent.data.controller.button;
        if(
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_Grip ||
            vrEvent.data.controller.button == vr::EVRButtonId::k_EButton_SteamVR_Trigger )
        {
            m_grabber = vr::k_unTrackedDeviceIndexInvalid;
        }
        break;
    default:
        break;
    }
}

osg::ref_ptr< osg::Node > WMVRCamera::addControllerGeometry()
{
    // Add controller geometry to the scene
    osg::ref_ptr< osg::Node > controllerNode = osgDB::readNodeFile( ( m_localPath / "VIVEController.obj" ).string() );

    osg::ref_ptr<WGEManagedGroupNode> m_rootnode = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );
    m_rootnode->insert( controllerNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootnode );

    return controllerNode;
}

void WMVRCamera::updateDeviceIDs()
{
    for( uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++ )
    {
        vr::ETrackedControllerRole cRole = m_vrSystem->GetControllerRoleForTrackedDeviceIndex( i );

        if( cRole == vr::TrackedControllerRole_LeftHand ) m_controllerLeft_deviceID = i;
        if( cRole == vr::TrackedControllerRole_RightHand ) m_controllerRight_deviceID = i;
        // more devices IDs can be tracked here (such as the sonsors)
    }
}

void WMVRCamera::updateControllerPoses()
{
    vr::TrackedDevicePose_t trackedDevicePose;
    vr::VRControllerState_t controllerState;

    // Left Hand
    if( m_vrSystem->GetControllerRoleForTrackedDeviceIndex( m_controllerLeft_deviceID ) == vr::TrackedControllerRole_LeftHand )
    {
        // Load geometry once controller is being tracked
        if( m_controllerLeft_isLoaded == false )
        {
            m_controllerLeft_node = addControllerGeometry();
            m_controllerLeft_isLoaded = true;
        }

        m_vrSystem->GetControllerStateWithPose( vr::TrackingUniverseStanding,
        m_controllerLeft_deviceID, &controllerState, sizeof( controllerState ),
        &trackedDevicePose );

        m_controllerLeft_transform = convertHmdMatrixToOSG( trackedDevicePose.mDeviceToAbsoluteTracking );
    }
    // Right Hand
    if( m_vrSystem->GetControllerRoleForTrackedDeviceIndex( m_controllerRight_deviceID ) == vr::TrackedControllerRole_RightHand )
    {
        // Load geometry once controller is being tracked
        if( m_controllerRight_isLoaded == false )
        {
            m_controllerRight_node = addControllerGeometry();
            m_controllerRight_isLoaded = true;
        }

        m_vrSystem->GetControllerStateWithPose( vr::TrackingUniverseStanding,
        m_controllerRight_deviceID, &controllerState, sizeof( controllerState ),
        &trackedDevicePose );

        m_controllerRight_transform = convertHmdMatrixToOSG( trackedDevicePose.mDeviceToAbsoluteTracking );
    }
}

void WMVRCamera::updateHMDPose()
{
    updateDeviceIDs();
    updateControllerPoses();

    // get all OpenVR tracking information
    for( uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i )
    {
        m_poses[i].bPoseIsValid = false;
    }
    vr::VRCompositor()->WaitGetPoses( m_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

    // handle controller events
    vr::VREvent_t vrEvent;
    while( m_vrSystem->PollNextEvent( &vrEvent, sizeof( vr::VREvent_t ) ) )
        handleVREvent( vrEvent );

    vr::TrackedDevicePose_t trackedDevicePose = m_poses[vr::k_unTrackedDeviceIndex_Hmd];

    if( !trackedDevicePose.bPoseIsValid )
    {
        return;
    }

    // pose calculation
    osg::Matrix poseTransform = convertHmdMatrixToOSG( trackedDevicePose.mDeviceToAbsoluteTracking );

    osg::Vec3 position = poseTransform.getTrans() * 100.0;
    m_HMD_rotation = poseTransform.getRotate().inverse();

    // switch y and z axis as openvr has different coordinate system.
    double help = position.y();
    position.y() = -position.z();
    position.z() = help;

    m_HMD_position = position;
}

void WMVRCamera::ResetHMDPosition()
{
    vr::VRChaperone()->ResetZeroPose( vr::TrackingUniverseSeated );
}

osg::Matrix WMVRCamera::convertHmdMatrixToOSG( const vr::HmdMatrix34_t &mat34 )
{
    osg::Matrix matrix(
        mat34.m[0][0], mat34.m[1][0], mat34.m[2][0], 0.0,
        mat34.m[0][1], mat34.m[1][1], mat34.m[2][1], 0.0,
        mat34.m[0][2], mat34.m[1][2], mat34.m[2][2], 0.0,
        mat34.m[0][3], mat34.m[1][3], mat34.m[2][3], 1.0f );
    return matrix;
}

osg::Matrix WMVRCamera::convertHmdMatrixToOSG( const vr::HmdMatrix44_t &mat44 )
{
    osg::Matrix matrix(
        mat44.m[0][0], mat44.m[1][0], mat44.m[2][0], mat44.m[3][0],
        mat44.m[0][1], mat44.m[1][1], mat44.m[2][1], mat44.m[3][1],
        mat44.m[0][2], mat44.m[1][2], mat44.m[2][2], mat44.m[3][2],
        mat44.m[0][3], mat44.m[1][3], mat44.m[2][3], mat44.m[3][3] );
    return matrix;
}
