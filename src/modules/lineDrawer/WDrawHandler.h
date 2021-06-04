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

#ifndef WDRAWHANDLER_H
#define WDRAWHANDLER_H

#include <osg/Viewport>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

#include "WMLineDrawer.h"

/**
 * Forward declaration of the LineDrawer.
 */
class WMLineDrawer;

/**
 * Handles the GUIEvents in context of the LineDrawer.
 */
class WDrawHandler : public osgGA::GUIEventHandler
{
public:
    /**
     * Construct a new WDrawHandler object.
     * 
     * \param drawer The LineDrawer this belongs to.
     */
    explicit WDrawHandler( WMLineDrawer* drawer );

    /**
     * Handles the incoming events.
     * 
     * \param ea        The EventAdapter of the event.
     * \param aa        The ActionAdapter of the event.
     * \return true     Don't propagate the event.
     * \return false    Propagate the event.
     */
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

private:
    WMLineDrawer* m_drawer; //!< The LineDrawer this belongs to.

    bool m_mousePressed;    //!< Whether the mouse is pressed or not.
    bool m_shiftPressed;    //!< Whether shift is pressed or not.
};

#endif  // WDRAWHANDLER_H
