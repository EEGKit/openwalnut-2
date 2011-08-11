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

#ifndef WMODULEFACTORY_H
#define WMODULEFACTORY_H

#include <set>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "modules/data/WMData.h" // this is the ONLY module with a special meaning. Every one knowing the factory also knows this
#include "../common/WSharedAssociativeContainer.h"
#include "WModuleCombinerTypes.h"
#include "WModule.h"
#include "WModuleLoader.h"

#include "../modules/fiberDisplay/WMFiberDisplay.h"

#include "WExportKernel.h"

/**
 * Class able to create a new copy of an arbitrary module. It uses the Factory and Prototype design pattern.
 */
class OWKERNEL_EXPORT WModuleFactory // NOLINT
{
friend class WModuleFactoryTest;
public:

    /**
     * For shortening: a type defining a shared set of WModule pointers.
     */
    typedef std::set< boost::shared_ptr< WModule > > PrototypeContainerType;

    /**
     * Const iterator for the prototype set.
     */
    typedef std::set< boost::shared_ptr< WModule > >::const_iterator PrototypeContainerConstIteratorType;

    /**
     * Iterator for the prototype set.
     */
    typedef std::set< boost::shared_ptr< WModule > >::iterator PrototypeContainerIteratorType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedAssociativeContainer< PrototypeContainerType > PrototypeSharedContainerType;

    /**
     * Destructor.
     */
    virtual ~WModuleFactory();

    /**
     * Loads the modules and creates prototypes.
     */
    void load();

    /**
     * Create a new and initialized module using the specified prototype.
     *
     * \param prototype the prototype to clone.
     *
     * \return the module created using the prototype.
     */
    boost::shared_ptr< WModule > create( boost::shared_ptr< WModule > prototype );

    /**
     * Returns instance of the module factory to use to create modules.
     *
     * \return the running module factory.
     */
    static boost::shared_ptr< WModuleFactory > getModuleFactory();

    /**
     * Searches a prototype by name. It returns the prototype, or a NULL pointer if it is not found. The difference to
     * getPrototypeByName() is, that an unavailable prototype does not throw an exception. This is nice for checking whether a
     * prototype exists or not.
     *
     * \param name name of the prototype to search
     *
     * \return the prototype if it exists, or NULL if not.
     */
    const boost::shared_ptr< WModule > isPrototypeAvailable( std::string name );

    /**
     * Finds a prototype using the specified name.
     *
     * \param name the name.
     *
     * \return the prototype whose name is equal to the specified one.
     */
    const boost::shared_ptr< WModule > getPrototypeByName( std::string name );

    /**
     * Finds a prototype using an instance of a module. This uses the type_info to find a proper prototype.
     *
     * \param instance the instance to use.
     *
     * \return the prototype.
     * \throw WPrototypeUnknown if prototype can not be found.
     */
    const boost::shared_ptr< WModule > getPrototypeByInstance( boost::shared_ptr< WModule > instance );

    /**
     * This method gives read access to the list of all prototypes.
     *
     * \return the read ticket for the prototype list
     */
    PrototypeSharedContainerType::ReadTicket getPrototypes() const;

    /**
     * Checks whether the first instance can be casted to the second one.
     *
     * \param module the module to check.
     *
     * \return true if the dynamic_cast is successful
     */
    template <typename T>
    static bool isA( boost::shared_ptr< WModule > module );

    /**
     * Returns a set of module combiners with module combinations compatible with the specified one.
     *
     * \param module the module to find the compatibles for.
     *
     * \note as the default parameter denotes, providing a NULL pointer (or calling the method without a parameter) returns the list of modules
     * which are compatible to every other module. In other words, it returns all modules without input connectors.
     *
     * \return set of compatible combiners.
     */
    WCombinerTypes::WCompatiblesList getCompatiblePrototypes(
            boost::shared_ptr< WModule > module = boost::shared_ptr< WModule >()
    );

    /**
     * This method uses a newly created instance of WModule and initializes it properly. After using this method, the module is
     * properly initialized and ready to be used.
     *
     * \param module the module to initialize.
     */
    static void initializeModule( boost::shared_ptr< WModule > module );

    /**
     * Checks whether the specified module is a prototype or an instantiated module.
     *
     * \param module the module to check
     *
     * \return true if it is a prototype
     */
    static bool isPrototype( boost::shared_ptr< WModule > module );

protected:
    /**
     * Constructors are protected because this is a Singleton.
     */
    WModuleFactory();

    /**
     * The module prototypes available.
     */
    PrototypeSharedContainerType m_prototypes;

    /**
     * Checks whether the specified module is a prototype or an instantiated module. Use isPrototype if no ticket acquired yet.
     *
     * \param module the module to check
     * \param ticket ticket which already has read lock.
     *
     * \return true if it is a prototype
     */
    bool checkPrototype( boost::shared_ptr< WModule > module, PrototypeSharedContainerType::ReadTicket ticket );

private:

    /**
     * Loader class managing dynamically loaded modules in open walnut.
     */
    WModuleLoader m_moduleLoader;

    /**
     * Singleton instance of WModuleFactory.
     */
    static boost::shared_ptr< WModuleFactory > m_instance;
};

template <typename T>
bool WModuleFactory::isA( boost::shared_ptr< WModule > module )
{
    // NOTE: this is RTTI. Everybody says: do not use it. We ignore them.
    return ( dynamic_cast< T* >( module.get() ) );  // NOLINT
}

#endif  // WMODULEFACTORY_H
