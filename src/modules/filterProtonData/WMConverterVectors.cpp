#include "WMConverterVectors.h"

WMConverterVectors::WMConverterVectors()
{
    m_vertices = SPFloatVector( new std::vector< float >() );
    m_sizes = SPFloatVector( new std::vector< float >() );
    m_colors = SPFloatVector( new std::vector< float >() );
    m_edeps = SPFloatVector( new std::vector< float >() );
    m_fiberStartIndexes = SPSizeVector( new std::vector< size_t >() );
    m_fiberLengths = SPSizeVector( new std::vector< size_t >() );
    m_verticesReverse = SPSizeVector( new std::vector< size_t >() );
    m_eventIDs = SPSizeVector( new std::vector< size_t >() );
}

void WMConverterVectors::clear()
{
    m_vertices->clear();
    m_sizes->clear();
    m_colors->clear();
    m_edeps->clear();
    m_fiberStartIndexes->clear();
    m_fiberLengths->clear();
    m_verticesReverse->clear();
    m_eventIDs->clear();
}

SPFloatVector WMConverterVectors::getVertices()
{
    return m_vertices;
}

SPFloatVector WMConverterVectors::getSizes()
{
    return m_sizes;
}

SPFloatVector WMConverterVectors::getColors()
{
    return m_colors;
}

SPFloatVector WMConverterVectors::getEdeps()
{
    return m_edeps;
}

SPSizeVector WMConverterVectors::getFiberStartIndexes()
{
    return m_fiberStartIndexes;
}

SPSizeVector WMConverterVectors::getFiberLengths()
{
    return m_fiberLengths;
}

SPSizeVector WMConverterVectors::getVerticesReverse()
{
    return m_verticesReverse;
}

SPSizeVector WMConverterVectors::getEventIDs()
{
    return m_eventIDs;
}
