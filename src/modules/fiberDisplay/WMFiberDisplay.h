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

#ifndef WMFIBERDISPLAY_H
#define WMFIBERDISPLAY_H

#include <string>

#include <osg/Geode>

#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

#include "../../math/WPosition.h"

/**
 * Test module for drawing fibers
 */
class WMFiberDisplay : public WModule, public osg::Referenced
{
public:
    /**
     * Constructs new FiberTestModule
     */
    WMFiberDisplay();

    /**
     * Destructs this FiberTestModule
     */
    virtual ~WMFiberDisplay();

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
     * Due to the prototype design pattern used to build modules, this method
     * returns a new instance of this method. NOTE: it should never be
     * initialized or modified in some other way. A simple new instance is
     * required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Generates an OSG geometry for the given fiber dataset.
     *
     * \return OSG geometry representing the fiber.
     */
    osg::ref_ptr< osg::Geode > genFiberGeode();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Redraws the scene.
     *
     */
    void update();

    /**
     * initial creation of the osg geometry
     */
    void create();

    /**
     * Deletes the primitive list and adds only the avtivated
     */
    void updateLinesShown();

    /**
     * Switches the osg to use the appropriate color array
     */
    void updateColoring();

private:
    /**
     * Input connector for a fiber dataset.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_fiberInput;

    /**
     * Pointer to the fiber data set
     */
    boost::shared_ptr< const WDataSetFibers > m_dataset;

    /**
     * OSG node for this module. All other OSG nodes of this module should be
     * placed as child to this node.
     */
    osg::ref_ptr< osg::Group > m_osgNode;

    /**
     * OSG geometry
     * this pointer is stored for reuse when updating fibers to display
     */
    osg::ref_ptr< osg::Geometry > m_geometry;

    /**
     * Array that stores the global (direction) color for each fiber
     */
    osg::ref_ptr< osg::Vec4Array > m_globalColors;

    /**
     * Array that stores the local (direction) color for each fiber
     */
    osg::ref_ptr< osg::Vec4Array > m_localColors;

    /**
     * lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    /**
     * calculates a color from the vector between two points in space
     *
     * \param pos1
     * \param pos2
     */
    WColor getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 );


    /**
     * Node callback to handle updates properly
     */
    class fdNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< WMFiberDisplay > module = static_cast< WMFiberDisplay* > ( node->getUserData() );

            if ( module )
            {
                module->update();
            }
            traverse( node, nv );
        }
    };
};

inline const std::string WMFiberDisplay::getName() const
{
    return std::string( "Fiber Display" );
}

inline const std::string WMFiberDisplay::getDescription() const
{
    return std::string( "Draws fibers out of a WDataSetFibers" );
}

#endif  // WMFIBERDISPLAY_H
