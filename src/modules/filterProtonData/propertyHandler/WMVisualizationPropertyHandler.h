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

#ifndef WMVISUALIZATIONPROPERTYHANDLER_H
#define WMVISUALIZATIONPROPERTYHANDLER_H

#include "../WMProtonData.h"

/**
 * Creates, updates and handles the visualization properties.
 */
class WMVisualizationPropertyHandler
{
public:
    /**
     * Function variables for updating the data 
     */
    typedef boost::function< void( ) > CallbackPtr;

    /**
     * shared_ptr that points to itself 
     */
    typedef boost::shared_ptr< WMVisualizationPropertyHandler > SPtr;

    /**
     * constructor
     *
     * \param protonData Pointer to the content and header of the CSV 
     * \param properties A property variable that is generated by the WModul 
     * \param dataUpdate A function variable that reinitializes the WDataSets 
     */
    explicit WMVisualizationPropertyHandler( WMProtonData::SPtr protonData,
                                        WPropertyGroup::SPtr properties,
                                        WMVisualizationPropertyHandler::CallbackPtr dataUpdate );

    /**
     * creates the group property and the subproperty    
     */
    void createProperties();

    /**
     * update current group property and subproperty    
     */
    void updateProperty();

    /**
     * toggle the visibility of plain color and transfer function
     */
    void toggleColorProperties();

    /**
     * Getter
     *
     * \return WPropBool of color from edep
     */
    WPropBool getColorFromEdep();

    /**
     * Getter
     *
     * \return WPropBool of sizes from edep
     */
    WPropBool getSizesFromEdep();

    /**
     * Getter
     *
     * \return WPropBool of color from selection
     */
    WPropColor getColorSelection();

    /**
     * Getter
     * \return WPropTransferFunction of the current transfer function
     */
    WPropTransferFunction getTransferFunction();

    /**
     * Sets the transfer function
     * \return WTransferFunction with specified gradient
     */
    WTransferFunction setColorGradient();

private:
    /**
     * Reload data when properties for selection of primaries and secondaries changed
     *
     * \param property contains reference to the property which called updateProperty()
     */
    void propertyCallback( WPropertyBase::SPtr property );

    /**
     * Pointer to the content and header of the CSV 
     */
    WMProtonData::SPtr m_protonData;

    /**
     * A property variable that is generated by the WModul 
     */
    WPropertyGroup::SPtr m_properties;

    /**
     * A function variable that reinitializes the WDataSets 
     */
    WMVisualizationPropertyHandler::CallbackPtr m_dataUpdate;

    /**
     * Property group for visualization modes
     */
    WPropGroup m_visualizationGroup;

    /**
     * Decides whether to color points based on the deposited energy or not.
     */
    WPropBool m_colorFromEdep;

    /**
     * Decides whether to scale the points sizes according to their deposited energy.
     */
    WPropBool m_sizesFromEdep;

    /**
     * Property that holds the current selected color when not coloring points based on their deposited energy.
     */
    WPropColor m_colorSelection;

    /**
     * Property that holds the current transfer function for the gradient.
     */
    WPropTransferFunction m_gradient;

    /**
     * Property that holds a trigger to apply the current transfer function for the gradient
     */
    WPropTrigger m_applyGradient;
};

#endif  // WMVISUALIZATIONPROPERTYHANDLER_H
