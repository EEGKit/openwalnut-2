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
#include <vector>

#include "QtGui/QMenu"

#include "../../common/WPreferences.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleCombiner.h"
#include "guiElements/WQtApplyModuleAction.h"
#include "WMainWindow.h"

#include "WQtCombinerActionList.h"

WQtCombinerActionList::WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCompatiblesList compatibles ):
    QList< QAction* >()
{
    // These modules will be allowed to be shown.
    std::string moduleWhiteListString;
    WPreferences::getPreference( "modules.whiteList", &moduleWhiteListString );
    std::vector< std::string > moduleWhiteList = string_utils::tokenize( moduleWhiteListString, "," );

    // create an action for each group:
    for ( WCompatiblesList::iterator groups = compatibles.begin(); groups != compatibles.end(); ++groups )
    {
        // check current prototype against whitelist
        if( moduleWhiteList.size()
            && std::find( moduleWhiteList.begin(), moduleWhiteList.end(), groups->first->getName() ) == moduleWhiteList.end() )
        {
            continue;
        }

        // create a new action for this group
        WQtApplyModuleAction* group = new WQtApplyModuleAction( parent, icons, *( *groups ).second.begin() );
        group->setIconVisibleInMenu( true );
        push_back( group );

        // only add a sub menu if there are more than 1 items in the group
        if ( ( *groups ).second.size() > 1 )
        {
            QMenu* groupMenu = new QMenu( parent );
            // iterate all the children
            for ( WCompatibleCombiners::iterator combiner = ( *groups ).second.begin();
                                                                       combiner != ( *groups ).second.end(); ++combiner )
            {
                WQtApplyModuleAction* a = new WQtApplyModuleAction( parent, icons, ( *combiner ), true );
                a->setIconVisibleInMenu( true );
                groupMenu->addAction( a );
            }
            group->setMenu( groupMenu );
        }
    }
}

WQtCombinerActionList::~WQtCombinerActionList()
{
    // cleanup
    clear();
}

