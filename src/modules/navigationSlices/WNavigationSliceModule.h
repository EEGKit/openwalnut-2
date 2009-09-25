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

#ifndef WNAVIGATIONSLICEMODULE_H
#define WNAVIGATIONSLICEMODULE_H

#include <string>

#include <osg/Node>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputConnector.h"
#include "../../graphicsEngine/WShader.h"


/**
 * \par Description:
 * Simple module for testing some WKernel functionality.
 */
class WNavigationSliceModule: public WModule
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    WNavigationSliceModule();

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WNavigationSliceModule();

    /**
     * \par Description
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * \par Description
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

protected:

    /**
     * \par Description
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

    /** 
     * Initialize the connectors this module is using.
     */
    virtual void initializeConnectors();

    /** 
     * Receive DATA_CHANGE notifications.
     * 
     * \param input the input connector that got the change signal. Typically it is one of the input connectors from this module.
     * \param output the output connector that sent the signal. Not part of this module instance.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );

private:
    /**
     * TODO(schurade): add comment
     */
    void createSlices();

    /**
     * TODO(schurade): add comment
     */
    osg::Geode* m_sliceNode;

    /**
     * TODO(schurade): add comment
     */
    boost::shared_ptr< WShader >m_shader;

    /** 
     * Input connector required by this module.
     */
    boost::shared_ptr<WModuleInputConnector> m_Input;
};

#endif  // WNAVIGATIONSLICEMODULE_H

