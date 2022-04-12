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

#include <memory>
#include <string>
#include <vector>

#include "WMTransferFunction2D.h"
#include "WMTransferFunction2D.xpm"
#include "core/common/WTransferFunction2D.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGrid.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WValueSet.h"
#include "core/dataHandler/WValueSetBase.h"
#include "core/dataHandler/datastructures/WValueSetHistogram.h"
#include "core/kernel/WKernel.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMTransferFunction2D )

WMTransferFunction2D::WMTransferFunction2D():
    WModule()
{
}

WMTransferFunction2D::~WMTransferFunction2D()
{
    // Cleanup!
}

std::shared_ptr< WModule > WMTransferFunction2D::factory() const
{
    return std::shared_ptr< WModule >( new WMTransferFunction2D() );
}

const char** WMTransferFunction2D::getXPMIcon() const
{
    return WMTransferFunction2D_xpm;
}

const std::string WMTransferFunction2D::getName() const
{
    return "Transfer Function 2D";
}

const std::string WMTransferFunction2D::getDescription() const
{
    return "A module to modify a transfer function.";
}

void WMTransferFunction2D::connectors()
{
    // the data set we use for displaying the histogram
    m_inputDataSet0 = WModuleInputData < WDataSetSingle >::createAndAdd( shared_from_this(),
            "histogram input data set 0", "The data set used to display a histogram." );

    // the data set we use for displaying the histogram
    m_inputDataSet1 = WModuleInputData < WDataSetSingle >::createAndAdd( shared_from_this(),
                                                                         "histogram input data set 1", "The data set used to display a histogram." );

    // an output connector for the transfer function created
    m_output = WModuleOutputData < WDataSetSingle >::createAndAdd( shared_from_this(),
            "TransferFunction2D", "The selected transfer function" );

    WModule::connectors();
}

void WMTransferFunction2D::properties()
{
    m_propCondition = std::shared_ptr< WCondition >( new WCondition() );

    WTransferFunction2D tf;
    unsigned char * arr = new unsigned char[ 100 * 100 * 4 ];
    for( int x = 0; x < 100; x++ )
            {
                for( int y = 0; y < 100; y++ )
                {
                   arr[ 4 * 100 * x + 4 * y + 0 ] = static_cast< unsigned char >( 0. );
                   arr[ 4 * 100 * x + 4 * y + 1 ] = static_cast< unsigned char >( 0. );
                   arr[ 4 * 100 * x + 4 * y + 2 ] = static_cast< unsigned char >( 0. );
                   arr[ 4 * 100 * x + 4 * y + 3 ] = static_cast< unsigned char >( 0. );
                }
            }
    tf.setTexture( arr, 100, 100 );
    m_transferFunction = m_properties->addProperty( "2D Transfer Function",
                                                    "The 2D transfer function editor. "
                                                    "The Origin (0, 0) is in the top left corner. "
                                                    "The first connected data set is shown on the y-axis from top to bottom. "
                                                    "The second connected data set is shown on the x-axis from left to right. "
                                                    , tf, m_propCondition, false );

    m_opacityScale = m_properties->addProperty( "Opacity Scaling",
                                                "Factor used to scale opacity for easier interaction",
                                                1.0,
                                                m_propCondition );
    m_opacityScale->setMin( 0 );
    m_opacityScale->setMax( 1 );

    m_resolution = m_properties->addProperty( "Number of Samples",
            "Number of samples in the transfer function. "
            "Some modules connected to the output may have additional restrictions. Volume rendering, e.g., requires a power of two "
            "samples at the moment."
            , 128, m_propCondition );
    m_resolution->setMin( 4 );
    m_resolution->setMax( 1024 );
    WModule::properties();
}

void WMTransferFunction2D::requirements()
{
    //m_requirements.push_back( new WGERequirement() );
}

void WMTransferFunction2D::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_inputDataSet0->getDataChangedCondition() );
    m_moduleState.add( m_inputDataSet1->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
          break;

        bool tfChanged = m_transferFunction->changed();
        WTransferFunction2D tf = m_transferFunction->get( true );
        if( m_inputDataSet0->updated() || m_inputDataSet1->updated() )
        {
            std::shared_ptr< WDataSetSingle > dataSet0 = m_inputDataSet0->getData();
            std::shared_ptr< WDataSetSingle > dataSet1 = m_inputDataSet1->getData();

            bool dataValid = ( dataSet0 != NULL && dataSet1 != NULL );
            if( !dataValid )
            {
            }
            else
            {
                std::shared_ptr< WValueSetBase > values0 = dataSet0->getValueSet();
                std::shared_ptr< WValueSetBase > values1 = dataSet1->getValueSet();

                wlog::debug( "WMTransFunc2D" ) << values0->getMinimumValue();
                wlog::debug( "WMTransFunc2D" ) << values0->getMaximumValue();
                wlog::debug( "WMTransFunc2D" ) << values1->getMinimumValue();
                wlog::debug( "WMTransFunc2D" ) << values1->getMaximumValue();

                // At the moment we equal sized data sets
                // Bucket size is equal to our texture size defined in WTransferFunction2DWidget
                auto histogram = std::make_shared< WHistogram2D >( values0->getMinimumValue(),
                                                  values0->getMaximumValue(),
                                                  values1->getMinimumValue(),
                                                  values1->getMaximumValue(),
                                                  300,
                                                  300 );

                for( size_t i = 0; i < values0->size(); ++i )
                {
                    histogram->insert( values0->getScalarDouble( i ), values1->getScalarDouble( i ) );
                }

                // We need a copy of the data here, because of the way how the data is handled in WTransferFunction2D
                //WHistogram2D vhistogram( histogram );
                tf.setHistogram( histogram );
            }
            // either way, we changed the data and want to update the TF
            m_transferFunction->set( tf );
        }
        if( tfChanged || m_opacityScale->changed() )
        {
            wlog::debug( "WMTransferFunction2D" ) << "tf changed";
            // debugLog() << "resampling transfer function";
            //unsigned int resolution = m_resolution->get( true );
            //TODO(Kai): resolution for x & y
            unsigned int resolution = 128;
            // debugLog() << "new resolution: " << resolution;
            std::shared_ptr< std::vector<unsigned char> > data( new std::vector<unsigned char>( resolution * resolution * 4 ) );
            tf.setOpacityScale( m_opacityScale->get( true ) );
            tf.sample2DTransferFunction( &( *data )[0], resolution, resolution );

            // Create data set which holds the TF
            std::shared_ptr< WValueSetBase > newValueSet( new WValueSet<unsigned char>( 1, 4, data, W_DT_UNSIGNED_CHAR ) );

            WGridTransformOrtho transform;
            std::shared_ptr< WGridRegular3D > newGrid( new WGridRegular3D( resolution, resolution, 1, transform ) );
            std::shared_ptr< WDataSetSingle > newData( new WDataSetSingle( newValueSet, newGrid ) );
            // publish the TF
            m_output->updateData( newData );
        }
    }
}
