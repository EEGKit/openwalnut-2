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

#ifndef WEVENTIDLIMITATIONPROPERTYHANDLER_H
#define WEVENTIDLIMITATIONPROPERTYHANDLER_H

#include "../WProtonData.h"


/**
 * Creates, updates and handles the EventID properties.
 */
class WEventIDLimitationPropertyHandler
{
public:
    /**
     * Function variables for updating the data 
     */
    typedef boost::shared_ptr< WEventIDLimitationPropertyHandler > SPtr;

    /**
     * shared_ptr that points to itself 
     */
    typedef boost::function< void( ) > CallbackPtr;

    /**
     * constructor
     *
     * \param protonData Pointer to the content and header of the CSV 
     * \param properties A property variable that is generated by the WModul 
     * \param dataUpdate A function variable that reinitializes the WDataSets 
     */
    explicit WEventIDLimitationPropertyHandler( WProtonData::SPtr protonData,
                                                WPropertyGroup::SPtr properties,
                                                WEventIDLimitationPropertyHandler::CallbackPtr dataUpdate );

    /**
     * creates the group property and the subproperty    
     */
    void createProperties();

    /**
     * update current group property and subproperty    
     */
    void updateProperty();

    /**
     * Getter
     *
     * \return WPropInt of the MinCap
     */
    WPropInt getMinCap();

    /**
     * Getter
     *
     * \return WPropInt of the MaxCap
     */
    WPropInt getMaxCap();

private:
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
    WEventIDLimitationPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * Set lower border of range of eventID selection.
     */
    WPropInt m_minCap;

    /**
     * Set upper border of range of eventID selection.
     */
    WPropInt m_maxCap;

    /**
     * Apply the current event ID selection
     */
    WPropTrigger m_applySelection;

    /**
     * Update your mesh when changing properties.
     */
    void updateMesh( );

     /**
     * Determines smalles und biggest eventID.
     */
    void determineMinMaxEventID();
};
#endif  // WEVENTIDLIMITATIONPROPERTYHANDLER_H
