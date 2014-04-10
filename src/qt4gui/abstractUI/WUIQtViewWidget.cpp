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

#include <string>

#include "core/common/WConditionOneShot.h"
#include "core/common/WFlag.h"

#include "WUIQtViewWidget.h"

WUIQtViewWidget::WUIQtViewWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            WMainWindow* mainWindow,
            WUIQtWidgetBase::SPtr parent ):
    WUIViewWidget( title ),
    WUIQtWidgetBase( mainWindow, parent ),
    m_projectionMode( projectionMode ),
    m_widgetDock( NULL )
{
    // initialize members
}

WUIQtViewWidget::~WUIQtViewWidget()
{
    // cleanup
}

QString WUIQtViewWidget::getTitleQString() const
{
    return QString::fromStdString( getTitle() );
}

osg::ref_ptr< WGEGroupNode > WUIQtViewWidget::getScene() const
{
    return m_scene;
}

boost::shared_ptr< WGEViewer > WUIQtViewWidget::getViewer() const
{
    // should be safe to call from outside the GUI thread !?
    return m_widgetDock->getGLWidget()->getViewer();
}

size_t WUIQtViewWidget::height() const
{
    if( m_widgetDock->getGLWidget()->getViewer() )
    {
        // should be safe to call from outside the GUI thread !?
        return static_cast< size_t >( m_widgetDock->getGLWidget()->getViewer()->getCamera()->getViewport()->height() );
    }
    else
    {
        return 0;
    }
}

size_t WUIQtViewWidget::width() const
{
    if( m_widgetDock->getGLWidget()->getViewer() )
    {
        // should be safe to call from outside the GUI thread !?
        return static_cast< size_t >( m_widgetDock->getGLWidget()->getViewer()->getCamera()->getViewport()->width() );
    }
    else
    {
        return 0;
    }
}

void WUIQtViewWidget::addEventHandler( osgGA::GUIEventHandler* handler )
{
    if( m_widgetDock->getGLWidget()->getViewer() )
    {
        // should be safe to call from outside the GUI thread !?
        m_widgetDock->getGLWidget()->getViewer()->getView()->addEventHandler( handler );
    }
}

void WUIQtViewWidget::show()
{
    WUIQtWidgetBase::show();
}

void WUIQtViewWidget::setVisible( bool visible )
{
    WUIQtWidgetBase::setVisible( visible );
}

bool WUIQtViewWidget::isVisible() const
{
    return WUIQtWidgetBase::isVisible();
}

void WUIQtViewWidget::cleanUpGT()
{
    // nothing to do
}

void WUIQtViewWidget::close()
{
    // use WUIViewWidget to handle this
    WUIViewWidget::close();
}

void WUIQtViewWidget::closeImpl()
{
    WUIQtWidgetBase::closeImpl();
}

void WUIQtViewWidget::realizeImpl()
{
    // this is called from withing the GUI thread -> we can safely create QT widgets here
    m_widgetDock = new WQtGLDockWidget( QString::fromStdString( getTitle() ),
                                        QString::fromStdString( getTitle() ), getCompellingQParent(), m_projectionMode );
    m_widgetDock->setObjectName( QString( "Custom Dock Window " ) + QString::fromStdString( getTitle() ) );

    // define some scene
    m_scene = new WGEGroupNode();
    m_scene->setDataVariance( osg::Object::DYNAMIC );
    m_widgetDock->getGLWidget()->getViewer()->setScene( m_scene );

    // remove the close button
    m_widgetDock->disableCloseButton();

    embedContent( m_widgetDock );
}

