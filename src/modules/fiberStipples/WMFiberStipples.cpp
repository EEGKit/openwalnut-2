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

#include <cstdlib>
#include <string>

#include <osg/Geometry>
#include <osg/MatrixTransform>
// #include <osg/Vec3>

#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "WMFiberStipples.h"
#include "WMFiberStipples.xpm"

#include "core/common/math/WMath.h"
#include "core/common/WPropertyHelper.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "core/graphicsEngine/callbacks/WGEPropertyUniformCallback.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WSelectionManager.h"


// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberStipples )

WMFiberStipples::WMFiberStipples()
    : WModule(),
      m_first( true )
{
}

WMFiberStipples::~WMFiberStipples()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberStipples::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberStipples() );
}

const char** WMFiberStipples::getXPMIcon() const
{
    return WMFiberStipples_xpm;
}

const std::string WMFiberStipples::getName() const
{
    return "Fiber Stipples";
}

const std::string WMFiberStipples::getDescription() const
{
    return "Slice based probabilistic tract display using Fiber Stipples. (see http://dx.doi.org/10.1109/BioVis.2011.6094044)";
}

void WMFiberStipples::connectors()
{
    m_vectorIC = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "vectors", "Principal diffusion direction." );
    m_probIC = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "probTract", "Probabilistic tract." );

    // call WModule's initialization
    WModule::connectors();
}

void WMFiberStipples::properties()
{
//    m_sliceGroup     = m_properties->addPropertyGroup( "Slices",  "Slice based probabilistic tractogram display." );
//
    m_Pos = m_properties->addProperty( "Slice position", "Slice position.", 0.0 );
    // we don't know anything about data dimensions yet => make slide unusable
    m_Pos->setMax( 0 );
    m_Pos->setMin( 0 );

    m_color = m_properties->addProperty( "Color", "Color for the fiber stipples", WColor( 1.0, 0.0, 0.0, 1.0 ) );
    m_threshold = m_properties->addProperty( "Threshold", "Connectivity scores below this threshold will be discarded.", 0.01 );
    m_threshold->setMin( 0.0 );
    m_threshold->setMax( 1.0 );

//    WPropDouble spacing = m_vectorGroup->addProperty( "Spacing", "Spacing of the sprites", 1.0, m_sliceChanged );
//    spacing->setMin( 0.25 );
//    spacing->setMax( 5.0 );
//    WPropDouble glyphSize = m_vectorGroup->addProperty( "Glyph size", "Size of the quads transformed to the glyphs", 1.0 );
//    glyphSize->setMin( 0.25 );
//    glyphSize->setMax( 5.0 );
//    WPropDouble glyphSpacing = m_vectorGroup->addProperty( "Glyph Spacing", "Spacing ", 0.4, m_sliceChanged );
//    glyphSpacing->setMin( 0.0 );
//    glyphSpacing->setMax( 5.0 );
//    WPropDouble glyphThickness = m_vectorGroup->addProperty( "Glyph Thickness", "Line thickness of the glyphs", 1.0 );
//    glyphThickness->setMin( 0.01 );
//    glyphThickness->setMax( 2.0 );

    // call WModule's initialization
    WModule::properties();
}

namespace
{
    osg::ref_ptr< osg::Geode > genScatteredDegeneratedQuads( size_t numSamples, osg::Vec3 const& base, osg::Vec3 const& a, osg::Vec3 const& b )
    {
        // the stuff needed by the OSG to create a geometry instance
        osg::ref_ptr< osg::Vec3Array > vertices = new osg::Vec3Array( numSamples * 4 );
        osg::ref_ptr< osg::Vec3Array > texcoords0 = new osg::Vec3Array( numSamples * 4 );
        osg::ref_ptr< osg::Vec3Array > texcoords1 = new osg::Vec3Array( numSamples * 4 );
        osg::ref_ptr< osg::Vec3Array > normals = new osg::Vec3Array;
        osg::ref_ptr< osg::Vec4Array > colors = new osg::Vec4Array;

        osg::Vec3 aCrossB = a ^ b;
        aCrossB.normalize();
        osg::Vec3 aNorm = a;
        aNorm.normalize();
        osg::Vec3 bNorm = b;
        bNorm.normalize();

        std::srand( time( NULL ) );
        double lambda0, lambda1;
        const double rndMax = RAND_MAX;

        for( size_t i = 0; i < numSamples; ++i )
        {
            // The degenerated QUAD should have all points in its center
            lambda0 = rand() / rndMax;
            lambda1 = rand() / rndMax;
            osg::Vec3 quadCenter = base + a * lambda0 + b * lambda1;
            for( int j = 0; j < 4; ++j )
            {
                vertices->push_back( quadCenter );
            }

            texcoords0->push_back( ( -aNorm + -bNorm ) );
            texcoords0->push_back( (  aNorm + -bNorm ) );
            texcoords0->push_back( (  aNorm +  bNorm ) );
            texcoords0->push_back( ( -aNorm +  bNorm ) );

            texcoords1->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
            texcoords1->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
            texcoords1->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
            texcoords1->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
        }

        normals->push_back( aCrossB );
        colors->push_back( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );

        // put it all together
        osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
        geometry->setVertexArray( vertices );
        geometry->setTexCoordArray( 0, texcoords0 );
        geometry->setTexCoordArray( 1, texcoords1 );
        geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
        geometry->setNormalArray( normals );
        geometry->setColorArray( colors );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

        osg::ref_ptr< osg::Geode > geode = new osg::Geode();
        geode->addDrawable( geometry );
        return geode;
    }
}

void WMFiberStipples::initOSG( boost::shared_ptr< WDataSetScalar > probTract )
{
    debugLog() << "Init OSG";

    m_output->clear();

    // no colormaps -> no slices
    bool empty = !WGEColormapping::instance()->size();
    if( empty )
    {
        // hide the slider properties.
        m_Pos->setHidden();
        return;
    }

    // grab the current bounding box for computing the size of the slice
    WBoundingBox bb = probTract->getGrid()->getBoundingBox();
    WVector3d minV = bb.getMin();
    WVector3d maxV = bb.getMax();
    WVector3d sizes = ( maxV - minV );
    WVector3d midBB = minV + ( sizes * 0.5 );

    // update the properties
    m_Pos->setMin( minV[1] );
    m_Pos->setMax( maxV[1] );
    m_Pos->setHidden( false );

    // if this is done the first time, set the slices to the center of the dataset
    if( m_first )
    {
        m_first = false;
        m_Pos->set( midBB[1] );
    }

    // create a new geode containing the slices
    osg::ref_ptr< osg::Node > slice = genScatteredDegeneratedQuads( 100000, minV, osg::Vec3( sizes[0], 0.0, 0.0 ),
                                                                    osg::Vec3( 0.0, 0.0, sizes[2] ) );
    slice->setName( "Coronal Slice" );

    osg::ref_ptr< osg::Uniform > u_aVec = new osg::Uniform( "u_aVec", osg::Vec3( sizes[0], 0.0, 0.0 ) );
    osg::ref_ptr< osg::Uniform > u_bVec = new osg::Uniform( "u_bVec", osg::Vec3( 0.0, 0.0, sizes[2] ) );
    osg::ref_ptr< osg::Uniform > u_WorldTransform = new osg::Uniform( "u_WorldTransform", osg::Matrix::identity() );
    boost::shared_ptr< const WGridRegular3D > grid = boost::shared_dynamic_cast< const WGridRegular3D >( probTract->getGrid() );
    if( !grid )
    {
        errorLog() << "This module can only process probabilistic Tracts with regular 3D grids, Hence you may see garbage from now on.";
    }
    boost::array< double, 3 > offsets = getOffsets( grid );
    osg::ref_ptr< osg::Uniform > u_pixelSizeX = new osg::Uniform( "u_pixelSizeX", static_cast< float >( offsets[0] ) );
    osg::ref_ptr< osg::Uniform > u_pixelSizeY = new osg::Uniform( "u_pixelSizeY", static_cast< float >( offsets[1] ) );
    osg::ref_ptr< osg::Uniform > u_pixelSizeZ = new osg::Uniform( "u_pixelSizeZ", static_cast< float >( offsets[2] ) );
    osg::ref_ptr< osg::Uniform > u_color = new WGEPropertyUniform< WPropColor >( "u_color", m_color );
    osg::ref_ptr< osg::Uniform > u_threshold = new WGEPropertyUniform< WPropDouble >( "u_threshold", m_threshold );
    osg::ref_ptr< osg::Uniform > u_maxConnectivityScore = new osg::Uniform( "u_maxConnectivityScore", static_cast< float >( probTract->getMax() ) );

    osg::StateSet *states = slice->getOrCreateStateSet();
    states->addUniform( u_aVec );
    states->addUniform( u_bVec );
    states->addUniform( u_WorldTransform );
    states->addUniform( u_pixelSizeX );
    states->addUniform( u_pixelSizeY );
    states->addUniform( u_pixelSizeZ );
    states->addUniform( u_color );
    states->addUniform( u_threshold );
    states->addUniform( u_maxConnectivityScore );
    slice->setCullingActive( false );

    // each slice is child of an transformation node
    osg::ref_ptr< osg::MatrixTransform > mT = new osg::MatrixTransform();
    mT->addChild( slice );

    // Control transformation node by properties. We use an additional uniform here to provide the shader
    // the transformation matrix used to translate the slice.
    mT->addUpdateCallback( new WGELinearTranslationCallback< WPropDouble >( osg::Vec3( 0.0, 1.0, 0.0 ), m_Pos, u_WorldTransform ) );

    m_output->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    m_output->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    m_output->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_output->insert( mT );
    m_output->dirtyBound();
}

void WMFiberStipples::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_probIC->getDataChangedCondition() );
    m_moduleState.add( m_vectorIC->getDataChangedCondition() );

    ready();

    // graphics setup
    m_output = osg::ref_ptr< WGEManagedGroupNode >( new WGEManagedGroupNode( m_active ) );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_output );
    osg::ref_ptr< WGEShader > shader = new WGEShader( "WFiberStipples", m_localPath );
    shader->apply( m_output ); // this automatically applies the shader

    // main loop
    while( !m_shutdownFlag() )
    {
        infoLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // save data behind connectors since it might change during processing
        boost::shared_ptr< WDataSetVector > vectors = m_vectorIC->getData();
        boost::shared_ptr< WDataSetScalar > probTract = m_probIC->getData();
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( probTract->getGrid() );

        if( !( vectors && probTract ) || !grid ) // if data valid
        {
            continue;
        }

        initOSG( probTract );

        wge::bindTexture( m_output, vectors->getTexture(), 0, "u_vectors" );
        wge::bindTexture( m_output, probTract->getTexture(), 1, "u_probTract" );

        // TODO(math): unbind textures, so we have a clean OSG root node for this module again
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_output );
}