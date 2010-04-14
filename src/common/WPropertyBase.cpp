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

#include <list>
#include <string>

#include <boost/filesystem.hpp>

#include "exceptions/WPropertyNameMalformed.h"

#include "WPropertyBase.h"
#include "WProperties.h"
#include "WPropertyVariable.h"

WPropertyBase::WPropertyBase( std::string name, std::string description ):
    m_name( name ),
    m_description( description ),
    m_hidden( false ),
    m_updateCondition( new WConditionSet() )
{
    // check name validity
    if ( ( m_name.find( std::string( "/" ) ) != std::string::npos ) || m_name.empty() )
    {
        throw WPropertyNameMalformed( "Property name \"" + name +
                                      "\" is malformed. Do not use slashes (\"/\") or empty strings in property names." );
    }
}

WPropertyBase::~WPropertyBase()
{
    // cleanup
}

std::string WPropertyBase::getName() const
{
    return m_name;
}

std::string WPropertyBase::getDescription() const
{
    return m_description;
}

PROPERTY_TYPE WPropertyBase::getType() const
{
    return m_type;
}

void WPropertyBase::updateType()
{
    m_type = PV_UNKNOWN;
}

bool WPropertyBase::isHidden() const
{
    return m_hidden;
}

void WPropertyBase::setHidden( bool hidden )
{
    if ( m_hidden != hidden )
    {
        m_hidden = hidden;
        m_updateCondition->notify();
    }
}

WPropInt WPropertyBase::toPropInt()
{
    return boost::shared_static_cast< WPVInt >( shared_from_this() );
}

WPropDouble WPropertyBase::toPropDouble()
{
    return boost::shared_static_cast< WPVDouble >( shared_from_this() );
}

WPropBool WPropertyBase::toPropBool()
{
    return boost::shared_static_cast< WPVBool >( shared_from_this() );
}

WPropString WPropertyBase::toPropString()
{
    return boost::shared_static_cast< WPVString >( shared_from_this() );
}

WPropFilename WPropertyBase::toPropFilename()
{
    return boost::shared_static_cast< WPVFilename >( shared_from_this() );
}

WPropSelection WPropertyBase::toPropSelection()
{
    return boost::shared_static_cast< WPVSelection >( shared_from_this() );
}

WPropColor WPropertyBase::toPropColor()
{
    return boost::shared_static_cast< WPVColor >( shared_from_this() );
}

WPropPosition WPropertyBase::toPropPosition()
{
    return boost::shared_static_cast< WPVPosition >( shared_from_this() );
}

WPropGroup WPropertyBase::toPropGroup()
{
    return boost::shared_static_cast< WPVGroup >( shared_from_this() );
}

WPropTrigger WPropertyBase::toPropTrigger()
{
    return boost::shared_static_cast< WPVTrigger >( shared_from_this() );
}

boost::shared_ptr< WCondition > WPropertyBase::getUpdateCondition() const
{
    return m_updateCondition;
}

