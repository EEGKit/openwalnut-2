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

#include <cstddef>

#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Array>
#include <osg/Drawable>
#include <osg/Geometry>

#include "../../common/WFlag.h"
#include "WEEGEvent.h"
#include "WHeadSurfaceCallback.h"


WHeadSurfaceCallback::WHeadSurfaceCallback( const std::vector< std::size_t >& channelIDs,
                                            boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > > event )
    : m_currentTime( -1.0 ),
      m_channelIDs( channelIDs ),
      m_event( event )
{
}

void WHeadSurfaceCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable )
{
    boost::shared_ptr< WEEGEvent > event = m_event->get();
    const double newTime = event->getTime();
    if( newTime != m_currentTime )
    {
        osg::Geometry* geometry = static_cast< osg::Geometry* >( drawable );
        if( geometry )
        {
            osg::FloatArray* texCoords = static_cast< osg::FloatArray* >( geometry->getTexCoordArray( 0 ) );
            if( 0.0 <= newTime )
            {
                const std::vector< double >& values = event->getValues();
                for( std::size_t vertexID = 0; vertexID < texCoords->size(); ++vertexID )
                {
                    const int size = 256;
                    const double scale = 0.01;
                    const double factor = scale * ( 1.0 - 1.0 / size );
                    (*texCoords)[vertexID] = values[m_channelIDs[vertexID]] * factor + 0.5;
                }
            }
            else
            {
                for( std::size_t vertexID = 0; vertexID < texCoords->size(); ++vertexID )
                {
                    (*texCoords)[vertexID] = 0.5f;
                }
            }
            geometry->setTexCoordArray( 0, texCoords );
        }

        m_currentTime = newTime;
    }
}
