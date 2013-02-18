//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMDATAHANDLERTEMPLATE_H
#define WMDATAHANDLERTEMPLATE_H

#include <string>

#include <osg/Geode>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../newDataHandler/WGridRegular3D2.h"
#include "../../newDataHandler/WGridRegular3D2Specializations.h"
#include "../../newDataHandler/structuralTypes/WScalarStructural.h"
#include "../../newDataHandler/WDataSet2.h"

/** 
 * \class WMDataHandlerTemplate
 *
 * A template module that shows the usage of the WDataSet2<> template and the dataset iterators.
 * 
 * \ingroup modules
 */
class WMDataHandlerTemplate: public WModule
{
public:

    /**
     * Constructor.
     */
    WMDataHandlerTemplate();

    /**
     * Destructor.
     */
    virtual ~WMDataHandlerTemplate();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
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
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:

    /**
     * Do various things with a scalar dataset.
     *
     * \param ds The dataset to do various things with.
     */
    void processScalarSet( WDataSet2< WGridRegular3D2, WScalarStructural >::ConstSPtr const& ds );

    /**
     * An example on how to use a visitor to get the type of the data.
     *
     * \param ds The dataset.
     */
    void processScalarSetCorrectType( WDataSet2< WGridRegular3D2, WScalarStructural >::ConstSPtr const& ds );

    /**
     * An example on how to use neighborhood iterators. Implements an erosion operation.
     *
     * \param ds The dataset.
     */
    void erodeScalarSet( WDataSet2< WGridRegular3D2, WScalarStructural >::ConstSPtr const& ds );

    /**
     * An example on how to use slice iterators. Implements creation of a synthetic vector dataset.
     */
    void createVectorSet();

    //! An input connector for the data.
    boost::shared_ptr< WModuleInputData< WDataSet2Base > > m_input;

    //! An output connector.
    boost::shared_ptr< WModuleOutputData< WDataSet2Base > > m_output;

    //! The data.
    boost::shared_ptr< WDataSet2Base > m_dataSet;
};

#endif  // WMDATAHANDLERTEMPLATE_H