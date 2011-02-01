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

#version 120

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#ifdef CLIPPLANE_ENABLED
/**
 * The distance to the plane
 */
varying float dist;
#endif

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/**
 * A point on the plane.
 */
uniform vec3 u_planePoint;

/**
 * The normal vector of the plane
 */
uniform vec3 u_planeVector;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point of the vertex shader.
 */
void main()
{
#ifdef CLIPPLANE_ENABLED
    // define the plane
    vec3 n = normalize( u_planeVector );
    float d = dot( u_planePoint, n );
    dist = dot( gl_Vertex.xyz, n ) - d;
#endif

    // Grab the tangent. We have uploaded it normalized in gl_Normal per vertex
    // We need to transfer it to the world-space ass all further operations are done there.
    vec3 tangent = normalize( ( gl_ModelViewMatrix * vec4( gl_Normal, 0.0 ) ).xyz );

    // The same accounds for the vertex. Transfer it to world-space.
    vec4 vertex  = gl_ModelViewMatrix * gl_Vertex;

    // The view direction in world-space. In OpenGL this is always defined by this vector
    vec3 view = vec3( 0.0, 0.0, -1.0 );

    // To avoid that the quad strip gets thinner and thinner when zooming in (or the other way around: to avoid the quad strip always occupies
    // the same screen space), we need to calculate the zoom factor involved in OpenWalnut's current camera.
    float worldScale = length( ( gl_ModelViewMatrix * vec4( 1.0, 1.0, 1.0, 0.0 ) ).xyz );

    // To enforce that each quad's normal points towards the user, we move the two vertex (which are at the same point currently) according to
    // the direction stored in gl_MultiTexCoord0
    vec3 offset = normalize( gl_MultiTexCoord0.s * cross( view, tangent ) );

    // Apply the offset and scale correctly.
    vertex.xyz += 0.1 * worldScale * offset;

    // Simply project the vertex afterwards
    gl_Position = gl_ProjectionMatrix * vertex;
    gl_FrontColor = vec4( vec3( gl_MultiTexCoord0.s ), 1.0 );
}

