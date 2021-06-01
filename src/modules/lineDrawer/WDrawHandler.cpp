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

#include "WDrawHandler.h"

static const unsigned int SHIFT_DOWN = 65505;
static const unsigned int SHIFT_UP = 16777248;

WDrawHandler::WDrawHandler( WMLineDrawer* drawer ):
    m_drawer( drawer ),
    m_mousePressed( false ),
    m_shiftPressed( false )
{
}

bool WDrawHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN )
    {
        if( ea.getKey() == SHIFT_DOWN )
        {
            m_shiftPressed = true;
        }
    }
    if( ea.getEventType() == osgGA::GUIEventAdapter::KEYUP )
    {
        if( ea.getKey() == SHIFT_UP )
        {
            m_shiftPressed = false;
        }
    }

    if( !m_shiftPressed )
    {
        return false;
    }

    if( ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON  )
    {
        if( ea.getEventType() == osgGA::GUIEventAdapter::PUSH )
        {
            m_mousePressed = true;
            m_drawer->startNewLine();
        }
        else if( ea.getEventType() == osgGA::GUIEventAdapter::RELEASE )
        {
            m_mousePressed = false;
        }
    }

    if( m_mousePressed && ea.getEventType() == osgGA::GUIEventAdapter::DRAG )
    {
        m_drawer->addPoint( ( ea.getXnormalized() + 1.0 ) / 2.0, ( ea.getYnormalized() + 1.0 ) / 2.0 );
        m_drawer->updateLines();
        return true;
    }

    return false;
}
