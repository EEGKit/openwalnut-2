//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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
#include <cmath>

#include <osg/Geometry>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "WMButterfly.xpm"
#include "WMButterfly.h"

#ifndef M_PI
const double M_PI = 3.14159265358979323846;
#endif

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMButterfly )

WMButterfly::WMButterfly() :
    WModule(), m_propCondition( new WCondition() )
{
    butterfly = new ButterflyFactory();
    verts = new VertexFactory();
}

WMButterfly::~WMButterfly()
{
}

boost::shared_ptr<WModule> WMButterfly::factory() const
{
    return boost::shared_ptr<WModule>(new WMButterfly());
}

const char** WMButterfly::getXPMIcon() const
{
    return WMButterfly_xpm;
}

const std::string WMButterfly::getName() const
{
    return "Butterfly subdivision";
}

const std::string WMButterfly::getDescription() const
{
    return "Should draw values above some threshold.";
}

void WMButterfly::connectors()
{
    m_input = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(),
                                                               "input mesh", "The mesh to display" );
    m_output = boost::shared_ptr<WModuleOutputData<WTriangleMesh> >(
        new WModuleOutputData<WTriangleMesh>( shared_from_this(),
                                              "output mesh",
                                              "The loaded mesh." ) );
    addConnector( m_output );
    WModule::connectors();
}

void WMButterfly::properties()
{
    // ---> Put the code for your properties here. See "src/modules/template/" for an extensively documented example.
    m_iterations = m_properties->addProperty( "Iterations count = ",
                                              "Iteration count that is attempted regarding the maximal triangle count.",
                                              1,
                                              m_propCondition );
    m_iterations->setMin( 0 );
    m_iterations->setMax( 10 );
    m_maxTriangles10n = m_properties->addProperty( "Max. triangles = 10^: ",
                                                   "Maximal triangle count as result, scaled by 10^n.",
                                                   5.0,
                                                   m_propCondition );
    m_maxTriangles10n->setMin( 2 );
    m_maxTriangles10n->setMax( 7.5 );
    m_maxTriangles = m_properties->addProperty( "Total triangles: ",
                                                "voxel count.",
                                                ( int ) pow( 10, m_maxTriangles10n->get() ) );
    m_maxTriangles->setPurpose( PV_PURPOSE_INFORMATION );
    WModule::properties();
}

void WMButterfly::requirements()
{
}

void WMButterfly::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    // graphics setup
    m_rootNode = osg::ref_ptr<WGEManagedGroupNode>( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // main loop
    while( !m_shutdownFlag() )
    {
        int maxTriangles = ( int ) pow( 10, m_maxTriangles10n->get() );
        m_maxTriangles->set( maxTriangles );
        //infoLog() << "Waiting ...";
        m_moduleState.wait();

        butterfly->assignSettings( m_iterations, m_maxTriangles10n );
        mesh = m_input->getData();
        if( mesh )
        {
            boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Subdivide", 2 ) );
            m_progress->addSubProgress( progress );

            m_output->updateData( butterfly->getInterpolatedMesh( mesh ) );
            infoLog()  << "Mesh applied for " << m_iterations->get() << " iterations setting\r\n";
            progress->finish();
        }

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // save data behind connectors since it might change during processing
        boost::shared_ptr< WTriangleMesh > meshData = m_input->getData();

        if( !meshData )
        {
            continue;
        }

        // ---> Insert code doing the real stuff here
    }



    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

boost::shared_ptr< WTriangleMesh > WMButterfly::popUnusedVertices( boost::shared_ptr< WTriangleMesh > inputMesh )
{
    boost::shared_ptr<WTriangleMesh> outputMesh( new WTriangleMesh( 0, 0 ) );

    size_t triCount = 0;
    boost::shared_ptr<WTriangleMesh> tmpMesh( new WTriangleMesh( 0, 0 ) );
    try
    {
        while  (true)
        {
            Vec3 point0 = inputMesh->getVertex( mesh->getTriVertId0( triCount ) );
            Vec3 point1 = inputMesh->getVertex( mesh->getTriVertId1( triCount ) );
            Vec3 point2 = inputMesh->getVertex( mesh->getTriVertId2( triCount ) );
            outputMesh->addTriangle( point0, point1, point2 );
            triCount++;
        }
    } catch( ... )
    {
    }

    return inputMesh;
}
