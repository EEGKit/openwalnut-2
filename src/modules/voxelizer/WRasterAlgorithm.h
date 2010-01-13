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

#ifndef WRASTERALGORITHM_H
#define WRASTERALGORITHM_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../math/WLine.h"
#include "WRasterAlgorithm.h"

/**
 * TODO(math): Document this!
 */
class WRasterAlgorithm
{
public:
    /**
     * TODO(math): Document this!
     */
    explicit WRasterAlgorithm( boost::shared_ptr< WGridRegular3D > grid );

    /**
     * TODO(math): Document this!
     */
    virtual ~WRasterAlgorithm();

    /**
     * Rasterize the given line into the grid of dataset.
     * The value of the voxel which will be hit changes its value.
     *
     *\param line Polyline which is about to be rastered.
     */
    virtual void raster( const wmath::WLine& line ) = 0;

    /**
     * Computes a dataset out of our voxel values and the previously given
     * grid. Note this may take some time.
     *
     *\return Dataset where all voxels which are hit by the rastered lines are
     * non zero.
     */
    boost::shared_ptr< WDataSetSingle > generateDataSet() const;

protected:
    /**
     * Marks a given voxel as hit.
     */
    virtual void markVoxel( int x, int y, int z );

    /**
     * The grid is used for the following purposes:
     *  - First we need it when creating the final dataset
     *  - Second we need it to determine how many voxels are there at
     *    construction time
     *  - Thrid we need it when computing the value number out of a position
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * Stores the value of each voxel. If and only if a voxel is _not_ hit by
     * a line segment its value is 0.0.
     */
    std::vector< double > m_values;

private:
};

inline void WRasterAlgorithm::markVoxel( int x, int y, int z )
{
    size_t idx = x + y * m_grid->getNbCoordsX() + z * m_grid->getNbCoordsX() * m_grid->getNbCoordsY();
    m_values[ idx ] = 1.0;
}

#endif  // WRASTERALGORITHM_H
