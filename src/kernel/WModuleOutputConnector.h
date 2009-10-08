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

#ifndef WMODULEOUTPUTCONNECTOR_H
#define WMODULEOUTPUTCONNECTOR_H

#include <string>

#include "boost/signals2/signal.hpp"
#include "boost/signals2/connection.hpp"

#include "WModule.h"
#include "WModuleConnector.h"
#include "WModuleConnectorSignals.h"

/**
 * Class implementing output connection functionality between modules.
 */
class WModuleOutputConnector: public WModuleConnector
{
public:

    // **************************************************************************************************************************
    // Methods
    // **************************************************************************************************************************

    /** 
     * Constructor.
     * 
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */    
    WModuleOutputConnector( boost::shared_ptr<WModule> module, std::string name="", std::string description="" );

    /**
     * Destructor.
     */
    virtual ~WModuleOutputConnector();

    /** 
     * Connects (subscribes) a specified notify function with a signal this module instance is offering.
     * 
     * \exception WModuleSignalSubscriptionFailed thrown if the signal can't be connected.
     *
     * \param signal the signal to connect to.
     * \param notifier the notifier function to bind.
     */
     boost::signals2::connection subscribeSignal( MODULE_CONNECTOR_SIGNAL signal, t_GenericSignalHandlerType notifier );

protected:

    /** 
     * Checks whether the specified connector is an input connector.
     * 
     * \param con the connector to check against.
     * 
     * \return true if compatible.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con );

    /** 
     * Connect additional signals.
     * 
     * \param con the connector that requests connection.
     * 
     */
    // If you want to add additional signals an output connector should subscribe FROM an input connector, overwrite
    // connectSignals
    // virtual void connectSignals( boost::shared_ptr<WModuleConnector> con );

    /** 
     * Propagates the signal "DATA_CHANGED" to all connected items.
     */
    virtual void propagateDataChange();

private:

    /** 
     * Signal fired whenever new data should be propagated. Represented by DATA_CHANGED enum- element.
     */
    t_GenericSignalType signal_DataChanged;
};

#endif  // WMODULEOUTPUTCONNECTOR_H

