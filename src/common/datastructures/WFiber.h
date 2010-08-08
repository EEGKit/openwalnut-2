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

#ifndef WFIBER_H
#define WFIBER_H

#include <vector>

#include "../math/WLine.h"
#include "../math/WPosition.h"
#include "../WExportCommon.h"

// we need this to find the WFiberTest class which is not inside wmath namespace
// Additionally I found no way to put the WFiberTest class into wmath and CXXTest
// finds it by it self. Hence this is the only solution that might work
class WFiberTest;

namespace wmath
{
/**
 * Represents a neural pathway.
 */
class EXPORT_OWCOMMON_DLL WFiber : public WLine
{
friend class WFiberTest;
public:
    /**
     * Constructs a new fiber out of WPositions.
     *
     * \param points Reference to the points which belong to this fiber
     */
    explicit WFiber( const std::vector< WPosition > &points );

    /**
     * Creates an empty fiber.
     */
    WFiber();

    /**
     * This is the Smaller thresholded distance as described by Zhang: http://dx.doi.org/10.1109/TVCG.2008.52 .
     *
     * \param thresholdSquare Threshold upto which the distances should be ignored given as square for reasons of performance.
     * \param q First fiber
     * \param r Second fiber
     *
     * \return The minimum of dt(Q,R) and dt(R,Q)
     */
    static double distDST( double thresholdSquare, const wmath::WFiber &q, const wmath::WFiber &r );

    /**
     * This is the Larger thresholded distance as described by Zhang: http://dx.doi.org/10.1109/TVCG.2008.52 .
     *
     * \param thresholdSquare Threshold upto which the distances should be ignored given as square for reasons of performance.
     * \param q First fiber
     * \param r Second fiber
     *
     * \return The maximum of dt(Q,R) and dt(R,Q)
     */
    static double distDLT( double thresholdSquare, const wmath::WFiber &q, const wmath::WFiber &r );

protected:
private:
};

/**
 * Boolean predicate indicating that the first fiber has a greater length then
 * the second one.
 *
 * \param first First fiber
 * \param second Second fiber
 * \return True if the first fiber is longer than the second
 */
inline bool hasGreaterLengthThen( const WFiber &first, const WFiber &second )
{
    return first.size() > second.size();
}

} // end of namespace
#endif  // WFIBER_H
