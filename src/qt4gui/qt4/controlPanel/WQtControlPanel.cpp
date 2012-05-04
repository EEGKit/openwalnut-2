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

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <QtCore/QList>
#include <QtGui/QMenu>
#include <QtGui/QScrollArea>
#include <QtGui/QShortcut>
#include <QtGui/QSplitter>
#include <QtGui/QMessageBox>

#include "core/common/WLogger.h"
#include "core/common/WPredicateHelper.h"
#include "core/dataHandler/WDataSet.h"
#include "core/kernel/WDataModule.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModuleFactory.h"
#include "core/kernel/WROIManager.h"
#include "../WMainWindow.h"
#include "../WQt4Gui.h"
#include "../WQtCombinerActionList.h"
#include "../WQtModuleConfig.h"
#include "../events/WEventTypes.h"
#include "../events/WModuleAssocEvent.h"
#include "../events/WModuleConnectEvent.h"
#include "../events/WModuleConnectorEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WModuleDisconnectEvent.h"
#include "../events/WModuleReadyEvent.h"
#include "../events/WModuleRemovedEvent.h"
#include "../events/WRoiAssocEvent.h"
#include "../events/WRoiRemoveEvent.h"
#include "../guiElements/WQtModuleMetaInfo.h"
#include "../networkEditor/WQtNetworkEditor.h"
#include "WQtBranchTreeItem.h"
#include "WQtColormapper.h"

#include "WQtControlPanel.h"
#include "WQtControlPanel.moc"

WQtControlPanel::WQtControlPanel( WMainWindow* parent )
    : QDockWidget( "Control Panel", parent ),
    m_ignoreSelectionChange( false ),
    m_activeModule( WModule::SPtr() )
{
    setObjectName( "Control Panel Dock" );

    m_mainWindow = parent;
    setMinimumWidth( 200 );

    m_moduleTreeWidget = new WQtTreeWidget();
    m_moduleTreeWidget->setContextMenuPolicy( Qt::ActionsContextMenu );

    m_moduleTreeWidget->setHeaderLabel( QString( "Module Tree" ) );
    m_moduleTreeWidget->setDragEnabled( false );
    m_moduleTreeWidget->viewport()->setAcceptDrops( true );
    m_moduleTreeWidget->setDropIndicatorShown( true );
    m_moduleTreeWidget->setMinimumHeight( 100 );

    // create context menu for tree items

    // a separator to clean up the tree widget's context menu
    QAction* separator = new QAction( m_moduleTreeWidget );
    separator->setSeparator( true );
    m_moduleTreeWidget->addAction( separator );

    m_addModuleAction = new QAction( "Add Module", m_moduleTreeWidget );
    m_moduleTreeWidget->addAction( m_addModuleAction );
    m_connectWithPrototypeAction = new QAction( "Add Module and Connect", m_moduleTreeWidget );
    m_moduleTreeWidget->addAction( m_connectWithPrototypeAction );
    m_connectWithModuleAction = new QAction( "Connect Existing Module", m_moduleTreeWidget );
    m_moduleTreeWidget->addAction( m_connectWithModuleAction );
    m_disconnectAction = new QAction( "Disconnect", m_moduleTreeWidget );
    m_moduleTreeWidget->addAction( m_disconnectAction );

    // a separator to clean up the tree widget's context menu
    m_moduleTreeWidget->addAction( separator );

    m_deleteModuleAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "remove" ), "Remove Module", m_moduleTreeWidget );
    {
        // Set the key for removing modules
        //m_deleteModuleAction->setShortcutContext( Qt::WidgetShortcut );
        m_deleteModuleAction->setShortcutContext( Qt::WidgetWithChildrenShortcut );
        m_deleteModuleAction->setShortcut( QKeySequence::Delete );
        m_deleteModuleAction->setIconVisibleInMenu( true );
    }
    connect( m_deleteModuleAction, SIGNAL( triggered() ), this, SLOT( deleteModule() ) );
    m_moduleTreeWidget->addAction( m_deleteModuleAction );

    // a separator to clean up the tree widget's context menu
    m_moduleTreeWidget->addAction( separator );

    // add an entry for those who search their module without luck
    m_missingModuleAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "missingModule" ), "Missing Module?",
                                          m_moduleTreeWidget );
    m_missingModuleAction->setToolTip( "Having trouble finding your module? This opens the module configuration, which allows you to define the "
                                       "modules that should be shown or hidden." );
    m_missingModuleAction->setIconVisibleInMenu( true );
    m_moduleTreeWidget->addAction( m_missingModuleAction );

    // the network editor also needs the context menu
    if( m_mainWindow->getNetworkEditor() )
    {
        m_mainWindow->getNetworkEditor()->setContextMenuPolicy( Qt::ActionsContextMenu );
        m_mainWindow->getNetworkEditor()->addAction( m_addModuleAction );
        m_mainWindow->getNetworkEditor()->addAction( separator );
        m_mainWindow->getNetworkEditor()->addAction( m_connectWithPrototypeAction );
        m_mainWindow->getNetworkEditor()->addAction( m_connectWithModuleAction );
        m_mainWindow->getNetworkEditor()->addAction( m_disconnectAction );
        m_mainWindow->getNetworkEditor()->addAction( separator );
        m_mainWindow->getNetworkEditor()->addAction( m_deleteModuleAction );
        m_mainWindow->getNetworkEditor()->addAction( separator );
        m_mainWindow->getNetworkEditor()->addAction( m_missingModuleAction );
    }

    m_colormapper = new WQtColormapper( m_mainWindow );
    m_colormapper->setToolTip( "Reorder the textures." );

    m_tabWidget = new QTabWidget(  );
    m_tabWidget->setMinimumHeight( 100 );

    m_moduleDock = new QDockWidget( "Module Tree", m_mainWindow );
    m_moduleDock->setObjectName( "Module Dock" );
    m_moduleDock->setWidget( m_moduleTreeWidget );

    m_roiDock = new QDockWidget( "ROIs", m_mainWindow );
    m_roiDock->setObjectName( "ROI Dock" );
    m_roiTreeWidget = new WQtTreeWidget();
    m_roiTreeWidget->setToolTip( "Regions of intrest (ROIs) for selecting fiber  clusters. Branches are combined using logic <b>or</b>, "
                                 "inside the branches the ROIs are combined using logic <b>and</b>." );
    m_roiTreeWidget->setHeaderLabel( QString( "ROIs" ) );
    m_roiTreeWidget->setHeaderHidden( true );
    m_roiTreeWidget->setDragEnabled( true );
    m_roiTreeWidget->viewport()->setAcceptDrops( true );
    m_roiTreeWidget->setDropIndicatorShown( true );
    m_roiTreeWidget->setDragDropMode( QAbstractItemView::InternalMove );
    m_roiDock->setWidget( m_roiTreeWidget );

    m_moduleExcluder = new WQtModuleConfig( parent );
    connect( m_missingModuleAction, SIGNAL( triggered( bool ) ), m_moduleExcluder, SLOT( configure() ) );

    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->setWidget( m_tabWidget );

    m_tiModules = new WQtModuleHeaderTreeItem( m_moduleTreeWidget );
    m_tiModules->setText( 0, QString( "Subject-independent Modules" ) );
    m_tiModules->setToolTip( 0, "Subject-independent modules and modules for which no parent module could be detected." );
    m_tiRois = new WQtRoiHeaderTreeItem( m_roiTreeWidget );
    m_tiRois->setText( 0, QString( "ROIs" ) );

    connectSlots();

    // similar to the module delete action: a ROI delete action
    m_deleteRoiAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "remove" ), "Remove ROI", m_roiTreeWidget );
    {
        // Set the key for removing modules
        m_deleteRoiAction->setShortcutContext( Qt::WidgetShortcut );
        m_deleteRoiAction->setShortcut( QKeySequence::Delete );
        m_deleteRoiAction->setIconVisibleInMenu( true );
    }
    connect( m_deleteRoiAction, SIGNAL( triggered() ), this, SLOT( deleteROITreeItem() ) );
    m_roiTreeWidget->addAction( m_deleteModuleAction );
    m_roiTreeWidget->addAction( m_deleteRoiAction );
}

WQtControlPanel::~WQtControlPanel()
{
}

void WQtControlPanel::connectSlots()
{
    // if the user changes some white/blacklist setting: update.
    connect( m_moduleExcluder, SIGNAL( updated() ), this, SLOT( reselectTreeItem() ) );
    connect( m_moduleTreeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( selectTreeItem() ) );
    connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( changeTreeItem( QTreeWidgetItem*, int ) ) );
    connect( m_moduleTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ),  m_roiTreeWidget, SLOT( clearSelection() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), this, SLOT( selectRoiTreeItem() ) );
    connect( m_roiTreeWidget, SIGNAL( itemClicked( QTreeWidgetItem*, int ) ), m_moduleTreeWidget, SLOT( clearSelection() ) );
    connect( m_colormapper, SIGNAL( textureSelectionChanged( osg::ref_ptr< WGETexture3D > ) ),
             this, SLOT( selectDataModule( osg::ref_ptr< WGETexture3D > ) ) );
    connect( m_roiTreeWidget, SIGNAL( dragDrop() ), this, SLOT( handleDragDrop() ) );
}

WQtSubjectTreeItem* WQtControlPanel::addSubject( std::string name )
{
    WQtSubjectTreeItem* subject = new WQtSubjectTreeItem( m_moduleTreeWidget );
    subject->setText( 0, QString::fromStdString( name ) );
    subject->setToolTip( 0, QString::fromStdString( "All data and modules that are children of this tree item belong to the subject \"" +
                name + "\"." ) );

    return subject;
}

bool WQtControlPanel::event( QEvent* event )
{
    if( event->type() == WQT_ROI_ASSOC_EVENT )
    {
        WRoiAssocEvent* e2 = dynamic_cast< WRoiAssocEvent* >( event );     // NOLINT
        if( e2 )
        {
            addRoi( e2->getRoi() );
            WLogger::getLogger()->addLogMessage( "Inserting ROI to control panel.", "ControlPanel", LL_DEBUG );
        }

        return true;
    }
    if( event->type() == WQT_ROI_REMOVE_EVENT )
    {
        WRoiRemoveEvent* e3 = dynamic_cast< WRoiRemoveEvent* >( event );
        if( e3 )
        {
            removeRoi( e3->getRoi() );
            WLogger::getLogger()->addLogMessage( "Removing ROI from control panel.", "ControlPanel", LL_DEBUG );
        }

        return true;
    }

    // a module got associated with the root container -> add it to the list
    if( event->type() == WQT_ASSOC_EVENT )
    {
        // convert event to assoc event
        WModuleAssocEvent* e1 = dynamic_cast< WModuleAssocEvent* >( event );     // NOLINT
        if( e1 )
        {
            WLogger::getLogger()->addLogMessage( "Inserting module " + e1->getModule()->getName() + " to control panel.",
                                                 "ControlPanel", LL_DEBUG );

            // show deprecation message?
            if( e1->getModule()->isDeprecated() )
            {
                std::string d = e1->getModule()->getDeprecationMessage();
                std::string m = "The module \"" + e1->getModule()->getName() + "\" is marked deprecated. You should avoid using it."
                                "<br><br>"
                                "Message: " + d;
                QMessageBox::warning( this, "Deprecation Warning", QString::fromStdString( m ) );
            }

            // finally add the module
            // TODO(schurade): is this differentiation between data and "normal" modules really needed?
            if( boost::shared_dynamic_cast< WDataModule >( e1->getModule() ).get() )
            {
                addDataset( e1->getModule(), 0 );
            }
            else
            {
                addModule( e1->getModule() );
            }
        }
        return true;
    }

    // a module changed its state to "ready" -> activate it in control panel
    if( event->type() == WQT_READY_EVENT )
    {
        // convert event to assoc event
        WModuleReadyEvent* e = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if( !e )
        {
            // this should never happen, since the type is set to WQT_READY_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModueReadyEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );

            return true;
        }

        std::list< WQtTreeItem* > items = findItemsByModule( e->getModule() );
        for( std::list< WQtTreeItem* >::const_iterator it = items.begin(); it != items.end(); ++it )
        {
            ( *it )->setDisabled( false );
        }

        setActiveModule( e->getModule() );

        return true;
    }

    // a module tree item was connected to another one
    if( event->type() == WQT_MODULE_CONNECT_EVENT )
    {
        WModuleConnectEvent* e = dynamic_cast< WModuleConnectEvent* >( event );     // NOLINT
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_CONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleConnectEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // get the module of the input involved in this connection
        boost::shared_ptr< WModule > mIn = e->getInput()->getModule();
        boost::shared_ptr< WModule > mOut = e->getOutput()->getModule();

        // NOTE: the following is ugly. We definitely should rethink our GUI

        // at this moment items for each input connector are inside the tree.
        // search the items not yet associated with some other module for the first item
        std::list< WQtTreeItem* > items = findItemsByModule( mIn, m_tiModules );
        for( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            ( *iter )->setHidden( false );
            ( *iter )->setHandledInput( e->getInput()->getName() );
            ( *iter )->setHandledOutput( e->getOutput()->getName() );

            // move it to the module with the involved output
            std::list< WQtTreeItem* > possibleParents = findItemsByModule( mOut );
            for( std::list< WQtTreeItem* >::const_iterator parIter = possibleParents.begin(); parIter != possibleParents.end(); ++parIter )
            {
                // remove child from tiModules
                m_tiModules->removeChild( *iter );
                if( !( *parIter )->isHidden() )
                {
                    ( *parIter )->addChild( *iter );
                    ( *parIter )->setExpanded( true );
                    break;
                }
            }

            // job done.
            break;
        }
    }

    // a module tree item was disconnected from another one
    if( event->type() == WQT_MODULE_DISCONNECT_EVENT )
    {
        WModuleDisconnectEvent* e = dynamic_cast< WModuleDisconnectEvent* >( event );     // NOLINT
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_DISCONNECT_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDisconnectEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // get the module of the input involved in this connection
        boost::shared_ptr< WModule > mIn = e->getInput()->getModule();
        boost::shared_ptr< WModule > mOut = e->getOutput()->getModule();

        // NOTE: the following is ugly. We definitely should rethink our GUI

        // at this moment items for each input connector are inside the tree.
        // search all items an find those containing a children which belongs to the connection input
        std::list< WQtTreeItem* > items = findItemsByModule( mOut );
        for( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            // each of them can contain a child with the involved input
            std::list< WQtTreeItem* > childs = findItemsByModule( mIn, *iter );
            for( std::list< WQtTreeItem* >::const_iterator citer = childs.begin(); citer != childs.end(); ++citer )
            {
                if( ( *citer )->getHandledInput() == e->getInput()->getName() )
                {
                    ( *iter )->removeChild( *citer );

                    // move it back to the reservoir in m_tiModules
                    m_tiModules->addChild( *citer );
                    ( *citer )->setHidden( true );
                    ( *citer )->setHandledInput( "" );
                    ( *citer )->setHandledOutput( "" );
                }
            }
        }

        // we need to ensure that at least one item for mIn is visible somewhere
        items = findItemsByModule( mIn );
        bool oneVisible = false;
        for( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            oneVisible = oneVisible || !( *iter )->isHidden();
        }
        if( !oneVisible )
        {
            ( *items.begin() )->setHidden( false );
        }
    }

    // a module tree item should be deleted
    if( event->type() == WQT_MODULE_DELETE_EVENT )
    {
        WModuleDeleteEvent* e = dynamic_cast< WModuleDeleteEvent* >( event );
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_DELETE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleDeleteEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // grab the module reference and print some info
        boost::shared_ptr< WModule > module = e->getTreeItem()->getModule();
        WLogger::getLogger()->addLogMessage( "Deleting module \"" + module->getName() + "\" from Tree.",
                                             "ControlPanel", LL_DEBUG );

        // remove it from the tree and free last ref count
        m_moduleTreeWidget->deleteItem( e->getTreeItem() );

        // ref count != 1? (only if there are now tree items left)
        bool lastTreeItem = !findItemsByModule( module ).size();
        if( lastTreeItem && ( module.use_count() != 1 ) )
        {
            wlog::error( "ControlPanel" ) << "Removed module has strange usage count: " << module.use_count() << ". Should be 1 here. " <<
                                              "Module reference is held by someone else.";
        }

        return true;
    }

    // a module was removed from the container
    if( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        WModuleRemovedEvent* e = dynamic_cast< WModuleRemovedEvent* >( event );
        if( !e )
        {
            // this should never happen, since the type is set to WQT_MODULE_REMOVE_EVENT.
            WLogger::getLogger()->addLogMessage( "Event is not an WModuleRemovedEvent although its type claims it. Ignoring event.",
                                                 "ControlPanel", LL_WARNING );
            return true;
        }

        // iterate tree items and find proper one
        std::list< WQtTreeItem* > items = findItemsByModule( e->getModule() );
        for( std::list< WQtTreeItem* >::const_iterator iter = items.begin(); iter != items.end(); ++iter )
        {
            ( *iter )->gotRemoved();
        }

        // be nice and print some info
        WLogger::getLogger()->addLogMessage( "Removing module \"" + e->getModule()->getName() + "\" from Tree.", "ControlPanel", LL_DEBUG );

        // stop the module
        e->getModule()->requestStop();
        WLogger::getLogger()->addLogMessage( "Waiting for module \"" + e->getModule()->getName() + "\" to finish before deleting.",
                                             "ControlPanel", LL_DEBUG );

        return true;
    }

    // a connector was updated
    if( event->type() == WQT_MODULE_CONNECTOR_EVENT )
    {
        // convert event to ready event
        WModuleConnectorEvent* e1 = dynamic_cast< WModuleConnectorEvent* >( event );     // NOLINT
        if( e1 )
        {
            // iterate tree items and find proper one -> check if selected
            // NOTE: This could return multiple items here. But, the GUI always selects all of the same module or none. So it is enough to check
            // the first item if it is selected to check whether the current module is active.
            std::list< WQtTreeItem* > items = findItemsByModule( e1->getModule() );
            if( !items.empty() && ( *items.begin() )->isSelected() )
            {
                // ok, the module is selected. Now, update the connectables lists (buttons and menu)
                createCompatibleButtons( e1->getModule() );
            }
        }

        return true;
    }

    return QDockWidget::event( event );
}

std::list< WQtTreeItem* > WQtControlPanel::findItemsByModule( boost::shared_ptr< WModule > module, QTreeWidgetItem* where )
{
    std::list< WQtTreeItem* > l;

    // iterate tree items and find proper one
    QTreeWidgetItemIterator it( where );
    while( *it )
    {
        WQtTreeItem* item = dynamic_cast< WQtTreeItem* >( *it );
        boost::shared_ptr< WModule > itemModule = boost::shared_ptr< WModule >();
        if( item )
        {
            itemModule = item->getModule();
        }

        // if the pointer is NULL the item was none of the above
        if( !itemModule.get() )
        {
            ++it;
            continue;
        }

        // we found it
        if( module == itemModule )
        {
            l.push_back( item );
        }

        ++it;
    }
    return l;
}

std::list< WQtTreeItem* > WQtControlPanel::findItemsByModule( boost::shared_ptr< WModule > module )
{
    std::list< WQtTreeItem* > ret = findItemsByModule( module, m_moduleTreeWidget->invisibleRootItem() );
    std::list< WQtTreeItem* > ret2 = findItemsByModule( module, m_moduleTreeWidget->topLevelItem( 0 ) );
    ret.merge( ret2 );
    return ret;
}

WQtDatasetTreeItem* WQtControlPanel::addDataset( boost::shared_ptr< WModule > module, int subjectId )
{
    int c = getFirstSubject();
    WQtSubjectTreeItem* subject = static_cast< WQtSubjectTreeItem* >( m_moduleTreeWidget->topLevelItem( subjectId + c ) );
    subject->setExpanded( true );
    WQtDatasetTreeItem* item = subject->addDatasetItem( module );
    item->setDisabled( true );
    item->setExpanded( true );

    return item;
}

WQtModuleTreeItem* WQtControlPanel::addModule( boost::shared_ptr< WModule > module )
{
    m_tiModules->setExpanded( true );
    WQtModuleTreeItem* item;
    // for each input, create an item
    m_moduleTreeWidget->setCurrentItem( NULL );
    bool firstItem = true;
    WModule::InputConnectorList cons = module->getInputConnectors();
    for( WModule::InputConnectorList::const_iterator iter = cons.begin(); iter != cons.end(); ++iter )
    {
        // every module gets added to tiModules first. The connection events then move these things to the right parent
        item = m_tiModules->addModuleItem( module );
        item->setDisabled( true );
        item->setExpanded( true );

        // all but the first item get hidden by default. They get visible after a connection event has been fired
        if( !firstItem )
        {
            item->setHidden( true );
        }

        firstItem = false;
    }

    // this module has not inputs. So we simply add it to the tiModules
    if( firstItem )
    {
        item = m_tiModules->addModuleItem( module );
        item->setDisabled( true );
    }

    return item;
}

void WQtControlPanel::addRoi( osg::ref_ptr< WROI > roi )
{
    WQtRoiTreeItem* newItem;
    WQtBranchTreeItem* branchItem;

    m_tiRois->setExpanded( true );
    bool found = false;

    // go through all branches
    for( int branchID = 0; branchID < m_tiRois->childCount(); ++branchID )
    {
        branchItem = dynamic_cast< WQtBranchTreeItem* >( m_tiRois->child( branchID ) );
        // if branch == roi branch
        if( branchItem->getBranch() == WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) )
        {
            found = true;
            break;
        }
    }

    if( !found )
    {
        branchItem = m_tiRois->addBranch( WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) );
    }

    branchItem->setExpanded( true );
    newItem = branchItem->addRoiItem( roi );
    newItem->setDisabled( false );
    newItem->setSelected( true );
    WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
}

void WQtControlPanel::removeRoi( osg::ref_ptr< WROI > roi )
{
    for( int branchID = 0; branchID < m_tiRois->childCount(); ++branchID )
    {
        QTreeWidgetItem* branchTreeItem = m_tiRois->child( branchID );
        for( int roiID = 0; roiID < branchTreeItem->childCount(); ++roiID )
        {
            WQtRoiTreeItem* roiTreeItem = dynamic_cast< WQtRoiTreeItem* >( branchTreeItem->child( roiID ) );
            if( roiTreeItem && roiTreeItem->getRoi() == roi )
            {
                delete roiTreeItem;

                if( branchTreeItem->childCount() == 0 )
                {
                    delete branchTreeItem;
                }

                break;
            }
        }
    }
    WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
}

boost::shared_ptr< WModule > WQtControlPanel::getSelectedModule()
{
    if( m_moduleTreeWidget->selectedItems().at( 0 )->type() == 1 )
    {
        return ( static_cast< WQtDatasetTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) )->getModule() );
    }
    else if( m_moduleTreeWidget->selectedItems().at( 0 )->type() == 3 )
    {
        return ( static_cast< WQtModuleTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) )->getModule() );
    }

    return boost::shared_ptr< WModule >();
}

void WQtControlPanel::reselectTreeItem()
{
    setActiveModule( getSelectedModule(), true );
}

void WQtControlPanel::selectTreeItem()
{
    if( m_ignoreSelectionChange )
    {
        return;
    }

    if( m_moduleTreeWidget->selectedItems().size() != 0  )
    {
        switch( m_moduleTreeWidget->selectedItems().at( 0 )->type() )
        {
            case SUBJECT:
            case MODULEHEADER:
                {
                    // deletion of headers and subjects is not allowed
                    deactivateModuleSelection( false );
                }
                break;
            case DATASET:
                {
                    WModule::SPtr module = ( static_cast< WQtDatasetTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) ) )->getModule();
                    setActiveModule( module );
                }
                break;
            case MODULE:
                {
                    WModule::SPtr module = ( static_cast< WQtModuleTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) ) )->getModule();
                    setActiveModule( module );
                }
                break;
            case ROIHEADER:
            case ROIBRANCH:
            case ROI:
            default:
                deactivateModuleSelection( false );
                break;
        }
    }
    else
    {
        // clean up if nothing is selected
        setActiveModule( WModule::SPtr() );  // module is NULL at this point
    }
}

void WQtControlPanel::selectRoiTreeItem()
{
    clearAndDeleteTabs();

    // Make compatibles toolbar empty
    {
        if( m_mainWindow->getCompatiblesToolbar() != 0 )
        {
            m_mainWindow->getCompatiblesToolbar()->makeEmpty();
        }
        else
        {
            m_mainWindow->setCompatiblesToolbar( new WQtCombinerToolbar( m_mainWindow ) );
        }
    }

    boost::shared_ptr< WModule > module;
    boost::shared_ptr< WProperties > props;

    if( m_roiTreeWidget->selectedItems().size() != 0  )
    {
        switch( m_roiTreeWidget->selectedItems().at( 0 )->type() )
        {
            case SUBJECT:
            case DATASET:
            case MODULEHEADER:
            case MODULE:
            case ROIHEADER:
                WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
                break;
            case ROIBRANCH:
                props = ( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getBranch()->getProperties();
                WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getFirstRoiInSelectedBranch() );
                break;
            case ROI:
                props = ( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi()->getProperties();
                WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getSelectedRoi() );
                break;
            default:
                break;
        }
    }

    if( m_roiTreeWidget->selectedItems().size() == 1 && m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        osg::ref_ptr< WROI > roi = ( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi();
        roi->getProperties()->getProperty( "active" )->toPropBool()->set( m_roiTreeWidget->selectedItems().at( 0 )->checkState( 0 ) );
    }

    buildPropTab( props, boost::shared_ptr< WProperties >() );
}

void WQtControlPanel::selectDataModule( osg::ref_ptr< WGETexture3D > texture )
{
    clearAndDeleteTabs();
    buildPropTab( texture->getProperties(), texture->getInformationProperties() );
}

QTreeWidgetItem* WQtControlPanel::findModuleItem( WModule::SPtr module ) const
{
    QTreeWidgetItemIterator it( m_moduleTreeWidget );
    QTreeWidgetItem* item = NULL;
    while( *it )
    {
        if( dynamic_cast< WQtModuleTreeItem* >( *it ) )
        {
            WModule::SPtr currentModule;
            currentModule = boost::shared_dynamic_cast< WModule >( ( dynamic_cast< WQtModuleTreeItem* >( *it ) )->getModule() );
            if( currentModule )
            {
                if( currentModule == module )
                {
                    item = *it;
                    break;
                }
            }
        }
        else if( dynamic_cast< WQtDatasetTreeItem* >( *it ) )
        {
            boost::shared_ptr< WDataModule > dataModule;
            dataModule = boost::shared_dynamic_cast< WDataModule >( ( dynamic_cast< WQtDatasetTreeItem* >( *it ) )->getModule() );
            if( dataModule )
            {
                if( dataModule == module )
                {
                    item = *it;
                    break;
                }
            }
        }
        ++it;
    }

    return item;
}

void WQtControlPanel::deactivateModuleSelection( bool selectTopmost )
{
    m_ignoreSelectionChange = true;

    m_activeModule = WModule::SPtr();

    // select top element and reset some menu actions
    if( selectTopmost )
    {
        m_moduleTreeWidget->clearSelection();
        selectUpperMostEntry();
    }
    m_deleteModuleAction->setEnabled( false );

    // remove properties tabs
    clearAndDeleteTabs();

    // clean compatibles toolbar, add only subject independent items
    createCompatibleButtons( m_activeModule );

    // also notify network editor
    WQtNetworkEditor* nwe = m_mainWindow->getNetworkEditor();
    if( nwe )
    {
        nwe->clearSelection();
    }

    m_ignoreSelectionChange = false;

    return;
}

void WQtControlPanel::setActiveModule( WModule::SPtr module, bool forceUpdate )
{
    m_ignoreSelectionChange = true;

    // is module NULL? remove everything
    if( !module )
    {
        deactivateModuleSelection();
        m_ignoreSelectionChange = false;
        return;
    }

    // only if something has changed
    if( ( m_activeModule == module ) && !forceUpdate )
    {
        m_ignoreSelectionChange = false;
        return;
    }
    m_activeModule = module;

    wlog::debug( "ControlPanel" ) << "Activating module \"" << module->getName() << "\".";

    // update the m_moduleTreeWidget
    std::list< WQtTreeItem* > items = findItemsByModule( module );
    WAssert( items.size(), "No item found for module " + module->getName() );
    m_moduleTreeWidget->clearSelection();
    for( std::list< WQtTreeItem* >::const_iterator it = items.begin(); it != items.end(); ++it )
    {
        ( *it )->setSelected( true );
    }

    // also notify network editor
    WQtNetworkEditor* nwe = m_mainWindow->getNetworkEditor();
    if( nwe )
    {
        nwe->selectByModule( module );
    }

    // remove property tabs
    clearAndDeleteTabs();
    // update module meta info tab also for crashed modules
    WQtModuleMetaInfo* metaInfoWidget = new WQtModuleMetaInfo( module );
    m_tabWidget->addTab( metaInfoWidget, "About && Help" );

    // set new property tabs if module is not crashed
    if( !module->isCrashed() )
    {
        buildPropTab( module->getProperties(), module->getInformationProperties() );
    }

    // update compatibles toolbar
    createCompatibleButtons( module );

    // disable delete action for tree items that have children.
    if( m_moduleTreeWidget->selectedItems().at( 0 )->childCount() != 0 )
    {
        m_deleteModuleAction->setEnabled( false );
    }
    else
    {
        m_deleteModuleAction->setEnabled( true );
    }

    m_ignoreSelectionChange = false;
}

WQtPropertyGroupWidget*  WQtControlPanel::buildPropWidget( WPropertyGroupBase::SPtr props )
{
    WQtPropertyGroupWidget* tab = new WQtPropertyGroupWidget( props );
    if( props.get() )
    {
        // read lock, gets unlocked upon destruction (out of scope)
        WPropertyGroupBase::PropertySharedContainerType::ReadTicket propAccess = props->getProperties();

        tab->setName( QString::fromStdString( props->getName() ) );

        // iterate all properties.
        for( WPropertyGroupBase::PropertyConstIterator iter = propAccess->get().begin(); iter != propAccess->get().end(); ++iter )
        {
            if( ( *iter )->getType() == PV_GROUP )
            {
                tab->addGroup( buildPropWidget( ( *iter )->toPropGroupBase() ) );
            }
            else
            {
                tab->addProp( *iter );
            }
        }
    }

    tab->addSpacer();
    return tab;
}

void WQtControlPanel::buildPropTab( boost::shared_ptr< WProperties > props, boost::shared_ptr< WProperties > infoProps )
{
    WQtPropertyGroupWidget* tab = NULL;
    WQtPropertyGroupWidget* infoTab = NULL;
    if( props )
    {
        tab = buildPropWidget( props );
        if( tab )
        {
            tab->setName( "Settings" );
        }
    }
    if( infoProps )
    {
        infoTab = buildPropWidget( infoProps );
        if( infoTab )
        {
            infoTab->setName( "Information" );
        }
    }

    int infoIdx = addTabWidgetContent( infoTab );
    int propIdx = addTabWidgetContent( tab );

    // select the property widget preferably
    if( propIdx != -1 )
    {
        m_tabWidget->setCurrentIndex( propIdx );
    }
    else if( infoIdx != -1 )
    {
        m_tabWidget->setCurrentIndex( infoIdx );
    }
}

/**
 * Clears a hierarchy of QActions in a list. This deeply clears and deletes the lists.
 *
 * \param l the list to clear and delete
 */
void deepDeleteActionList( QList< QAction* >& l )   // NOLINT   - we need the non-const ref here.
{
    // traverse
    for( QList< QAction* >::iterator it = l.begin(); it != l.end(); ++it )
    {
        if( ( *it )->menu() )
        {
            // recursively remove sub-menu items
            QList< QAction* > subs = ( *it )->menu()->actions();
            deepDeleteActionList( subs );
        }

        delete ( *it );
    }

    // remove items afterwards
    l.clear();
}

void WQtControlPanel::createCompatibleButtons( boost::shared_ptr< WModule > module )
{
    // we need to clean up the action lists
    deepDeleteActionList( m_addModuleActionList );
    deepDeleteActionList( m_connectWithPrototypeActionList );
    deepDeleteActionList( m_connectWithModuleActionList );
    deepDeleteActionList( m_disconnectActionList );

    // acquire new action lists
    m_connectWithPrototypeActionList = WQtCombinerActionList( this, m_mainWindow->getIconManager(),
                                                              WModuleFactory::getModuleFactory()->getCompatiblePrototypes( module ),
                                                              m_moduleExcluder );
    m_connectWithModuleActionList = WQtCombinerActionList( this, m_mainWindow->getIconManager(),
                                                           WKernel::getRunningKernel()->getRootContainer()->getPossibleConnections( module ),
                                                           0, true );

    m_addModuleActionList = WQtCombinerActionList( this, m_mainWindow->getIconManager(),
                                                           WModuleFactory::getModuleFactory()->getAllPrototypes(),
                                                           0, false );
    if( module )
    {
        m_disconnectActionList = WQtCombinerActionList( this, m_mainWindow->getIconManager(), module->getPossibleDisconnections() );
    }

    // build the add menu
    QMenu* m = new QMenu( m_moduleTreeWidget );
    m->addActions( m_addModuleActionList );
    m_addModuleAction->setDisabled( !m_addModuleActionList.size() || module );  // disable if no entry inside or a module was selected
    delete( m_addModuleAction->menu() ); // ensure that combiners get free'd
    m_addModuleAction->setMenu( m );

    // build the prototype menu
    m = new QMenu( m_moduleTreeWidget );
    m->addActions( m_connectWithPrototypeActionList );
    m_connectWithPrototypeAction->setDisabled( !m_connectWithPrototypeActionList.size() );  // disable if no entry inside
    delete( m_connectWithPrototypeAction->menu() ); // ensure that combiners get free'd
    m_connectWithPrototypeAction->setMenu( m );

    // build the module menu
    m = new QMenu( m_moduleTreeWidget );
    m->addActions( m_connectWithModuleActionList );
    m_connectWithModuleAction->setDisabled( !m_connectWithModuleActionList.size() );  // disable if no entry inside
    delete m_connectWithModuleAction->menu();
    m_connectWithModuleAction->setMenu( m );

    // build the disconnect menu
    m = new QMenu( m_moduleTreeWidget );
    m->addActions( m_disconnectActionList );
    m_disconnectAction->setDisabled( !m_disconnectActionList.size() );  // disable if no entry inside
    delete( m_disconnectAction->menu() ); // ensure that combiners get free'd
    m_disconnectAction->setMenu( m );

    // finally, set the actions to the compatibles toolbar.
    if( m_mainWindow->getCompatiblesToolbar() != 0 )
    {
        m_mainWindow->getCompatiblesToolbar()->updateButtons( m_connectWithPrototypeActionList );
    }
    else
    {
        m_mainWindow->setCompatiblesToolbar( new WQtCombinerToolbar( m_mainWindow, m_connectWithPrototypeActionList ) );
    }
}

void WQtControlPanel::changeTreeItem( QTreeWidgetItem* item, int /* column */ )
{
    WQtTreeItem* witem = dynamic_cast< WQtTreeItem* >( item );
    if( witem )
    {
        witem->handleCheckStateChange();
    }
}

int WQtControlPanel::addTabWidgetContent( WQtPropertyGroupWidget* content )
{
    if( !content || content->isEmpty() )
    {
        // we destroy the widget if we not use it to avoid empty widgets popping up
        if( content )
        {
            delete content;
        }
        return -1;
    }

    QScrollArea* sa = new QScrollArea();
    sa->setWidget( content );
    sa->setWidgetResizable( true );

    return m_tabWidget->addTab( sa, content->getName() );
}

int WQtControlPanel::getFirstSubject()
{
    int c = 0;
    for( int i = 0; i < m_moduleTreeWidget->topLevelItemCount() ; ++i )
    {
        if( m_moduleTreeWidget->topLevelItem( i )->type() == SUBJECT )
        {
            break;
        }
        ++c;
    }
    return c;
}

osg::ref_ptr< WROI > WQtControlPanel::getSelectedRoi()
{
    osg::ref_ptr< WROI > roi;
    if( m_roiTreeWidget->selectedItems().count() == 0 )
    {
        return roi;
    }
    if( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        roi =( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi();
    }
    return roi;
}

osg::ref_ptr< WROI > WQtControlPanel::getFirstRoiInSelectedBranch()
{
    osg::ref_ptr< WROI >roi;
    if( m_roiTreeWidget->selectedItems().count() == 0 )
    {
        return roi;
    }
    if( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
    {
        WQtBranchTreeItem* branch = ( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 )->parent() ) );
        roi =( static_cast< WQtRoiTreeItem* >( branch->child( 0 ) ) )->getRoi();
    }
    if( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROIBRANCH )
    {
        WQtBranchTreeItem* branch = ( static_cast< WQtBranchTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) );
        if( branch->childCount() > 0 )
        {
            roi =( static_cast< WQtRoiTreeItem* >( branch->child( 0 ) ) )->getRoi();
        }
    }
    return roi;
}

QAction* WQtControlPanel::toggleViewAction() const
{
    QAction* result = QDockWidget::toggleViewAction();
    QList< QKeySequence > shortcut;
#if defined( __APPLE__ )
    shortcut.append( QKeySequence( Qt::CTRL + Qt::Key_F9 ) ); // In Mac OS X F9 is already taken by window managment
#else
    shortcut.append( QKeySequence( Qt::Key_F9 ) );
#endif
    result->setShortcuts( shortcut );
    return result;
}

void WQtControlPanel::deleteModule()
{
    if( m_moduleTreeWidget->hasFocus() )
    {
        if( m_moduleTreeWidget->selectedItems().count() > 0 )
        {
            if( ( m_moduleTreeWidget->selectedItems().at( 0 )->type() == MODULE ) ||
                    ( m_moduleTreeWidget->selectedItems().at( 0 )->type() == DATASET ) )
            {
                // deleting crashed modules is not really save as we do not know the internal state of it
                if( static_cast< WQtTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) )->getModule()->isCrashed() )
                {
                    return;
                }

                // remove from the container. It will create a new event in the GUI after it has been removed which is then handled by the tree item.
                // This method deep removes the module ( it also removes depending modules )
                WKernel::getRunningKernel()->getRootContainer()->remove(
                        static_cast< WQtTreeItem* >( m_moduleTreeWidget->selectedItems().at( 0 ) )->getModule()
                        );
                // select another item
                m_moduleTreeWidget->setCurrentItem( m_moduleTreeWidget->topLevelItem( 0 ) );
            }
        }
    }
    else if( m_mainWindow->getNetworkEditor()->hasFocus() )
    {
        if( m_mainWindow->getNetworkEditor()->selectedItems().count() > 0 )
        {
            // deleting crashed modules is not really save as we do not know the internal state of it
            if( static_cast< WQtNetworkItem* >( m_mainWindow->getNetworkEditor()->selectedItems().at( 0 ) )->getModule()->isCrashed() )
            {
                return;
            }

            // This method deep removes the module ( it also removes depending modules )
            WKernel::getRunningKernel()->getRootContainer()->remove(
                static_cast< WQtNetworkItem* >( m_mainWindow->getNetworkEditor()->selectedItems().at( 0 ) )->getModule()
                );
        }
    }
}

void WQtControlPanel::deleteROITreeItem()
{
    osg::ref_ptr< WROI >roi;
    if( m_roiTreeWidget->selectedItems().count() > 0 )
    {
        if( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROIBRANCH )
        {
            roi = getFirstRoiInSelectedBranch();
            if( roi )
            {
                WKernel::getRunningKernel()->getRoiManager()->removeBranch( roi );
            }
            delete m_roiTreeWidget->selectedItems().at( 0 );
        }

        else if( m_roiTreeWidget->selectedItems().at( 0 )->type() == ROI )
        {
            roi =( static_cast< WQtRoiTreeItem* >( m_roiTreeWidget->selectedItems().at( 0 ) ) )->getRoi();
            if( roi )
            {
                WKernel::getRunningKernel()->getRoiManager()->removeRoi( roi );
                // Removing the roi from the tree widget is also done by WROIManagerFibers::removeRoi().
            }
        }
    }
    WKernel::getRunningKernel()->getRoiManager()->setSelectedRoi( getFirstRoiInSelectedBranch() );
}

void WQtControlPanel::selectUpperMostEntry()
{
    m_tiModules->setSelected( true );
}

void WQtControlPanel::handleDragDrop()
{
    WLogger::getLogger()->addLogMessage( "Drag and drop handler not implemented yet!", "ControlPanel", LL_DEBUG );
}

QDockWidget* WQtControlPanel::getRoiDock() const
{
    return m_roiDock;
}

QDockWidget* WQtControlPanel::getModuleDock() const
{
    return m_moduleDock;
}

QDockWidget* WQtControlPanel::getColormapperDock() const
{
    return m_colormapper;
}

WQtModuleConfig& WQtControlPanel::getModuleConfig() const
{
    return *m_moduleExcluder;
}

QAction* WQtControlPanel::getMissingModuleAction() const
{
    return m_missingModuleAction;
}

void WQtControlPanel::clearAndDeleteTabs()
{
    m_tabWidget->setDisabled( true );
    QWidget *widget;
    while( ( widget = m_tabWidget->widget( 0 ) ))
    {
        m_tabWidget->removeTab( 0 );
        delete widget;
    }
    m_tabWidget->setEnabled( true );
}

