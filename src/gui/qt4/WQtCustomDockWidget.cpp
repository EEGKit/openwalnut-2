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

#include "WQtCustomDockWidget.h"
#include <QtGui/QCloseEvent>

WQtCustomDockWidget::WQtCustomDockWidget( std::string title, QWidget* parent )
    : QDockWidget( QString::fromStdString( title ), parent )
{
    // setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    m_glWidget = boost::shared_ptr< WQtGLWidget >( new WQtGLWidget( title, this, WGECamera::PERSPECTIVE ) );
    m_glWidget->initialize();

    setWidget( m_glWidget.get() );
}

void WQtCustomDockWidget::closeEvent( QCloseEvent* event )
{
    // forward events
    m_glWidget->close();

    event->accept();
}
