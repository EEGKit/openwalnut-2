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

#ifndef WMDATACREATORPOINTS_H
#define WMDATACREATORPOINTS_H

#include <memory>
#include <string>

#include "WDataSetPointsCreatorInterface.h"
#include "core/common/WObjectNDIP.h"
#include "core/common/WStrategyHelper.h"
#include "core/dataHandler/WDataSetPoints.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * Module which utilizes the strategy pattern to provide a multitude of dataset creation algorithms for point data.
 *
 * \ingroup modules
 */
class WMDataCreatorPoints: public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMDataCreatorPoints();

    /**
     * Destructor.
     */
    ~WMDataCreatorPoints();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual std::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

private:
    /**
     * A condition used to notify about changes in several properties.
     */
    std::shared_ptr< WCondition > m_propCondition;

    std::shared_ptr< WModuleOutputData< WDataSetPoints > > m_output; //!< The only output of this module.

    /**
     * Number of points.
     */
    WPropInt m_numPoints;

    /**
     * The seed for the random.
     */
    WPropInt m_seed;

    /**
     * Point color.
     */
    WPropColor m_pointColor;

    /**
     * Size of the point bounding box.
     */
    WPropPosition m_size;

    /**
     * Origin of the bounding box.
     */
    WPropPosition m_origin;

    WStrategyHelper< WObjectNDIP< WDataSetPointsCreatorInterface > > m_strategy; //!< the strategy currently active.
};

#endif  // WMDATACREATORPOINTS_H
