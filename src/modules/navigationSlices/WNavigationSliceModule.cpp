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

#include <iostream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>

#include "WNavigationSliceModule.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModuleInputConnector.h"

#include "../../graphicsEngine/WShader.h"

WNavigationSliceModule::WNavigationSliceModule():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.

    // initialize members
    m_shader = boost::shared_ptr< WShader > ( new WShader( "slice" ) );
}

WNavigationSliceModule::~WNavigationSliceModule()
{
    // cleanup
    removeConnectors();
}

const std::string WNavigationSliceModule::getName() const
{
    return "Navigation Slice Module";
}

const std::string WNavigationSliceModule::getDescription() const
{
    return "This module shows 3 orthogonal navigation slices.";
}

void WNavigationSliceModule::connectors()
{
    // initialize connectors
    // XXX to add a new connector and to offer it, these simple steps need to be done
    // initialize it first
    m_Input= boost::shared_ptr<WModuleInputConnector>(
            new WModuleInputConnector( shared_from_this(), "in1", "Dataset to show on the slices." )
    );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_Input );

    // call WModules initialization
    WModule::connectors();
}

void WNavigationSliceModule::notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                               boost::shared_ptr<WModuleConnector> output )
{
    WModule::notifyDataChange( input, output );

    // in this case input==m_Input
}

void WNavigationSliceModule::threadMain()
{
    createSlices();

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

void WNavigationSliceModule::createSlices()
{
    osg::Geode* m_sliceNode = new osg::Geode();

    osg::Geometry* sliceGeometry = new osg::Geometry();
    m_sliceNode->addDrawable( sliceGeometry );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, 50, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, 50, 100 ) );
    sliceVertices->push_back( osg::Vec3( 100, 50, 100 ) );
    sliceVertices->push_back( osg::Vec3( 100, 50, 0 ) );

    sliceVertices->push_back( osg::Vec3( 50, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( 50, 0, 100 ) );
    sliceVertices->push_back( osg::Vec3( 50, 100, 100 ) );
    sliceVertices->push_back( osg::Vec3( 50, 100, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, 50 ) );
    sliceVertices->push_back( osg::Vec3( 0, 100, 50 ) );
    sliceVertices->push_back( osg::Vec3( 100, 100, 50 ) );
    sliceVertices->push_back( osg::Vec3( 100, 0, 50 ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, 0.5, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, 0.5, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.5, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.5, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.5, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.5, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( 0.5, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( 0.5, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, 0.5 ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, 0.5 ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, 0.5 ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, 0.5 ) );

    sliceGeometry->setTexCoordArray( 0, texCoords );

    osg::DrawElementsUInt* slice0 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice0->push_back( 3 );
    slice0->push_back( 2 );
    slice0->push_back( 1 );
    slice0->push_back( 0 );

    osg::DrawElementsUInt* slice1 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice1->push_back( 7 );
    slice1->push_back( 6 );
    slice1->push_back( 5 );
    slice1->push_back( 4 );

    osg::DrawElementsUInt* slice2 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice2->push_back( 11 );
    slice2->push_back( 10 );
    slice2->push_back( 9 );
    slice2->push_back( 8 );

    sliceGeometry->addPrimitiveSet( slice0 );
    sliceGeometry->addPrimitiveSet( slice1 );
    sliceGeometry->addPrimitiveSet( slice2 );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_sliceNode );

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();

    sliceState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );
}
