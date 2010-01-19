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

#include "WGE2DManipulator.h"


WGE2DManipulator::WGE2DManipulator()
    : m_positionX( 0.0 ),
      m_positionY( 0.0 )
{
}

const char* WGE2DManipulator::className() const
{
    return "WGE2DManipulator";
}

void WGE2DManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    m_positionX = matrix.getTrans().x();
    m_positionY = matrix.getTrans().y();
}

void WGE2DManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
    m_positionX = -matrix.getTrans().x();
    m_positionY = -matrix.getTrans().y();
}

osg::Matrixd WGE2DManipulator::getMatrix() const
{
    return osg::Matrixd::translate( m_positionX, m_positionY, 0.0 );
}

osg::Matrixd WGE2DManipulator::getInverseMatrix() const
{
    return osg::Matrixd::translate( -m_positionX, -m_positionY, 0.0 );
}

void WGE2DManipulator::home( const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& us ) // NOLINT We can not change the interface of OSG
{
    m_positionX = 0.0;
    m_positionY = 0.0;

    us.requestRedraw();
    flushMouseEventStack();
}

void WGE2DManipulator::init( const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& us ) // NOLINT We can not change the interface of OSG
{
    flushMouseEventStack();

    us.requestContinuousUpdate( false );
}

bool WGE2DManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    bool handled;
    switch( ea.getEventType() )
    {
        case( osgGA::GUIEventAdapter::PUSH ):
        {
            addMouseEvent( ea );
            handled = true;
            break;
        }
        case( osgGA::GUIEventAdapter::DRAG ):
        {
            addMouseEvent( ea );
            if( calcMovement() )
            {
                us.requestRedraw();
            }
            handled = true;
            break;
        }
        case( osgGA::GUIEventAdapter::KEYDOWN ):
        {
            if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Space )
            {
                home( ea, us );
                handled = true;
            }
            else
            {
                handled = false;
            }
            break;
        }
        default:
            handled = false;
    }

    return handled;
}

void WGE2DManipulator::getUsage( osg::ApplicationUsage& usage ) const // NOLINT We can not change the interface of OSG
{
    usage.addKeyboardMouseBinding( "Space", "Reset the view to home" );
}

WGE2DManipulator::~WGE2DManipulator()
{
}

void WGE2DManipulator::flushMouseEventStack()
{
    m_ga_t1 = NULL;
    m_ga_t0 = NULL;
}

void WGE2DManipulator::addMouseEvent( const osgGA::GUIEventAdapter& ea )
{
    m_ga_t1 = m_ga_t0;
    m_ga_t0 = &ea;
}

bool WGE2DManipulator::calcMovement()
{
    bool changed = false;
    // return if less then two events have been added.
    if( m_ga_t0.valid() && m_ga_t1.valid() )
    {
        unsigned int buttonMask = m_ga_t0->getButtonMask();
        if( buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON
            || buttonMask == ( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON ) )
        {
            m_positionX += m_ga_t1->getX() - m_ga_t0->getX();
            m_positionY += m_ga_t1->getY() - m_ga_t0->getY();
            changed = true;
        }
    }

    return changed;
}
