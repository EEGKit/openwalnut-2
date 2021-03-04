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

#include <regex>
#include <string>
#include <vector>

#include "WMFilterPropertyHandler.h"

WMFilterPropertyHandler::WMFilterPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMFilterPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WMFilterPropertyHandler::createProperties()
{
    m_filteringGroup = m_properties->addPropertyGroup( "Filtering", "Filter primaries and secondaries" );

    createCheckBoxForPrimaryAndSecondary();
    createMultiSelectionForPDG();
    createPropToSetParticleNames();

    updateProperty();
}

void WMFilterPropertyHandler::updateProperty()
{
    m_currentColumnIndex = m_protonData->getColumnIndex( "PDGEncoding" );
    m_filteringGroup->removeProperty( m_multiSelection );
    createMultiSelectionForPDG();

    if( m_protonData->isColumnAvailable( "parentID" ) && m_protonData->isColumnAvailable( "trackID" ) )
    {
        m_showPrimaries->setHidden( false );
        m_showSecondaries->setHidden( false );
    }
    else
    {
        m_showPrimaries->setHidden( true );
        m_showSecondaries->setHidden( true );
    }

    if( m_protonData->isColumnAvailable( "PDGEncoding" ) )
    {
        m_multiSelection->setHidden( false );
    }
    else
    {
        m_multiSelection->setHidden( true );
    }
}

void WMFilterPropertyHandler::createCheckBoxForPrimaryAndSecondary()
{
    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind(
        &WMFilterPropertyHandler::updateCheckboxProperty, this, boost::placeholders::_1 );

    m_showPrimaries = m_filteringGroup->addProperty( "Show primaries", "Show/hide primaries", true, notifierCheckBox );
    m_showSecondaries = m_filteringGroup->addProperty( "Show secondaries", "Show/hide secondaries", true, notifierCheckBox );
}

void WMFilterPropertyHandler::searchPDGTypes()
{
    int pdgColumnIndex = m_protonData->getColumnIndex( "PDGEncoding" );

    if(pdgColumnIndex < 0)
    {
        return;
    }

    for( size_t idx = 0; idx < m_protonData->getCSVData()->size(); idx++)
    {
        std::vector< std::string > row = m_protonData->getCSVData()->at( idx );
        int currentParticleID = std::stoi( row.at( pdgColumnIndex ) );

        if( std::find( m_pdgTypes.begin(), m_pdgTypes.end(), currentParticleID ) == m_pdgTypes.end() )
        {
            m_pdgTypes.push_back( currentParticleID );
        }
    }
}

void WMFilterPropertyHandler::createMultiSelectionForPDG()
{
    WPropertyBase::PropertyChangeNotifierType pdgEncodingnotifier = boost::bind(
        &WMFilterPropertyHandler::updateSelectedPDGTypes, this );


    boost::shared_ptr< WItemSelection > possibleSelection = WItemSelection::SPtr( new WItemSelection() );

    m_pdgTypes.clear();
    searchPDGTypes();

    for( auto pdgType : m_pdgTypes )
    {
        possibleSelection->addItem(
                getParticleNameFromPdg( pdgType ) + "(" + std::to_string( pdgType ) + ")"
        );
    }

    if(m_pdgTypes.size() > 0)
    {
        m_multiSelection = m_filteringGroup->addProperty( "PDGEncoding", "Choose particle type(s) you want to show",
                                                                possibleSelection->getSelectorAll(), pdgEncodingnotifier );
    }
    else
    {
        m_multiSelection = m_filteringGroup->addProperty( "PDGEncoding", "Choose particle type(s) you want to show",
                                                                        possibleSelection->getSelectorNone(), pdgEncodingnotifier );
    }

    WItemSelector selectedItems = m_multiSelection->get( true );
    for( size_t i = 0; i < selectedItems.size(); ++i )
    {
        m_selectedPDGTypes.push_back( getPdgFromName( selectedItems.at( i )->getName() ) );
    }

    WPropertyHelper::PC_NOTEMPTY::addTo( m_multiSelection );
}

void WMFilterPropertyHandler::updateSelectedPDGTypes()
{
    m_dataUpdate( );
}

bool WMFilterPropertyHandler::isPDGTypeSelected( int pdgType )
{
    WItemSelector selectedItems = m_multiSelection->get( true );

    for( size_t i = 0; i < selectedItems.size(); ++i )
    {
        if(getPdgFromName( selectedItems.at( i )->getName()) == pdgType)
        {
            return true;
        }
    }
    return false;
}

void WMFilterPropertyHandler::updateCheckboxProperty( WPropertyBase::SPtr property )
{
    if( m_showPrimaries->get() || m_showSecondaries->get() )
    {
        m_dataUpdate( );
    }
    else
    {
        if( property == m_showPrimaries )
        {
            m_showPrimaries->set( true );
        }

        if( property == m_showSecondaries )
        {
            m_showSecondaries->set( true );
        }
    }
}

void WMFilterPropertyHandler::createPDGMap( std::string path )
{
    m_PdgParticelNamePath= path;

    std::fstream pdgSignFile;
    std::string dataRow;
    bool firstLine = true;
    std::string particleName;
    int pdg;

    pdgSignFile.open( path );

    if( !pdgSignFile.is_open() )
    {
        throw WException( "File could not be opened!" );
    }

    while( std::getline( pdgSignFile, dataRow ) )
    {
        if( firstLine )
        {
            firstLine = false;
            continue;
        }
        std::istringstream iss( dataRow );
        iss >> particleName >> pdg;

        m_PdgNamesByID.insert( PdgElement( particleName, pdg ) );
    }
    pdgSignFile.close();
}

std::string WMFilterPropertyHandler::getParticleNameFromPdg( int pdg )
{
    BM_PDG::right_const_iterator pdg_iter = m_PdgNamesByID.right.find( pdg );

    if( pdg_iter != m_PdgNamesByID.right.end() )
    {
        return pdg_iter->second;
    }
    else
    {
        return "unknown";
    }
}

int WMFilterPropertyHandler::getPdgFromName( std::string particleName )
{
    BM_PDG::left_const_iterator pdg_iter = m_PdgNamesByID.left.find( particleName.substr( 0, particleName.find( "(" ) ) );
    if( pdg_iter != m_PdgNamesByID.left.end() )
    {
        return pdg_iter->second;
    }
    else
    {
        std::regex regexp( "[-+0-9]+" );
        std::smatch m;
        std::regex_search( particleName, m, regexp );
        return std::stoi( m[0] );
    }
}

WPropBool WMFilterPropertyHandler::getShowPrimaries()
{
    return m_showPrimaries;
}

WPropBool WMFilterPropertyHandler::getShowSecondaries()
{
    return m_showSecondaries;
}

void WMFilterPropertyHandler::changePdgBiMap( int pdg, std::string newParticleName )
{
    BM_PDG::right_iterator pdg_iter = m_PdgNamesByID.right.find( pdg );

    if( pdg_iter != m_PdgNamesByID.right.end() )
    {
        bool success_replace = m_PdgNamesByID.right.replace_data( pdg_iter, newParticleName );
        if( !success_replace )
        {
            throw WException( "Fail to replace new particle name" );
        }
    }
    else    //unknown particle
    {
        m_PdgNamesByID.insert( PdgElement( newParticleName, pdg ) );
    }

    writePdgMapInParticleNameFile();
}
void WMFilterPropertyHandler::writePdgMapInParticleNameFile()
{
    std::ofstream pdgSignFile;

    pdgSignFile.open( m_PdgParticelNamePath, std::ios_base::out | std::ios::trunc );

    if( !pdgSignFile.is_open() )
    {
        throw WException( "File could not be opened!" );
    }

    for( auto pdgNameIterator = m_PdgNamesByID.begin(); pdgNameIterator != m_PdgNamesByID.end(); ++pdgNameIterator )
    {
        pdgSignFile << pdgNameIterator->left << " " << pdgNameIterator->right << "\n";
    }

    m_saveButton->set( WPVBaseTypes::PV_TRIGGER_READY, false );
    pdgSignFile.close();
}
