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

#ifndef WLOADERFIBERS2_H
#define WLOADERFIBERS2_H

#include <fstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../WLoader.h"
#include "../exceptions/WDHIOFailure.h"
#include "../../math/WPosition.h"
#include "../../math/WFiber.h"

/**
 * Loader for the VTK file formats. For VTK just see http://www.vtk.org.
 * Currently only a subset of the legacy format is supported: MedInria's
 * Fib-VTK format. The byte order of the files is assumed to be big endian by
 * default.
 *
 * \ingroup dataHandler
 */
class WLoaderFibers2 : public WLoader
{
public:
    /**
     * Constructs and makes a new VTK loader for separate thread start.
     *
     * \param fname File name where to load data from
     * \throws WDHIOFailure
     */
    explicit WLoaderFibers2( std::string fname ) throw( WDHIOFailure );

    /**
     * Destroys this instance and closes the file.
     */
    virtual ~WLoaderFibers2() throw();

    /**
     * Loads the dataset.
     *
     * \return the dataset loaded.
     */
    virtual boost::shared_ptr< WDataSet > load();

protected:
    /**
     * Read header from file.
     *
     * \return The offset where header ends, so we may skip this next operation.
     */
    void readHeader() throw( WDHIOFailure, WDHException );

    /**
     * Read points from input stream.
     */
    void readPoints();

    /**
     * Read lines from input stream and use the points for constructing fibers.
     */
    void readLines();

    /**
     * First four lines of ASCII text describing this file
     */
    std::vector< std::string > m_header;

    /**
     * Point vector for all fibers that is actually usable for what we want to do
     */
    boost::shared_ptr< std::vector< float > > m_vertices;

    /**
     * Line vector that contains the start index for each line
     */
    boost::shared_ptr< std::vector< unsigned int > > m_lineStartIndexes;

    /**
     * Line vector that contains the number of vertices for each line
     */
    boost::shared_ptr< std::vector< unsigned int > > m_lineLengths;

    /**
     * Reverse lookup table for which point belongs to which fiber
     */
    boost::shared_ptr< std::vector< unsigned int > > m_verticesReverse;

    /**
     * Pointer to the input file stream reader.
     */
    boost::shared_ptr< std::ifstream > m_ifs;

private:
};

#endif  // WLOADERFIBERS2_H
