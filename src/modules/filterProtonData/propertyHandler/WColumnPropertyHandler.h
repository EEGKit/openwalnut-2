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

#ifndef WCOLUMNPROPERTYHANDLER_H
#define WCOLUMNPROPERTYHANDLER_H

#include <tuple>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <boost/function.hpp>

#include "core/common/WItemSelectionItemTyped.h"

#include "../WProtonData.h"
#include "../WSingleSelectorName.h"

/**
 * Creates, updates and handles the column properties.
 */
class WColumnPropertyHandler
{
public:
    /**
     * tuple with 3 string as value
     * 1. Name of single-selector,
     * 2. Desciption of single-selector,
     * 3. Value that is searched for in the csv header
     */
    typedef std::tuple< std::string, std::string, std::string, std::string > NameDescriptionSearchTyp;

    /**
     * Function variables for updating the data 
     */
    typedef boost::function< void( ) > CallbackPtr;

    /**
     * shared_ptr that points to itself 
     */
    typedef boost::shared_ptr< WColumnPropertyHandler > SPtr;

    /**
     * constructor
     *
     * \param protonData Pointer to the content and header of the CSV 
     * \param properties A property variable that is generated by the WModul 
     * \param dataUpdate A function variable that reinitializes the WDataSets 
     */
    explicit WColumnPropertyHandler( WProtonData::SPtr protonData,
                                WPropertyGroup::SPtr properties,
                                WColumnPropertyHandler::CallbackPtr dataUpdate );

    /**
     * creates the group property and the subproperty    
     */
    void createProperties();

    /**
     * update current group property and subproperty
     */
    void updateProperty();

    /**
     * setter to use a external function
     * \param externEventMethod A void function variable that can be use outside
     */
    void setSelectionEventMethod( WColumnPropertyHandler::CallbackPtr externEventMethod );

private:
    /**
     * A map between WPropSelection items and column names as strings
     */
     std::map< WPropSelection, std::string > mapPropSelectionsToString;

    /**
     * Pointer to the content and header of the CSV 
     */
    WProtonData::SPtr m_protonData;

    /**
     * A property variable that is generated by the WModul 
     */
    WPropertyGroup::SPtr m_properties;

    /**
     * A function variable that reinitializes the WDataSets 
     */
    WColumnPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * A void function variable that can be use outside
     */
    WColumnPropertyHandler::CallbackPtr m_externEventMethod;

    /**
     * Creates the individual WItemSelection
     * \param ndst ( n = Name, d = Description, s = Search, t = Typ )
     * \return WPropSelection The created selection
     */
    WPropSelection addHeaderProperty( WColumnPropertyHandler::NameDescriptionSearchTyp ndst,
                                    WPropertyBase::PropertyChangeNotifierType notifier );

    /**
     * Event function when WItemSelection is triggered
     * \param property The changed property
     */
    void propertyNotifier( WPropertyBase::SPtr property );

    /**
     * Property group for column selection
     */
    WPropGroup m_columnSelectionGroup;

    /**
     * represents the item type for item-selection
     */
    typedef WItemSelectionItemTyped< std::string > ItemType;

    /**
     * Reresents an entry in a < WPropSelection, string > map
     */
     typedef std::map< WPropSelection, std::string >::iterator PropMapEntry;

    /**
     * creates the content (options) of the WItemSelection 
     * \param typeName Type of column.
     * \return content of WItemSelection
     */
    boost::shared_ptr< WItemSelection > InitializeSelectionItem( std::string typeName );

    /**
     * converts the index to the filtered index  
     * \param index index to search.
     * \param typeName Type of column.
     * \return filterd index
     */
    int getfilterIndex( int index, std::string typeName );
};

#endif  // WCOLUMNPROPERTYHANDLER_H
