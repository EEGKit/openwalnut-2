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

#include <osg/Geode>

#include "core/common/math/WMath.h"
#include "core/common/WLimits.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"

#include "core/dataHandler/WDataSetPoints.h"
#include "core/dataHandler/WDataSetPointsAndSizes.h"

#include "WMPointConnector.h"
#include "WMClickHandler.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMPointConnector )

WMPointConnector::WMPointConnector():
    WModule()
{
}

WMPointConnector::~WMPointConnector()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMPointConnector::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMPointConnector() );
}

const char** WMPointConnector::getXPMIcon() const
{
    return NULL;
}

const std::string WMPointConnector::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Point Connector";
}

const std::string WMPointConnector::getDescription() const
{
    return "Allows user to manually connect points to fibers.";
}

void WMPointConnector::connectors()
{
    // this input contains the triangle data
    m_pointInput = WModuleInputData< WDataSetPoints >::createAndAdd( shared_from_this(), "points", "The data to display" );

    // call WModule's initialization
    WModule::connectors();
}

void WMPointConnector::properties()
{
    // some properties need to trigger an update
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // setup all the properties. See header file for their meaning and purpose.
    m_size = m_properties->addProperty( "Point Size", "The size of the points.", 0.25 );
    m_size->setMin( 0.0001 );
    m_size->setMax( 10.0 );

    m_useCorrectDepth = m_properties->addProperty( "Correct Depth", "If set, the depths of the sprites are calculated correctly. You can disable "
                                                                    "this to get higher framerates at the cost of visual correctness.", true );

    // call WModule's initialization
    WModule::properties();
}

void WMPointConnector::moduleMain()
{
    // let the main loop awake if the data changes.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_pointInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // setup the main graphics-node:
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the post-processing node which actually does the nice stuff to the rendered image
    postNode = new WGEPostprocessingNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );
    postNode->addUpdateCallback( new WGENodeMaskCallback( m_active ) ); // disable the postNode with m_active
    // provide the properties of the post-processor to the user
    m_properties->addProperty( postNode->getProperties() );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( postNode );

    // load the GLSL shader:
    m_shader = new WGEShader( "WMPointConnector", m_localPath );
    // set geometry shader options
    m_shader->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
    m_shader->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS );
    m_shader->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );

    // insert some uniforms and defines
    postNode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_pointSize", m_size ) );
    m_shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_useCorrectDepth, "DEPTHWRITE_DISABLED", "DEPTHWRITE_ENABLED" ) )
    );

    osg::ref_ptr<osgViewer::View> viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getView();
    osg::ref_ptr<WMClickHandler> handler = new WMClickHandler( this );
    viewer->addEventHandler(handler.get());

    vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

    // signal ready state. The module is now ready to be used.
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // Get data and check for invalid data.
        WDataSetPoints::SPtr points = m_pointInput->getData();
        if( !points )
        {
            debugLog() << "Invalid Data. Disabling.";
            postNode->clear();
            continue;
        }

        // convert point arrays to osg vec3 arrays
        vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
        colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

        WDataSetPoints::VertexArray pointVertices = points->getVertices();
        WDataSetPoints::ColorArray pointColors = points->getColors();
        for( size_t pointIdx = 0; pointIdx < points->size(); ++pointIdx )
        {
            osg::Vec3 vert = points->operator[]( pointIdx );
            osg::Vec4 color = points->getColor( pointIdx );

            vertices->push_back( vert );
            colors->push_back( color );
        }

        redraw();
    }

    // it is important to always remove the modules again
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( postNode );
}

void WMPointConnector::redraw()
{
    osg::ref_ptr< osg::Geometry > geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );
    osg::ref_ptr< osg::Geode > geode( new osg::Geode() );

    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setMode( GL_BLEND, osg::StateAttribute::ON );

    // combine to geometry
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, vertices->size() ) );
    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    // add geometry to geode
    geode->addDrawable( geometry );

    // shader and colormapping
    m_shader->apply( geode );

    // add geode to group
    postNode->clear();
    postNode->insert( geode, m_shader );
}

void WMPointConnector::handleClick(osg::Vec3 cameraPosition, osg::Vec3 direction)
{
   // debugLog() << "HANDLE DA CLICK";
    for(osg::MixinVector<osg::Vec3f>::size_type idx = 0; idx < vertices->size(); idx++)
    {
        //debugLog() << "CHECK DA VERTEX " << idx;
        osg::Vec3 vertex = vertices->at( idx );

        float hit = hitVertex( cameraPosition, direction, vertex, m_size->get() );
        //debugLog() << hit;
        if( hit > 0.0 )
        {
            //debugLog() << "HIT IT";
        }
    }
    //debugLog() << "WE ENDED DA CLICK";
}

float WMPointConnector::hitVertex(osg::Vec3 rayStart, osg::Vec3 rayDir, osg::Vec3 vertex, float radius)
{
    osg::Vec3 oc = rayStart - vertex;
    float a = rayDir * rayDir;
    float b = oc * rayDir;
    float c = ( oc * oc ) - radius * radius;

    if( c > 0.0 && b > 0.0 )
    {
        return -1;
    }

    float discriminant = b * b - c;
    if( discriminant < 0 )
    {
        return -1.0;
    }
    return (-b - sqrt( discriminant ) );

    /*osg::Vec3 oc = rayStart - vertex;
    float a = rayDir * rayDir;
    float b = 2.0 * ( oc * rayDir );
    float c = ( oc * oc ) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if( discriminant < 0.0 )
    {
        return -1.0;
    }
    else
    {
        float numerator = -b - sqrt( discriminant );
        if( numerator > 0.0 )
        {
            return numerator / ( 2.0 * a );
        }

        numerator = -b + sqrt( discriminant );
        if( numerator > 0.0 )
        {
            return numerator / ( 2.0 * a );
        }
        else
        {
            return -1.0;
        }
    }*/
}
