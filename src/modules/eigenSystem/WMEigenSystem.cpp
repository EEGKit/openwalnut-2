//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include "../../dataHandler/WDataSetDTI.h"
#include "../../dataHandler/WDataSetVector.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "WMEigenSystem.xpm"
#include "WMEigenSystem.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMEigenSystem )

WMEigenSystem::WMEigenSystem():
    WModule()
{
}

WMEigenSystem::~WMEigenSystem()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMEigenSystem::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMEigenSystem() );
}

const char** WMEigenSystem::getXPMIcon() const
{
    return WMEigenSystem_xpm;
}
const std::string WMEigenSystem::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "EigenSystem";
}

const std::string WMEigenSystem::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"math\"";
}

void WMEigenSystem::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.
    m_tensorIC = WModuleInputData< WDataSetDTI >::createAndAdd( shared_from_this(), "tensorInput", "The tensor field" );

    m_evecOutputs.push_back( WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "evec1Output", "The 1. eigenvector field" ) );
    m_evecOutputs.push_back( WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "evec2Output", "The 2. eigenvector field" ) );
    m_evecOutputs.push_back( WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "evec3Output", "The 3. eigenvector field" ) );
    m_evecOutputs.push_back( WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "evalsOutput", "All three eigenvalues" ) );

    m_evalOutputs.push_back( WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "eval1Output", "The 1. eigenvalue field" ) );
    m_evalOutputs.push_back( WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "eval2Output", "The 2. eigenvalue field" ) );
    m_evalOutputs.push_back( WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "eval3Output", "The 3. eigenvalue field" ) );

    WModule::connectors();
}

void WMEigenSystem::properties()
{
    WModule::properties();
}

void WMEigenSystem::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMEigenSystem::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_tensorIC->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() )
    {
        infoLog() << "Waiting.";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetDTI > tensors = m_tensorIC->getData();

        if( !m_eigenPool && tensors )
        {
            // start the eigenvector computation if input is DTI double or float otherwise continue
            // when the computation finishes, we'll be notified by the threadspool's threadsDoneCondition
            resetEigenFunction( tensors );
            if( !m_eigenPool )
            {
                continue;
            }
            resetProgress( tensors->getValueSet()->size(), "Compute eigen system" );
            m_eigenPool->run();
            infoLog() << "Computing eigen systems...";
        }
        else if( m_eigenPool && m_eigenPool->status() == W_THREADS_FINISHED )
        {
            // the computation of the eigenvectors has finished we have a new field of eigen systems
            m_currentProgress->finish();
            WAssert( m_eigenOperationFloat || m_eigenOperationDouble, "Bug: No result is available. Checked double and float!" );

            if( tensors->getValueSet()->getDataType() == W_DT_DOUBLE )
            {
                updateOCs( m_eigenOperationDouble->getResult() );
            }
            else
            {
                updateOCs( m_eigenOperationFloat->getResult() );
            }

            m_eigenPool.reset();
            infoLog() << "Computing eigen systems done.";
        }
    }
}

void WMEigenSystem::updateOCs( boost::shared_ptr< const WDataSetSingle > es )
{
    WAssert( es, "Bug: updating the output with no data makes no sense." );
    boost::shared_ptr< const WValueSet< double > > vs = boost::shared_dynamic_cast< WValueSet< double > >( es->getValueSet() );
    WAssert( vs, "Bug: invalid value-set from WThreadedPerVoxelOperation dataset" );
    boost::shared_ptr< WGrid > grid = es->getGrid();

    typedef boost::shared_ptr< std::vector< double > > DataPointer;
    std::vector< DataPointer > vecdata( 4, DataPointer( new std::vector< double >( vs->size() * 3 ) ) );
    std::vector< DataPointer > valdata( 3, DataPointer( new std::vector< double >( vs->size() ) ) );

    for( size_t i = 0; i < vs->size(); ++i )
    {
        WValue< double > sys = vs->getWValue( i );
        // eigenvalues
        ( *vecdata[0] )[ i * 3 ] = sys[0];
        ( *valdata[0] )[ i ] = sys[0];
        ( *vecdata[0] )[ i * 3 + 1] = sys[4];
        ( *valdata[1] )[ i ] = sys[4];
        ( *vecdata[0] )[ i * 3 + 2] = sys[8];
        ( *valdata[2] )[ i ] = sys[8];
        // first eigenvector
        ( *vecdata[1] )[ i * 3 ] = sys[1];
        ( *vecdata[1] )[ i * 3 + 1] = sys[2];
        ( *vecdata[1] )[ i * 3 + 2] = sys[3];
        // second eigenvector
        ( *vecdata[2] )[ i * 3 ] = sys[5];
        ( *vecdata[2] )[ i * 3 + 1] = sys[6];
        ( *vecdata[2] )[ i * 3 + 2] = sys[7];
        // third eigenvector
        ( *vecdata[3] )[ i * 3 ] = sys[9];
        ( *vecdata[3] )[ i * 3 + 1] = sys[10];
        ( *vecdata[3] )[ i * 3 + 2] = sys[11];
    }
    typedef boost::shared_ptr< WDataSetVector > PDSV;
    typedef boost::shared_ptr< WDataSetScalar > PDSS;
    typedef WValueSet< double > WVSDBL;
    typedef boost::shared_ptr< WVSDBL > PWVSDBL;

    m_evecOutputs[0]->updateData( PDSV( new WDataSetVector( PWVSDBL( new WVSDBL( 1, 3, vecdata[0], W_DT_DOUBLE ) ), grid ) ) );
    m_evecOutputs[1]->updateData( PDSV( new WDataSetVector( PWVSDBL( new WVSDBL( 1, 3, vecdata[1], W_DT_DOUBLE ) ), grid ) ) );
    m_evecOutputs[2]->updateData( PDSV( new WDataSetVector( PWVSDBL( new WVSDBL( 1, 3, vecdata[2], W_DT_DOUBLE ) ), grid ) ) );
    m_evecOutputs[3]->updateData( PDSV( new WDataSetVector( PWVSDBL( new WVSDBL( 1, 3, vecdata[3], W_DT_DOUBLE ) ), grid ) ) );
    m_evalOutputs[0]->updateData( PDSS( new WDataSetScalar( PWVSDBL( new WVSDBL( 0, 1, valdata[0], W_DT_DOUBLE ) ), grid ) ) );
    m_evalOutputs[1]->updateData( PDSS( new WDataSetScalar( PWVSDBL( new WVSDBL( 0, 1, valdata[1], W_DT_DOUBLE ) ), grid ) ) );
    m_evalOutputs[2]->updateData( PDSS( new WDataSetScalar( PWVSDBL( new WVSDBL( 0, 1, valdata[2], W_DT_DOUBLE ) ), grid ) ) );
}

boost::array< double, 12 > WMEigenSystem::computeEigenSystem( WTensorSym< 2, 3, double > const& m ) const
{
    RealEigenSystem sys;
    jacobiEigenvector3D( m, &sys );
    boost::array< double, 12 > result = { { sys[0].first, sys[0].second[0],
                                                          sys[0].second[1],
                                                          sys[0].second[2],
                                            sys[1].first, sys[1].second[0],
                                                          sys[1].second[1],
                                                          sys[1].second[2],
                                            sys[2].first, sys[2].second[0],
                                                          sys[2].second[1],
                                                          sys[2].second[2] } }; // NOLINT
    return result;
}

WMEigenSystem::TPVOFloat::OutTransmitType const WMEigenSystem::eigenFuncFloat( TPVOFloat::TransmitType const& input )
{
    WTensorSym< 2, 3, double > m;
    m( 0, 0 ) = static_cast< double >( input[ 0 ] );
    m( 0, 1 ) = static_cast< double >( input[ 1 ] );
    m( 0, 2 ) = static_cast< double >( input[ 2 ] );
    m( 1, 1 ) = static_cast< double >( input[ 3 ] );
    m( 1, 2 ) = static_cast< double >( input[ 4 ] );
    m( 2, 2 ) = static_cast< double >( input[ 5 ] );

    ++*m_currentProgress;

    return computeEigenSystem( m );
}

WMEigenSystem::TPVODouble::OutTransmitType const WMEigenSystem::eigenFuncDouble( TPVODouble::TransmitType const& input )
{
    WTensorSym< 2, 3, double > m;
    m( 0, 0 ) = static_cast< double >( input[ 0 ] );
    m( 0, 1 ) = static_cast< double >( input[ 1 ] );
    m( 0, 2 ) = static_cast< double >( input[ 2 ] );
    m( 1, 1 ) = static_cast< double >( input[ 3 ] );
    m( 1, 2 ) = static_cast< double >( input[ 4 ] );
    m( 2, 2 ) = static_cast< double >( input[ 5 ] );

    ++*m_currentProgress;

    return computeEigenSystem( m );
}

void WMEigenSystem::resetProgress( std::size_t todo, std::string name )
{
    if( m_currentProgress )
    {
        m_currentProgress->finish();
    }
    m_currentProgress = boost::shared_ptr< WProgress >( new WProgress( name, todo ) );
    m_progress->addSubProgress( m_currentProgress );
}

void WMEigenSystem::resetEigenFunction( boost::shared_ptr< WDataSetDTI > tensors )
{
    // check if we need to stop the currently running eigencomputation
    if( m_eigenPool )
    {
        debugLog() << "Stopping eigencomputation.";
        WThreadedFunctionStatus s = m_eigenPool->status();
        if( s != W_THREADS_FINISHED && s != W_THREADS_ABORTED )
        {
            m_eigenPool->stop();
            m_eigenPool->wait();
            s = m_eigenPool->status();
            WAssert( s == W_THREADS_FINISHED || s == W_THREADS_ABORTED, "" );
        }
        m_moduleState.remove( m_eigenPool->getThreadsDoneCondition() );
    }
    // the threadpool should have finished computing by now

    m_eigenOperationFloat = boost::shared_ptr< TPVOFloat >();
    m_eigenOperationDouble = boost::shared_ptr< TPVODouble >();

    // create a new one
    if( tensors->getValueSet()->getDataType() == W_DT_DOUBLE )
    {
        m_eigenOperationDouble = boost::shared_ptr< TPVODouble >( new TPVODouble( tensors,
                                                boost::bind( &WMEigenSystem::eigenFuncDouble, this, _1 ) ) );
        m_eigenPool = boost::shared_ptr< WThreadedFunctionBase >( new EigenFunctionTypeDouble( W_AUTOMATIC_NB_THREADS, m_eigenOperationDouble ) );
        m_moduleState.add( m_eigenPool->getThreadsDoneCondition() );
    }
    else if( tensors->getValueSet()->getDataType() == W_DT_FLOAT )
    {
        m_eigenOperationFloat = boost::shared_ptr< TPVOFloat >( new TPVOFloat( tensors,
                                                boost::bind( &WMEigenSystem::eigenFuncFloat, this, _1 ) ) );
        m_eigenPool = boost::shared_ptr< WThreadedFunctionBase >( new EigenFunctionTypeFloat( W_AUTOMATIC_NB_THREADS, m_eigenOperationFloat ) );
        m_moduleState.add( m_eigenPool->getThreadsDoneCondition() );
    }
    else
    {
        errorLog() << "Input data does not contain floating point values, skipping.";
        m_eigenPool.reset();
    }
}