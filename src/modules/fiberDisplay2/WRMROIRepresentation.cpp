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

#include <list>
#include <vector>

#include "WRMBranch.h"

#include "WROIManagerFibers.h"
#include "../../graphicsEngine/WROIBox.h"

#include "WRMROIRepresentation.h"



WRMROIRepresentation::WRMROIRepresentation( boost::shared_ptr< WROI > roi, boost::shared_ptr< WRMBranch > branch  ) :
    m_roi( roi ),
    m_branch( branch )
{
    roi->getSignalIsModified()->connect( boost::bind( &WRMROIRepresentation::setDirty, this ) );
    setDirty();
}

WRMROIRepresentation::~WRMROIRepresentation()
{
}

boost::shared_ptr< WROI > WRMROIRepresentation::getROI()
{
    return m_roi;
}

boost::shared_ptr< std::vector< bool > > WRMROIRepresentation::getBitField( unsigned int index )
{
    if ( m_dirty )
    {
        recalculate();
    }
    unsigned int c = 0;
    for( std::list< boost::shared_ptr< std::vector<bool> > >::iterator iter = m_bitFields.begin(); iter != m_bitFields.end(); ++iter )
    {
        if ( c == index)
        {
            return *iter;
        }
        ++c;
    }
}

void WRMROIRepresentation::addBitField( size_t size )
{
    boost::shared_ptr< std::vector< bool > >bf = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( size, false ) );
    m_bitFields.push_back( bf );
    setDirty();
}

void WRMROIRepresentation::recalculate()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    //std::cout << "roi recalc" << std::endl;
    m_currentBitfield = m_bitFields.front();
    size_t size = m_currentBitfield->size();
    m_currentBitfield->clear();
    m_currentBitfield->resize( size, false );

    m_currentArray = m_branch->getRoiManager()->getDataSet( 0 )->getVertices();
    m_currentReverse = m_branch->getRoiManager()->getDataSet( 0 )->getVerticesReverse();
    m_kdTree = m_branch->getRoiManager()->getKdTree( 0 );

    m_boxMin.resize( 3 );
    m_boxMax.resize( 3 );

    boost::shared_ptr<WROIBox> box = boost::shared_static_cast<WROIBox>( m_roi );

    m_boxMin[0] = box->getMinPos()[0];
    m_boxMax[0] = box->getMaxPos()[0];
    m_boxMin[1] = box->getMinPos()[1];
    m_boxMax[1] = box->getMaxPos()[1];
    m_boxMin[2] = box->getMinPos()[2];
    m_boxMax[2] = box->getMaxPos()[2];

    boxTest( 0, m_currentArray->size() / 3 - 1, 0 );

//    int counter = 0;
//    for (int i = 0 ; i < m_currentBitfield->size() ; ++i)
//    {
//        if ( m_currentBitfield->at(i) )
//            ++counter;
//    }
//    std::cout << "active fibers:" << counter << std::endl;
    m_dirty = false;

    slock.unlock();
}

void WRMROIRepresentation::boxTest( int left, int right, int axis )
{
    // abort condition
    if ( left > right )
        return;

    int root = left + ( ( right - left ) / 2 );
    int axis1 = ( axis + 1 ) % 3;
    int pointIndex = m_kdTree->m_tree[root] * 3;

    if ( m_currentArray->at( pointIndex + axis ) < m_boxMin[axis] )
    {
        boxTest( root + 1, right, axis1 );
    }
    else if ( m_currentArray->at( pointIndex + axis ) > m_boxMax[axis] )
    {
        boxTest( left, root - 1, axis1 );
    }
    else
    {
        int axis2 = ( axis + 2 ) % 3;
        if ( m_currentArray->at( pointIndex + axis1 ) <= m_boxMax[axis1] && m_currentArray->at( pointIndex + axis1 )
                >= m_boxMin[axis1] && m_currentArray->at( pointIndex + axis2 ) <= m_boxMax[axis2]
                && m_currentArray->at( pointIndex + axis2 ) >= m_boxMin[axis2] )
        {
            m_currentBitfield->at( getLineForPoint( m_kdTree->m_tree[root] ) ) = 1;
        }
        boxTest( left, root - 1, axis1 );
        boxTest( root + 1, right, axis1 );
    }
}

unsigned int WRMROIRepresentation::getLineForPoint( unsigned int point )
{
    return m_currentReverse->at( point );
}

bool WRMROIRepresentation::isDirty()
{
    return m_dirty;
}

void WRMROIRepresentation::setDirty()
{
    m_dirty = true;
    m_branch->setDirty();
}
