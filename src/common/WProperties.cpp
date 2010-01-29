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
#include <map>
#include <string>
#include <vector>

#include "../common/WLogger.h"

#include "WProperties.h"

WProperties::WProperties()
{
}

WProperties::~WProperties()
{
}

boost::signals2::signal1< void, std::string >* WProperties::addBool( std::string name, bool value, bool hidden,
        std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}

boost::signals2::signal1< void, std::string >* WProperties::addChar( std::string name, char value, bool hidden,
        std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}


boost::signals2::signal1< void, std::string >* WProperties::addInt( std::string name, int value, bool hidden,
        std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}


boost::signals2::signal1< void, std::string >* WProperties::addFloat( std::string name, float value, bool hidden,
        std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}


boost::signals2::signal1< void, std::string >* WProperties::addDouble( std::string name, double value, bool hidden,
        std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}


boost::signals2::signal1< void, std::string >*
WProperties::addString( std::string name, std::string value, bool hidden, std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}

boost::signals2::signal1< void, std::string >* WProperties::addColor( std::string name, WColor value, bool hidden,
        std::string shortDesc, std::string longDesc )
{
    WProperty* prop = new WProperty( name, value, hidden, shortDesc, longDesc );
    m_propertyList[name] = prop;
    m_propertyVector.push_back( prop );
    return prop->getSignalValueChanged();
}

bool WProperties::existsProp( std::string name )
{
    return ( findProp( name ) != 0 );
}

WProperty* WProperties::findProp( std::string name )
{
    if( m_propertyList.count( name ) != 0 )
    {
        return m_propertyList[name];
    }
    else
    {
        // TODO(schurade): since the gui tries to set properties over all connected modules, the error
        // message here can be misleading, removing it for now
        // std::string msg( "Property doesn't exist: " + name );
        // WLogger::getLogger()->addLogMessage( msg, "Properties", LL_ERROR );
        return 0;
    }
}


std::string WProperties::getValueString( const std::string prop )
{
    if( findProp( prop ) )
    {
        return findProp( prop )->getValueString();
    }
    return "";
}

std::vector< WProperty* >& WProperties::getPropertyVector()
{
    return m_propertyVector;
}

void WProperties::hideProperty( std::string name )
{
    if ( findProp( name ) )
    {
        findProp( name )->hide();
    }
}

void WProperties::unhideProperty( std::string name )
{
    if ( findProp( name ) )
    {
        findProp( name )->unhide();
    }
}

void WProperties::reemitChangedValueSignals()
{
    std::vector< WProperty* >::iterator iter;
    for( iter = m_propertyVector.begin(); iter != m_propertyVector.end(); ++iter )
    {
        WProperty* property = *iter;
        if( property->isDirty() )
        {
            property->dirty( false );
            // Refire but don't change the value.
            property->signalValueChanged();
        }
    }
}

bool WProperties::isDirty() const
{
    std::vector< WProperty* >::const_iterator cit;
    for( cit = m_propertyVector.begin(); cit != m_propertyVector.end(); ++cit )
    {
        if( ( *cit )->isDirty() )
        {
            return true;
        }
    }
    return false;
}
