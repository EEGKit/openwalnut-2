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
#include <algorithm>

#include <boost/tokenizer.hpp>

#include "WLogger.h"
#include "exceptions/WPropertyUnknown.h"

#include "WPropertyHelper.h"

#include "WPropertyGroup.h"

WPropertyGroup::WPropertyGroup( std::string name, std::string description ):
    WPropertyGroupBase( name, description )
{
    // an empty list is automatically configured for us in WPropertyGroupBase
}

WPropertyGroup::~WPropertyGroup()
{
    // clean up
}

WPropertyGroup::WPropertyGroup( const WPropertyGroup& from ):
    WPropertyGroupBase( from )
{
    // an exact (deep) copy already is generated by WPropertyGroupBase. We do not have any additional members
}

boost::shared_ptr< WPropertyBase > WPropertyGroup::clone()
{
    // class copy constructor.
    return boost::shared_ptr< WPropertyGroup >( new WPropertyGroup( *this ) );
}

PROPERTY_TYPE WPropertyGroup::getType() const
{
    return PV_GROUP;
}

bool WPropertyGroup::setAsString( std::string /*value*/ )
{
    // groups can't be set in any way. -> ignore it.
    return true;
}

std::string WPropertyGroup::getAsString()
{
    // groups can't be set in any way. -> ignore it.
    return "";
}

/**
 * Add the default constraints for a certain type of property. By default, nothing is added.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
template< typename T >
T _addDefaultConstraints( T prop )
{
    return prop;
}

/**
 * Add the default constraints for a certain type of property. For selections, the PC_ISVALID constraint is added.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
WPropSelection _addDefaultConstraints( WPropSelection prop )
{
    WPropertyHelper::PC_ISVALID::addTo( prop );
    return prop;
}

/**
 * Add the default constraints for a certain type of property. For filenames, the PC_NOTEMPTY constraint is added.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property
 *
 * \return the property inserted gets returned.
 */
WPropFilename _addDefaultConstraints( WPropFilename prop )
{
    WPropertyHelper::PC_NOTEMPTY::addTo( prop );
    return prop;
}

/**
 * Add the default constraints for a certain type of property. Please specialize _addDefaultConstraints for your special needs and prop types.
 *
 * \note Information properties never get constraints by default
 *
 * \param prop the property to add the constraints to
 *
 * \return the property inserted
 */
template< typename T >
T addDefaultConstraints( T prop )
{
    if( prop->getPurpose() == PV_PURPOSE_INFORMATION )
    {
        return prop;
    }

    return _addDefaultConstraints( prop );
}

bool WPropertyGroup::set( boost::shared_ptr< WPropertyBase > value, bool recommendedOnly )
{
    // is this the same type as we are?
    WPropertyGroup::SPtr v = boost::shared_dynamic_cast< WPropertyGroup >( value );
    if( !v )
    {
        // it is not a WPropertyStruct with the same type
        return false;
    }

    // forward, use empty exclude list
    return set( v, std::vector< std::string >(), recommendedOnly );
}

bool WPropertyGroup::set( boost::shared_ptr< WPropertyGroup > value, std::vector< std::string > exclude, bool recommendedOnly )
{
    setImpl( value, "", exclude, recommendedOnly );
}

bool WPropertyGroup::setImpl( boost::shared_ptr< WPropertyGroup > value, std::string path, std::vector< std::string > exclude, bool recommendedOnly )
{
    // go through each of the given child props
    WPropertyGroup::PropertySharedContainerType::ReadTicket r = value->getReadTicket();
    size_t c = 0;   // number of props we have set
    for( WPropertyGroupBase::PropertyConstIterator it = r->get().begin(); it != r->get().end(); ++it )
    {
        // do we have a property named the same as in the source props?
        WPropertyBase::SPtr prop = findProperty( ( *it )->getName() );
        if( !prop )
        {
            // not found. Ignore it. We cannot set the target property as the source did not exist
            continue;
        }

        // ok there it is. check exclude list.
        // first: use the current property name and append it to path
        std::string completePath = path + WPropertyGroupBase::separator + ( *it )->getName();
        if( path == "" )
        {
            // no separator if the path is empty now
            completePath = ( *it )->getName();
        }

        // now check exclude list
        if( std::find( exclude.begin(), exclude.end(), completePath ) != exclude.end() )
        {
            // it is excluded
            continue;
        }

        // not excluded. Is it a group or something else?
        WPropertyGroup::SPtr meAsGroup = boost::shared_dynamic_cast< WPropertyGroup >( prop );
        WPropertyGroup::SPtr inputAsGroup = boost::shared_dynamic_cast< WPropertyGroup >( *it );
        if( inputAsGroup && meAsGroup )
        {
            // not excluded and is group, recurse:
            c += meAsGroup->setImpl( inputAsGroup, completePath, exclude, recommendedOnly );
        }
        else if( inputAsGroup || meAsGroup ) // one group and one not a group, skip
        {
            continue;
        }
        else
        {
            c += prop->set( *it, recommendedOnly );
        }
    }

    // success only if all props have been set
    // NOTE: it think this only will ever be correct if we have no nested groups ...
    return ( c == r->get().size() );
}

void WPropertyGroup::removeProperty( boost::shared_ptr< WPropertyBase > prop )
{
    if( !prop )
    {
        return;
    }

    // lock, unlocked if l looses focus
    PropertySharedContainerType::WriteTicket l = m_properties.getWriteTicket();
    l->get().erase( std::remove( l->get().begin(), l->get().end(), prop ), l->get().end() );
    m_updateCondition->remove( prop->getUpdateCondition() );
}

WPropGroup WPropertyGroup::addPropertyGroup( std::string name, std::string description, bool hide )
{
    WPropGroup p = WPropGroup( new WPropertyGroup( name, description ) );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

void WPropertyGroup::clear()
{
    // lock, unlocked if l looses focus
    PropertySharedContainerType::WriteTicket l = m_properties.getWriteTicket();
    l->get().clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, hide ) );
}

WPropInt WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, hide ) );
}

WPropDouble WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, hide ) );
}

WPropString WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, hide ) );
}

WPropFilename WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, hide ) );
}

WPropSelection WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, hide ) );
}

WPropPosition WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, hide ) );
}

WPropColor WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, hide ) );
}

WPropTrigger WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, hide ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, hide ) );
}

WPropInt WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, hide ) );
}

WPropDouble WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, hide ) );
}

WPropString WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, hide ) );
}

WPropFilename WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, hide ) );
}

WPropSelection WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                          boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, condition, hide ) );
}

WPropPosition WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, hide ) );
}

WPropColor WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, hide ) );
}

WPropTrigger WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, condition, hide ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, notifier, hide ) );
}

WPropInt WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                    WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, notifier, hide ) );
}

WPropDouble WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, notifier, hide ) );
}

WPropString WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, notifier, hide ) );
}

WPropFilename WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, notifier, hide ) );
}

WPropSelection WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, notifier, hide ) );
}

WPropPosition WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, notifier, hide ) );
}

WPropColor WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, notifier, hide ) );
}

WPropTrigger WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, notifier, hide ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// convenience methods for
// template< typename T>
// boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
//                                                          boost::shared_ptr< WCondition > condition,
//                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WPropBool WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_BOOL >( name, description, initial, condition, notifier, hide ) );
}

WPropInt WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_INT >( name, description, initial, condition, notifier, hide ) );
}

WPropDouble WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_DOUBLE >( name, description, initial, condition, notifier, hide ) );
}

WPropString WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_STRING >( name, description, initial, condition, notifier, hide ) );
}

WPropFilename WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_PATH >( name, description, initial, condition, notifier, hide ) );
}

WPropSelection WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                          boost::shared_ptr< WCondition > condition,
                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_SELECTION >( name, description, initial, condition, notifier, hide ) );
}

WPropPosition WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                         boost::shared_ptr< WCondition > condition,
                                         WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_POSITION >( name, description, initial, condition, notifier, hide ) );
}

WPropColor WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                     boost::shared_ptr< WCondition > condition,
                                     WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_COLOR >( name, description, initial, condition, notifier, hide ) );
}

WPropTrigger WPropertyGroup::addProperty( std::string name, std::string description, const WPVBaseTypes::PV_TRIGGER&   initial,
                                       boost::shared_ptr< WCondition > condition,
                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    return addDefaultConstraints( addProperty< WPVBaseTypes::PV_TRIGGER >( name, description, initial, condition, notifier, hide ) );
}

