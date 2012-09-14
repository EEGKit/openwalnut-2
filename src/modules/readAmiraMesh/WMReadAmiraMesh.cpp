//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "WMReadAmiraMesh.xpm"
#include "core/common/WIOTools.h"
#include "core/common/WPathHelper.h"
#include "core/dataHandler/WDataSetFiberVector.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WKernel.h"

#include "WMReadAmiraMesh.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadAmiraMesh )

WMReadAmiraMesh::WMReadAmiraMesh():
    WModule()
{
}

WMReadAmiraMesh::~WMReadAmiraMesh()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMReadAmiraMesh::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadAmiraMesh() );
}

const char** WMReadAmiraMesh::getXPMIcon() const
{
    return WMReadAmiraMesh_xpm;
}
const std::string WMReadAmiraMesh::getName() const
{
    return "[IN DEVELOPMENT] Read Amira Mesh";
}

const std::string WMReadAmiraMesh::getDescription() const
{
    return "Read AmiraMesh file format. At the moment only spatial graphs are supported.";
}

void WMReadAmiraMesh::connectors()
{
    // initialize connectors
    m_output = boost::shared_ptr< WModuleOutputData< WDataSetFibers > >( new WModuleOutputData< WDataSetFibers >(
                shared_from_this(), "out", "A loaded dataset." )
            );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    WModule::connectors();
}

void WMReadAmiraMesh::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_dataFile = m_properties->addProperty( "Filename", "", WPathHelper::getAppPath(), m_propCondition );

    WModule::properties();
}

void WMReadAmiraMesh::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMReadAmiraMesh::moduleMain()
{
    m_moduleState.add( m_propCondition );
    ready();
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        bool readingSuccessful = readAmiraMesh( m_dataFile->get().string() );
        if( readingSuccessful )
        {
            prepareResult();
        }
    }
}

void WMReadAmiraMesh::prepareResult()
{
    boost::shared_ptr< std::vector< WFiber > > fibs( new std::vector< WFiber > );
    size_t globalPointId = 0;
    for( size_t edgeId = 0; edgeId < m_edges.size(); ++edgeId )
    {
        std::vector< WPosition > fiberPoints;
        for( size_t localPointId = 0; localPointId < m_numEdgePoints[edgeId]; ++localPointId )
        {
            fiberPoints.push_back( m_edgePoints[globalPointId] );
            ++globalPointId;
        }

        fibs->push_back( WFiber( fiberPoints ) );
    }
    WDataSetFiberVector fibersVector( fibs );
    m_graph = boost::shared_ptr< WDataSetFibers >( fibersVector.toWDataSetFibers() );
    m_output->updateData( m_graph );
}

bool checkVersionString( std::string line )
{
    return line == "# AmiraMesh 3D ASCII 2.0";
}

size_t parseDefine( std::string line )
{
    std::vector< std::string > tokens = string_utils::tokenize( line );
    return string_utils::fromString< double >( tokens[2] );
}

void skipEmptyAndCommentLines( std::ifstream* data, std::string* line )
{
    getline( *data, *line );
    while( *line == std::string("") || (*line)[0] == '#' )
    {
        getline( *data, *line );
    }
}

bool parseParameters( std::ifstream* data, std::string* line )
{
    bool contenTypeIsHxSpatialGraph = false;
    while( *line != std::string("}") )
    {
        if( line->find( "ContentType" ) != std::string::npos
            && line->find( "HxSpatialGraph" ) != std::string::npos )
        {
            contenTypeIsHxSpatialGraph = true;
        }

        getline( *data, *line );
    }
    return contenTypeIsHxSpatialGraph;
}

bool WMReadAmiraMesh::readAmiraMesh( std::string fileName )
{
    std::ifstream dataFile( fileName.c_str() );

    if( dataFile && fileExists( fileName ) )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Read Amira Mesh", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + fileName , "Read Amira Mesh", LL_ERROR );
        return false;
    }

    std::string tmp;
    getline( dataFile, tmp );
    if( !checkVersionString( tmp ) )
    {
        return false;
    }

    skipEmptyAndCommentLines( &dataFile, &tmp );

    std::vector< size_t > dimensions;
    while( tmp.find( "define " ) == 0 )
    {
        dimensions.push_back( parseDefine( tmp ) );
        getline( dataFile, tmp );
    }

    skipEmptyAndCommentLines( &dataFile, &tmp );

    if( tmp.find( "Parameters {" ) == 0 )
    {
        if( !parseParameters( &dataFile, &tmp ) )
        {
            return false;
        }
    }
    else
    {
        return false;
    }

//#warning use information about defines
    while( tmp.find( "@1" ) != 0 )
    {
        getline( dataFile, tmp );
    }


    WPosition vertex;
    m_vertices.clear();
    for( size_t vertexId = 0; vertexId < dimensions[0]; ++vertexId )
    {
        dataFile >> vertex[0] >> vertex[1] >> vertex[2];
        m_vertices.push_back( vertex );
    }



//#warning use information about defines
    while( tmp.find( "@2" ) != 0 )
    {
        getline( dataFile, tmp );
    }


    std::pair< size_t, size_t > edge;
    m_edges.clear();
    for( size_t edgeId = 0; edgeId < dimensions[1]; ++edgeId )
    {
        dataFile >> edge.first >> edge.second;
        m_edges.push_back( edge );
    }

//#warning use information about defines
    while( tmp.find( "@3" ) != 0 )
    {
        getline( dataFile, tmp );
    }

    m_numEdgePoints.resize( dimensions[1] );
    for( size_t edgeId = 0; edgeId < dimensions[1]; ++edgeId )
    {
        dataFile >> m_numEdgePoints[edgeId];
    }

//#warning use information about defines
    while( tmp.find( "@4" ) != 0 )
    {
        getline( dataFile, tmp );
    }

    m_edgePoints.resize( dimensions[2] );
    for( size_t pointId = 0; pointId < dimensions[2]; ++pointId )
    {
        dataFile >> m_edgePoints[pointId][0] >> m_edgePoints[pointId][1] >>  m_edgePoints[pointId][2];
    }

    return true;
}
