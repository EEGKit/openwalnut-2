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

#ifndef WPROPERTIES_H
#define WPROPERTIES_H

#include <map>
#include <string>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>

#include "WSharedObject.h"
#include "WPropertyBase.h"
#include "WPropertyVariable.h"

/**
 * class to manage properties of an object and to provide convinience methods for easy access and manipulation. It also allows
 * thread safe iteration on its elements.
 */
class WProperties
{
public:

    // the following typedefs are for convenience.

    /**
     * For shortening: a type defining a shared vector of WSubject pointers.
     */
    typedef std::vector< boost::shared_ptr< WPropertyBase > > PropertyContainerType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedObject< PropertyContainerType > PropertySharedContainerType;

    /**
     * The access type
     */
    typedef PropertySharedContainerType::WSharedAccess PropertyAccessType;

    /**
     * The const iterator type of the container.
     */
    typedef PropertyContainerType::const_iterator PropertyConstIterator;

    /**
     * The iterator type of the container.
     */
    typedef PropertyContainerType::iterator PropertyIterator;

    /**
     * standard constructor
     */
    WProperties();

    /**
     * destructor
     */
    virtual ~WProperties();

    /**
     * Simply insert the specified property to the list.
     *
     * \param prop the property to add
     */
    void addProperty( boost::shared_ptr< WPropertyBase > prop );

    /**
     * Helper function that finds a property by its name. Use this method to find out whether the property exists or not, since
     * findProperty throws an exception.
     *
     * \param name name of searched property.
     */
    bool existsProperty( std::string name );

    /**
     * Function searches the property. If it does not exists, it throws an exception.
     *
     * \param name the name of the property
     *
     * \return a WProperty object
     */
    boost::shared_ptr< WPropertyBase > getProperty( std::string name );

    /**
     * Searches the property with a given name. It does not throw any exception. It simply returns NULL if it can't be found.
     *
     * \param name the name of the property to search
     *
     * \return the property or NULL if not found.
     */
    boost::shared_ptr< WPropertyBase > findProperty( std::string name );

    /**
     * Returns the access object usable to iterate/modify the property list in a thread safe manner.
     *
     * \return the access control object.
     */
    PropertySharedContainerType::WSharedAccess getAccessObject();

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods to create and add properties
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    template< typename T>
    boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
                                                             boost::shared_ptr< WCondition > condition,
                                                             WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods to create and add properties
    // NOTE: these methods use the type of the initial parameter to automatically use the proper type.
    // This works, since the compiler always calls the function with the best matching parameter types.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor         addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&  initial, bool hide = false );


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          boost::shared_ptr< WCondition > condition, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param condition use this external condition for notification.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                boost::shared_ptr< WCondition > condition, bool hide = false );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION& initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&  initial,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // convenience methods for
    // template< typename T>
    // boost::shared_ptr< WPropertyVariable< T > > addProperty( std::string name, std::string description, const T& initial,
    //                                                          boost::shared_ptr< WCondition > condition,
    //                                                          WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropBool      addProperty( std::string name, std::string description, const WPVBaseTypes::PV_BOOL&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropInt       addProperty( std::string name, std::string description, const WPVBaseTypes::PV_INT&    initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     * It also sets the min and max constraint to [0,100].
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropDouble    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_DOUBLE& initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );


    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropString    addProperty( std::string name, std::string description, const WPVBaseTypes::PV_STRING& initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropFilename  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_PATH&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropSelection addProperty( std::string name, std::string description, const WPVBaseTypes::PV_SELECTION&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropPosition  addProperty( std::string name, std::string description, const WPVBaseTypes::PV_POSITION&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

    /**
     * Create and add a new property of the template type. For more details see appropriate constructor ow WPropertyVariable.
     *
     * \see WPropertyVariable
     *
     * \param name  the property name
     * \param description the property description
     * \param initial the initial value
     * \param notifier use this notifier for change callbacks.
     * \param condition use this external condition for notification
     * \param hide set to true to set the hide flag directly.
     *
     * \return the newly created property variable instance.
     */
    WPropColor     addProperty( std::string name, std::string description, const WPVBaseTypes::PV_COLOR&   initial,
                                boost::shared_ptr< WCondition > condition,
                                WPropertyBase::PropertyChangeNotifierType notifier, bool hide = false );

private:

    /**
     * The set of proerties. This uses the operators ==,<,> WProperty to determine equalness.
     */
    PropertySharedContainerType m_properties;

    /**
     * Access to the above property list.
     */
    PropertySharedContainerType::WSharedAccess m_propAccess;
};

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial,
                                                                       boost::shared_ptr< WCondition > condition, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, condition )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial,
                                                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, notifier )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

template< typename T>
boost::shared_ptr< WPropertyVariable< T > > WProperties::addProperty( std::string name, std::string description, const T& initial,
                                                                       boost::shared_ptr< WCondition > condition,
                                                                       WPropertyBase::PropertyChangeNotifierType notifier, bool hide )
{
    boost::shared_ptr< WPropertyVariable< T > > p = boost::shared_ptr< WPropertyVariable< T > >(
            new WPropertyVariable< T >( name, description, initial, condition, notifier )
    );
    p->setHidden( hide );
    addProperty( p );
    return p;
}

#endif  // WPROPERTIES_H
