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

#ifndef WQTTREEITEM_H
#define WQTTREEITEM_H

#include <string>

#include <QtGui/QTreeWidgetItem>
#include <QtGui/QProgressBar>
#include <QtCore/QTimer>
#include "../../../kernel/WModule.h"

/**
 * Base class for all items in the dataset browser.
 */
class WQtTreeItem: public QObject,
                   public QTreeWidgetItem
{
    Q_OBJECT

public:
    /**
     * Constructor creates an empty item.
     *
     * \param parent The widget managing this widget
     * \param module The represented module
     * \param type the type used for the treeitem. Used to identify the items.
     */
    WQtTreeItem( QTreeWidgetItem * parent, int type, boost::shared_ptr< WModule > module );

    /**
     * Destructor.
     */
    virtual ~WQtTreeItem();

    /**
     * Get for the module pointer.
     *
     * \return the pointer to the module associated with this item.
     */
    boost::shared_ptr< WModule >getModule();

    /**
     * Returns the name used for this tree item.
     *
     * \return the name.
     */
    std::string getName();

public slots:

    /**
     * Gets called by m_updateTimer in some interval to update the item state, basing on the state of m_module.
     */
    void update();

protected:

    /**
     * Updates the state of the tree item basing on the module's state.
     */
    virtual void updateState();

    /**
     * Updates this item in regular intervals.
     */
    boost::shared_ptr< QTimer > m_updateTimer;

    /**
     * Name of the tree item.
     */
    std::string m_name;

private:

    /**
     * The module represented by this tree item.
     */
    boost::shared_ptr< WModule > m_module;
};

#endif  // WQTTREEITEM_H
