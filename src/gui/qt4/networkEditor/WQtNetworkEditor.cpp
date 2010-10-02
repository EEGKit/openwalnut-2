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
#include <iostream>

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsItemGroup>

#include "../WMainWindow.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkPort.h"

#include "../../../kernel/WKernel.h"
#include "../../../kernel/WModule.h"
#include "../../../kernel/WModuleFactory.h"
#include "../events/WEventTypes.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WModuleConnectEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WModuleDisconnectEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WModuleRemovedEvent.h"
#include "../events/WRoiAssocEvent.h"
#include "../events/WRoiRemoveEvent.h"


WQtNetworkEditor::WQtNetworkEditor( WMainWindow* parent )
    : QDockWidget( "NetworkEditor", parent )
{
    m_mainWindow = parent;

    m_panel = new QWidget( this );

    m_view = new QGraphicsView();
    m_view->setDragMode( QGraphicsView::RubberBandDrag );
    m_view->setRenderHint( QPainter::Antialiasing );

    m_scene = new WQtNetworkScene();
    m_scene->setSceneRect( -100.0, -100.0, 200.0, 200.0 );
    m_scene->setSceneRect( m_scene->itemsBoundingRect() );

    m_view->setScene( m_scene );

    m_layout = new QVBoxLayout;
    m_layout->addWidget( m_view );

    m_panel->setLayout( m_layout );

    this->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    this->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    setMinimumSize( 160, 240 );
    setWidget( m_panel );
}

WQtNetworkEditor::~WQtNetworkEditor()
{
}

void WQtNetworkEditor::addModule( WModule *module )
{
    WQtNetworkItem *netItem = new WQtNetworkItem( module );

    m_items.push_back( netItem );

    m_scene->addItem( netItem );
}
    

bool WQtNetworkEditor::event( QEvent* event )
{    
    // a module got associated with the root container -> add it to the list
    if ( event->type() == WQT_ASSOC_EVENT )
    {
        // convert event to assoc event
        WModuleAssocEvent* e1 = dynamic_cast< WModuleAssocEvent* >( event );     // NOLINT
        if ( e1 )
        {
            WLogger::getLogger()->addLogMessage( "Inserting module " + e1->getModule()->getName() +
                                                " to network editor.",
                                                 "NetworkEditor", LL_DEBUG );
            addModule( ( e1->getModule() ).get() );
        }
        //TODO: disablen des moduls solange nicht rdy!
        return true;
    }
 
    // a module changed its state to "ready" -> activate it in dataset browser
    if ( event->type() == WQT_READY_EVENT )
    {
        // convert event to assoc event
        WModuleReadyEvent* e = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_READY_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModueReadyEvent although its type claims it. Ignoring event.",
                                                 "NetworkEditor", LL_WARNING );

            return true;
        }

        WLogger::getLogger()->addLogMessage( "Activating module " + e->getModule()->getName() + " in dataset browser.",
                                             "NetworkEditor", LL_DEBUG );

        // search all the item matching the module
        WQtNetworkItem *item = findItemByModule( e->getModule().get() );
        item->activate( true );

        return true;
    }
 
    // a module tree item was connected to another one
    if ( event->type() == WQT_MODULE_CONNECT_EVENT )
    {
        WModuleConnectEvent* e = dynamic_cast< WModuleConnectEvent* >( event );     // NOLINT
        if ( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_CONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleConnectEvent although its type claims it. Ignoring event.",
                                                 "DatasetBrowser", LL_WARNING );
            return true;
        }

        // get the module of the input involved in this connection
        boost::shared_ptr< WModule > mIn = e->getInput()->getModule();
        boost::shared_ptr< WModule > mOut = e->getOutput()->getModule();

        WQtNetworkArrow *arrow = new WQtNetworkArrow();
        arrow->addConnection( e->getInput(), e->getOutput() );
        m_scene->addItem( arrow );

    }
    return QDockWidget::event( event );
}

WQtNetworkItem* WQtNetworkEditor::findItemByModule( WModule *module )
{
    WQtNetworkItem *item;

    std::cout << "findItem" << std::endl;

    for ( std::list< WQtNetworkItem* >::const_iterator iter = m_items.begin(); iter != m_items.end(); ++iter )
    {
       WQtNetworkItem *itemModule = dynamic_cast< WQtNetworkItem* >( *iter );
    
       std::cout << "search" << std::endl;

       if( module == itemModule->getModule() )
       {
           item = itemModule;
            
           std::cout << "found" << std::endl;
       }
    }
    return item;
}
