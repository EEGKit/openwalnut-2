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

#ifndef WVECTOR3D_H
#define WVECTOR3D_H

#include "WValue.hpp"

namespace wmath
{
/**
 * Efficient three-dimensional vector that allows many vector algebra operations
 */
class WVector3D : public WValue< double >
{
    /**
     * Only UnitTests are allowed to be friends of this class
     */
    friend class WVector3DTest;
public:
    /**
     * Produces a zero vector.
     */
    WVector3D();

    /**
     * Produces a vector consisting of the given components.
     */
    WVector3D( double x, double y, double z );

    /**
     * Copies the values of the given WVector3D.
     */
    WVector3D( const WVector3D& newVector );

    /**
     * Compute the cross product of the current WValue with the parameter.
     */
    WVector3D crossProduct( const WVector3D& factor2 ) const
    {
        WVector3D result;
        result[0] = (*this)[1] * factor2[2] - (*this)[2] * factor2[1];
        result[1] = (*this)[2] * factor2[0] - (*this)[0] * factor2[2];
        result[2] = (*this)[0] * factor2[1] - (*this)[1] * factor2[0];
        return result;
    }

    /**
     * Compute the dot product of the current WValue with the parameter.
     */
    double dotProduct( const WVector3D& factor2 ) const
    {
        double result = 0.0;
        for ( unsigned int i = 0; i < 3; ++i )
            result += (*this)[i] * factor2[i];
        return result;
    }

protected:
private:
};

/**
 * Define WPosition as an alias for WVector3D
 */
typedef WVector3D WPosition;
}  // End of namespace
#endif  // WVECTOR3D_H
