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

#ifndef WMODULE_H
#define WMODULE_H

#include <vector>
#include <string>
#include <typeinfo>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "WModuleCombinerTypes.h"
#include "WModuleConnectorSignals.h"
#include "WModuleSignals.h"
#include "WModuleTypes.h"

#include "../dataHandler/WDataSet.h"
#include "../dataHandler/WDataSetSingle.h"
#include "../dataHandler/WValueSet.h"

#include "../common/WLogger.h"
#include "../common/WProperties.h"
#include "../common/WProgressCombiner.h"
#include "../common/WProgress.h"
#include "../common/WThreadedRunner.h"
#include "../common/WPrototyped.h"
#include "../common/WConditionSet.h"

class WModuleConnector;
class WModuleInputConnector;
class WModuleOutputConnector;
class WModuleContainer;
class WModuleFactory;

/**
 * Class representing a single module of OpenWalnut.
 * \ingroup kernel
 */
class WModule: public WThreadedRunner,
               public WPrototyped,
               public boost::enable_shared_from_this< WModule >
{
friend class WModuleConnector;  // requires access to notify members
friend class WModuleFactory;    // for proper creation of module instaces, the factory needs access to protected functions.
                                // (especially initialize)
friend class WModuleContainer;  // for proper management of m_container WModuleContainer needs access.

public:

    /**
     * Constructs a new WModule instance
     */
    WModule();

    /**
     * Destructor.
     */
    virtual ~WModule();

    /**
     * The type for the list of input connectors.
     */
    typedef std::vector< boost::shared_ptr< WModuleInputConnector > > InputConnectorList;

    /**
     * The type for the list of output connectors.
     */
    typedef std::vector< boost::shared_ptr< WModuleOutputConnector > > OutputConnectorList;

    /**
     * Gives back input connectors.
     *
     * \return the input connectors.
     */
    const InputConnectorList& getInputConnectors() const;

    /**
     * Finds the named connector for the module.
     *
     * \param name the name. This can be a canonical name or the connector name.
     *
     * \return the connector.
     * \throw WModuleConnectorNotFound thrown whenever the module does not provide the specified connector.
     */
    boost::shared_ptr< WModuleInputConnector > getInputConnector( std::string name );

    /**
     * Finds the named connector for the module. This is similar to getInputConnector but it does not throw an exception if the connector could
     * not be found.
     *
     * \param name the name. This can be a canonical name or the connector name.
     *
     * \return the connector or NULL if not found
     */
    boost::shared_ptr< WModuleInputConnector > findInputConnector( std::string name );

    /**
     * Gives back output connectors.
     *
     * \return the output connectors.
     */
    const OutputConnectorList& getOutputConnectors() const;

    /**
     * Finds the named connector for the module.
     *
     * \param name the name. This can be a canonical name or the connector name.
     *
     * \return the connector.
     * \throw WModuleConnectorNotFound thrown whenever the module does not provide the specified connector.
     */
    boost::shared_ptr< WModuleOutputConnector > getOutputConnector( std::string name );

    /**
     * Finds the named connector for the module. This is similar to getOutputConnector but it does not throw an exception if the connector could
     * not be found.
     *
     * \param name the name. This can be a canonical name or the connector name.
     *
     * \return the connector or NULL if not found.
     */
    boost::shared_ptr< WModuleOutputConnector > findOutputConnector( std::string name );

    /**
     * Finds the named connector for the module. This searches for inputs and outputs.
     *
     * \param name the name. This can be a canonical name or the connector name.
     *
     * \return the connector.
     * \throw WModuleConnectorNotFound thrown whenever the module does not provide the specified connector.
     */
    boost::shared_ptr< WModuleConnector > getConnector( std::string name );

    /**
     * Finds the named connector for the module. This searches for inputs and outputs. This is similar to getConnector but it does not throw an
     * exception if the connector could not be found.
     *
     * \param name the name. This can be a canonical name or the connector name.
     *
     * \return the connector or NULL if not found.
     */
    boost::shared_ptr< WModuleConnector > findConnector( std::string name );

    /**
     * Return a pointer to the properties object of the module.
     *
     * \return the properties.
     */
    boost::shared_ptr< WProperties > getProperties() const;

    /**
     * Return a pointer to the information properties object of the module. The module intends these properties to not be modified.
     *
     * \return the properties.
     */
    boost::shared_ptr< WProperties > getInformationProperties() const;

    /**
     * Determines whether the module instance is properly initialized.
     *
     * \return true if properly initialized.
     */
    const WBoolFlag& isInitialized() const;

    /**
     * Checks whether the module instance is ready to be used. This is the case if isInitialized && isAssociated.
     *
     * \return isInitialized && isAssociated
     */
    const WBoolFlag&  isUseable() const;

     /**
      * Checks whether this module is associated with an container.
      *
      * \return true if associated.
      */
    const WBoolFlag&  isAssociated() const;

     /**
      * Checks whether this module is ready.
      *
      * \return true if ready.
      */
    const WBoolFlag&  isReady() const;

    /**
     * Checks whether this module has been crashed. This will be true whenever the module throws an unhandled exception.
     *
     * \return true if there has been an exception during moduleMain().
     */
    const WBoolFlag& isCrashed() const;

    /**
     * This is the logical or of isReady and isCrashed. You should use this condition if you need to wait for a module to get ready. If it
     * crashed before ready() got called, you most probably would wait endlessly.
     *
     * \return isReady || isCrashed.
     */
    const WBoolFlag& isReadyOrCrashed() const;

    /**
     * Returns a flag denoting whether the thread currently is running or nor. It is also useful to get a callback whenever a module stops.
     *
     * \return the flag
     */
    const WBoolFlag& isRunning() const;

     /**
      * The container this module is associated with.
      *
      * \return the container.
      */
    boost::shared_ptr< WModuleContainer > getAssociatedContainer() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const = 0;

    /**
     * Connects a specified notify function with a signal this module instance is offering.
     *
     * \exception WModuleSignalSubscriptionFailed thrown if the signal can't be connected.
     *
     * \param signal the signal to connect to.
     * \param notifier the notifier function to bind.
     *
     * \return connection descriptor.
     */
    virtual boost::signals2::connection subscribeSignal( MODULE_SIGNAL signal, t_ModuleGenericSignalHandlerType notifier );

    /**
     * Connects a specified notify function with a signal this module instance is offering.
     *
     * \exception WModuleSignalSubscriptionFailed thrown if the signal can't be connected.
     *
     * \param signal the signal to connect to.
     * \param notifier the notifier function to bind.
     *
     * \return connection descriptor.
     */
    virtual boost::signals2::connection subscribeSignal( MODULE_SIGNAL signal, t_ModuleErrorSignalHandlerType notifier );

    /**
     * Gets the modules base progress. This is actually a progress combiner, which bundles all progresses.
     *
     * \return the progress combiner for this module.
     */
    virtual boost::shared_ptr< WProgressCombiner > getRootProgressCombiner();

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Gets the type of the module. This is useful for FAST differentiation between several modules like standard modules and data
     * modules which play a special role in OpenWalnut/Kernel.
     *
     * \return the Type. If you do not overwrite this method, it will return MODULE_ARBITRARY.
     */
    virtual MODULE_TYPE getType() const;

    /**
     * Completely disconnects all connected connectors of this module. This is useful to isolate a module (for deletion, removal from a container
     * and so on.)
     */
    void disconnect();

    /**
     * Gives a list of all WDisconnectCombiners possible. Please note that while the list exists, connections might change.
     *
     * \return the list of possible disconnect operations
     */
    WCombinerTypes::WDisconnectList getPossibleDisconnections();

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain() = 0;

    /**
     * Thread entry point. Calls moduleMain and sends error notification if needed.
     */
    void threadMain();

     /**
      * Sets the container this module is associated with.
      *
      * \param container the container to associate with.
      */
    void setAssociatedContainer( boost::shared_ptr< WModuleContainer > container );

    // **************************************************************************************************************************
    //
    // Connector Management
    //
    // **************************************************************************************************************************

    /**
     * Initialize connectors in this function. This function must not be called multiple times for one module instance.
     * The module container manages calling those functions -> so just implement it.
     */
    virtual void connectors();

    /**
     * Initialize properties in this function. This function must not be called multiple times for one module instance.
     * The module container manages calling those functions -> so just implement it. Once initialized the number and type
     * of all properties should be set.
     */
    virtual void properties();

    /**
     * Manages connector initialization. Gets called by module container.
     *
     * \throw WModuleConnectorInitFailed if called multiple times.
     */
    void initialize();

    /**
     * Called whenever the module should shutdown.
     */
    virtual void cleanup();

    /**
     * Adds the specified connector to the list of inputs.
     *
     * \param con the connector.
     */
    void addConnector( boost::shared_ptr<WModuleInputConnector> con );

    /**
     * Adds the specified connector to the list of outputs.
     *
     * \param con the connector.
     */
    void addConnector( boost::shared_ptr<WModuleOutputConnector> con );

    /**
     * Removes all connectors properly. It disconnects the connectors and cleans the connectors list.
     */
    void removeConnectors();

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

    // **************************************************************************************************************************
    //
    // Signal handlers that HAVE to be in every module. By default they do nothing. You should overwrite them to get notified
    // with the corresponding signal
    //
    // **************************************************************************************************************************

    /**
     * Gives the signal handler function responsible for a given signal. Modules defining own signal handlers should overwrite
     * this function. This function is protected since boost::functions are callable, which is what is not wanted here. Just
     * signals should call them.
     *
     * \param signal the signal to get the handler for.
     *
     * \return the signal handler for "signal".
     */
    virtual const t_GenericSignalHandlerType getSignalHandler( MODULE_CONNECTOR_SIGNAL signal );

    /**
     * Gets called whenever a connector gets connected to the specified input.
     *
     * \param here the connector of THIS module that got connected to "there"
     * \param there the connector that has been connected with the connector "here" of this module.
     */
    virtual void notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> here,
                                              boost::shared_ptr<WModuleConnector> there );
    /**
     * Gets called whenever a connection between a remote and local connector gets closed.
     *
     * \param here the connector of THIS module getting disconnected.
     * \param there the connector of the other module getting disconnected.
     */
    virtual void notifyConnectionClosed( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there );

    /**
     * Gets called when the data on one input connector changed.
     *
     * \param input the input connector receiving the change.
     * \param output the output connector sending the change notification.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );

    /**
     * Call this whenever your module is ready and can react on property changes.
     */
    void ready();

    /**
     * Logger instance for comfortable info logging. Simply use logInfo() << "my info".
     *
     * \return the logger stream.
     */
    wlog::WStreamedLogger infoLog() const;

    /**
     * Logger instance for comfortable debug logging. Simply use logDebug() << "my debug".
     *
     * \return the logger stream.
     */
    wlog::WStreamedLogger debugLog() const;

    /**
     * Logger instance for comfortable warning- logs. Simply use logWarning() << "my warning".
     *
     * \return the logger stream.
     */
    wlog::WStreamedLogger warnLog() const;

    /**
     * Logger instance for comfortable error logging. Simply use logError() << "my error".
     *
     * \return the logger stream.
     */
    wlog::WStreamedLogger errorLog() const;

    // **************************************************************************************************************************
    //
    // Members
    //
    // **************************************************************************************************************************

    /**
     * The property object for the module.
     */
    boost::shared_ptr< WProperties > m_properties;

    /**
     * The property object for the module containing only module whose purpose is "PV_PURPOSE_INFORMNATION". It is useful to define some property
     * to only be of informational nature. The GUI does not modify them. As it is a WProperties instance, you can use it the same way as
     * m_properties.
     */
    boost::shared_ptr< WProperties > m_infoProperties;

    /**
     * Progress indicator used as parent for all progress' of this module.
     */
    boost::shared_ptr< WProgressCombiner > m_progress;

    /**
     * True if everything is initialized and ready to be used.
     */
    WBoolFlag m_initialized;

    /**
     * True if container got associated with this flag.
     */
    WBoolFlag m_isAssociated;

    /**
     * True if associated && initialized.
     */
    WBoolFlag m_isUsable;

    /**
     * True if ready() was called.
     */
    WBoolFlag m_isReady;

    /**
     * True whenever an exception is thrown during moduleMain.
     */
    WBoolFlag m_isCrashed;

    /**
     * It is true whenever m_isReady or m_isCrashed is true. This is mostly useful for functions which need to wait for a module to get ready.
     */
    WBoolFlag m_isReadyOrCrashed;

    /**
     * True if the module currently is running.
     */
    WBoolFlag m_isRunning;

    /**
     * Progress indicator for the "ready" state.
     */
    boost::shared_ptr< WProgress > m_readyProgress;

    /**
     * The internal state of the module. This is, by default, simply the exit flag from WThreadedRunner.
     */
    WConditionSet m_moduleState;

    /**
     * The container this module belongs to.
     */
    boost::shared_ptr< WModuleContainer > m_container;

    /**
     * Set of input connectors associated with this module.
     */
    InputConnectorList m_inputConnectors;

    /**
     * Set of output connectors associated with this module.
     */
    OutputConnectorList m_outputConnectors;

    /**
     * True whenever the module should be active
     */
    WPropBool m_active;

    /**
     * This property holds a user specified name for the current module instance.
     */
    WPropString m_runtimeName;

private:

     /**
     * Lock for m_inputConnectors.
     */
    // boost::shared_mutex m_inputConnectorsLock;

    /**
     * Lock for m_outputConnectors.
     */
    // boost::shared_mutex m_outputConnectorsLock;

    /**
     * Signal fired whenever a module main thread is ready.
     */
    t_ModuleGenericSignalType signal_ready;

    /**
     * Signal fired whenever a module main thread throws an exception/error.
     */
    t_ModuleErrorSignalType signal_error;
};

/**
 * The following macro is used by modules so the factory can aquire a prototype instance.
 */
#define W_SHARED_LIB_ENTRY( MODULECLASS ) \
extern "C" boost::shared_ptr< WModule > createModuleInstance() \
{\
	return boost::shared_ptr< WModule >( new MODULECLASS );\
}\

/**
 * \defgroup modules Modules
 *
 * \brief
 * This group contains modules of OpenWalnut.
 * The term modules can be understood as "plugin" or "algorithm" in this context.
 */
#endif  // WMODULE_H

