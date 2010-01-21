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
#include <string>

#include "../../../common/WProgressCombiner.h"
#include "../../../common/WProgress.h"

#include "WQtTreeItem.h"

WQtTreeItem::WQtTreeItem( QTreeWidgetItem * parent, int type, boost::shared_ptr< WModule > module ) :
    QTreeWidgetItem( parent, type )
{
    m_module = module;
    m_name = module->getName();

    if ( module->getProperties()->getValue< bool > ( "active" ) )
    {
        this->setCheckState( 0, Qt::Checked );
    }
    else
    {
        this->setCheckState( 0, Qt::Unchecked );
    }

    this->setFlags( Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );

    m_updateTimer = boost::shared_ptr< QTimer >( new QTimer() );
    connect( m_updateTimer.get(), SIGNAL( timeout() ), this, SLOT( update() ) );
    m_updateTimer->start( 50 );
}

WQtTreeItem::~WQtTreeItem()
{
}

boost::shared_ptr< WModule > WQtTreeItem::getModule()
{
    return m_module;
}

std::string WQtTreeItem::getName()
{
    return m_name;
}

void WQtTreeItem::update()
{
    updateState();
}

void WQtTreeItem::updateState()
{
    boost::shared_ptr< WProgressCombiner> p = m_module->getRootProgressCombiner();

    // update the progress combiners internal state
    p->update();

    // is it pending?
    if ( p->isPending() )
    {
        std::ostringstream title;
        title << m_name;

        if ( p->isDetermined() )
        {
            title.setf( std::ios::fixed );
            title.precision( 0 );
            title << " - " << p->getProgress() << "%";
        }
        else
        {
            title << " - Pending";
        }

        setText( 0, title.str().c_str() );
    }
    else
    {
        setText( 0, m_name.c_str() );
    }
}

