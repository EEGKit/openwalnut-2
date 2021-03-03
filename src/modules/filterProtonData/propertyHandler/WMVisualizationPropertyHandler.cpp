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

#include "WMVisualizationPropertyHandler.h"


WMVisualizationPropertyHandler::WMVisualizationPropertyHandler( WMProtonData::SPtr protonData,
                                            WPropertyGroup::SPtr properties,
                                            WMVisualizationPropertyHandler::CallbackPtr dataUpdate ):
    m_protonData( protonData ),
    m_properties( properties ),
    m_dataUpdate( dataUpdate )
{
}

void WMVisualizationPropertyHandler::createProperties()
{
    m_visualizationGroup = m_properties->addPropertyGroup( "Visualization", "Visualization options" );

    WPropertyBase::PropertyChangeNotifierType notifierCheckBox = boost::bind(
        &WMVisualizationPropertyHandler::updateCheckboxProperty, this, boost::placeholders::_1 );

    m_sizesFromEdep = m_visualizationGroup->addProperty( "Scale point size", "Scale point size with energy deposition", true, notifierCheckBox );
    m_colorFromEdep = m_visualizationGroup->addProperty( "Color by edep", "Color points based on energy deposition", true, notifierCheckBox );
    m_colorSelection = m_visualizationGroup->addProperty( "Plain color", "Choose how to color the points when not coloring by edep.",
        defaultColor::WHITE, notifierCheckBox );

    updateProperty();
}

void WMVisualizationPropertyHandler::updateProperty()
{
    if( m_protonData->IsColumnAvailable( "posX" ) &&
        m_protonData->IsColumnAvailable( "posY" ) &&
        m_protonData->IsColumnAvailable( "posZ" ) &&
        m_protonData->IsColumnAvailable( "edep" ) )
    {
        m_sizesFromEdep->setHidden( false );
        m_colorFromEdep->setHidden( false );
        m_colorSelection->setHidden( false );
    }
    else
    {
        m_sizesFromEdep->setHidden( true );
        m_colorFromEdep->setHidden( true );
        m_colorSelection->setHidden( true );
    }
}

void WMVisualizationPropertyHandler::updateCheckboxProperty( WPropertyBase::SPtr property )
{
    m_dataUpdate();
}

WPropBool WMVisualizationPropertyHandler::getColorFromEdep()
{
    return m_colorFromEdep;
}

WPropBool WMVisualizationPropertyHandler::getSizesFromEdep()
{
    return m_sizesFromEdep;
}

WPropColor WMVisualizationPropertyHandler::getColorSelection()
{
    return m_colorSelection;
}
