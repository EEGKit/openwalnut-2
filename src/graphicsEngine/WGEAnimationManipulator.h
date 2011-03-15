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

#ifndef WGEANIMATIONMANIPULATOR_H
#define WGEANIMATIONMANIPULATOR_H

#include <iostream>

#include <osgGA/MatrixManipulator>
#include <osg/Timer>

#include "WExportWGE.h"

/**
 * New OSG manipulator: AnimationManipulator. Can play back animation paths (not osg::AnimationPath),
 */
class WGE_EXPORT WGEAnimationManipulator: public osgGA::MatrixManipulator
{
public:
    /**
     * Constructs a animation path manipulator.
     */
    WGEAnimationManipulator();

    /**
     * Destructor.
     */
    virtual ~WGEAnimationManipulator();

    /**
     * Sets the current matrix of this animation manipulator. This most probably gets overwritten in the next frame by the current animation
     * matrix.
     *
     * \param matrix the matrix to set
     */
    virtual void setByMatrix( const osg::Matrixd& matrix );

    /**
     * Sets the current inverse matrix of this animation manipulator. This is, in most cases, the modelview matrix.
     * This most probably gets overwritten in the next frame by the current animation
     * matrix.
     *
     * \param matrix the matrix to set
     */
    virtual void setByInverseMatrix( const osg::Matrixd& matrix );

    /**
     * Gets the current animation matrix for the current time-step.
     *
     * \return the matrix.
     */
    virtual osg::Matrixd getMatrix() const;

    /**
     * Gets the current inverse animation matrix for the current time-step. In most cases, this should be used as modelview matrix.
     *
     * \return the matrix
     */
    virtual osg::Matrixd getInverseMatrix() const;

    /**
     * Handles incoming events send by the event dispatcher of the view.
     *
     * \param ea event adapter
     * \param us action adapter allowing interaction with the event dispatcher
     *
     * \return true if the event was handled
     */
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    /**
     * Initializes this manipulator. This simply calls home( 0 ).
     *
     * \param ea event adapter
     * \param us action adapter allowing interaction with the event dispatcher
     */
    virtual void init( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    /**
     * Sets the manipulator back to its default.
     *
     * \param ea event adapter
     * \param us action adapter allowing interaction with the event dispatcher
     */
    virtual void home( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    /**
     * Sets the animation callback to a certain time.
     *
     * \param currentTime the time to which the manipulator should be set-
     */
    virtual void home( double currentTime );

private:

    /**
     * The view matrix. Gets modified on a per-frame basis.
     */
    osg::Matrixd m_matrix;

    /**
     * This timer keeps track of the current time.
     */
    osg::Timer m_timer;

    /**
     * This method updates m_matrix per frame according to time elapsed.
     */
    void handleFrame();
};

#endif  // WGEANIMATIONMANIPULATOR_H
