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

#ifndef WGEGEODEUTILS_H
#define WGEGEODEUTILS_H

#include <osg/Geode>

#include "../common/WColor.h"
#include "../math/WPosition.h"

namespace wge
{
    /**
     * Generates an OSG geode for the bounding box.
     *
     * \param pos1 Front lower left corner
     * \param pos2 Back upper right corner
     * \param color The color in which the bounding box should be generated
     *
     * \return The OSG geode containing the 12 edges of the box.
     */
    osg::ref_ptr< osg::Geode > generateBoundingBoxGeode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color );

} // end of namespace wge

#endif  // WGEGEODEUTILS_H
