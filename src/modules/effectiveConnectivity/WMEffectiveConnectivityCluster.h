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

#ifndef WMEFFECTIVECONNECTIVITYCLUSTER_H
#define WMEFFECTIVECONNECTIVITYCLUSTER_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>

#include "../../dataHandler/WDataSetFibers.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleContainer.h"
#include "../../kernel/WModuleInputForwardData.h"
#include "../../kernel/WModuleOutputForwardData.h"
#include "../../graphicsEngine/WGEGroupNode.h"

/**
 * This module is able to visualize connectome data in the context of MRI data. It uses the module container class to allow the
 * module to be composed from other modules.
 * \ingroup modules
 */
class WMEffectiveConnectivityCluster : public WModuleContainer
{
public:
    /**
     * Standard constructor.
     */
    WMEffectiveConnectivityCluster();

    /**
     * Destructor.
     */
    ~WMEffectiveConnectivityCluster();

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
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
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

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * The fiber dataset used.
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetFibers > > m_fiberInput;

    /**
     * The volume of interest 1.
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetSingle > > m_VOI1;

    /**
     * The volume of interest 2.
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetSingle > > m_VOI2;

    boost::shared_ptr< WModule > m_fiberSelection;       //!< The fiber selection module.
    boost::shared_ptr< WModule > m_voxelizer;            //!< The voxelizer module.
    boost::shared_ptr< WModule > m_gauss;                //!< The gauss filter which filters the voxelized fibers.
    boost::shared_ptr< WModule > m_animation;            //!< The final animation.

    /////////////////////////////////////////////////////////////////////
    // The Properties
    /////////////////////////////////////////////////////////////////////

    /**
     * The name of the first region of interest
     */
    WPropString m_voi1Name;

    /**
     * The name of the first region of interest
     */
    WPropString m_voi2Name;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /////////////////////////////////////////////////////////////////////
    // OSG Stuff
    /////////////////////////////////////////////////////////////////////

    /**
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    osg::ref_ptr< WGEGroupNode > m_rootNode;

};

#endif  // WMEFFECTIVECONNECTIVITYCLUSTER_H
