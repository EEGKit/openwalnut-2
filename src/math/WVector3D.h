//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

/**
 * Efficient three-dimensional vector that allows many vector algebra operations
 */
class WVector3D : public WValue<double>
{
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

#warning how about constness, returning a reference and so on regarding the following operators.
    /**
     * Returns a reference to to the i-th component.
     */
    double& operator[]( size_t i ) const;
    WVector3D operator+( const WVector3D& summand2 ) const;
    WVector3D operator-( const WVector3D& subtrahend ) const;
    double operator*( const WVector3D &factor2 ) const;
    WVector3D crossproduct( const WVector3D& factor2 ) const;
    bool operator==( const WVector3D& rhs ) const;
    bool operator!=( const WVector3D& rhs ) const;
    double norm() const;
    void normalize();
    WVector3D normalized() const;

protected:
private:
};

/**
 * Define WPosition as an alias for WVector3D
 */
typedef WVector3D WPosition;


#endif  // WVECTOR3D_H
