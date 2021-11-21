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

#version 150 core

#include "WGEShader-attributes.glsl"
#include "WGEShader-uniforms.glsl"

#include "WGETransformationTools.glsl"

out vec4 v_color;

uniform vec4 u_planeColor;

// The surface normal
out vec3 v_normal;

// Normalized coordinate in the bounding volume of the sphere
out vec3 v_normalizedVertex;

void main()
{
    // prepare light
    v_normal = osg_NormalMatrix * osg_Normal;
    v_normalizedVertex = osg_Vertex.xyz / 100.0;

    // for easy access to texture coordinates
    // gl_TexCoord[0] = osg_MultiTexCoord0;

    //gl_FrontColor = u_planeColor;
    v_color = u_planeColor;
    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;
}
