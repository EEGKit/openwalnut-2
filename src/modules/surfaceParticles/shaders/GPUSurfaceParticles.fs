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
#extension GL_EXT_gpu_shader4 : enable

#include "TextureUtils.glsl"
#include "TransformationTools.glsl"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Varyings
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "GPUSurfaceParticles.varyings"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
///////////////////////////////////////////////////////////////////////////////////////////////////

// **************************************************************************
// Uniforms defining the input textures
// **************************************************************************

// texture containing the data
uniform sampler3D tex0;

// texture containing the directional data
uniform sampler3D tex1;

// texture containing the directional data -> the scaling factor of the values in the texture
uniform float u_tex1Scale;

// texture containing the directional data -> the min value of the values in the texture
uniform float u_tex1Min;

// texture containing the directional data -> the max value of the values in the texture
uniform float u_tex1Max;

// texture containing the tracing data
uniform sampler3D tex2;

// texture containing the tracing data -> the scaling factor of the values in the texture
uniform float u_tex2Scale;

// texture containing the tracing data -> the min value of the values in the texture
uniform float u_tex2Min;

// texture containing the tracing data -> the max value of the values in the texture
uniform float u_tex2Max;

// **************************************************************************
// Uniforms for the isosurface mode
// **************************************************************************

// The isovalue to use.
uniform float u_isovalue;

// The number of steps to use.
uniform int u_steps;

// The alpha value to set
uniform float u_alpha;

// **************************************************************************
// Uniforms defining the initial particle distribution
// **************************************************************************

// Uniform defining the grid resolution scaling in relation to the dataset grid
uniform float u_gridResolution;

// Relative size of the particle to the voxel
uniform float u_particleSize;

// Animation reference.
uniform int u_animationTime;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Calculates the endpoint and distance using the ray and ray-start-point up to the end of the
 * bounding volume.
 * 
 * \param d the distance along the ray until the ray leaves the bounding volume.
 * 
 * \return the end point -> v_rayStart + v_ray*d
 */
vec3 findRayEnd( out float d )
{
    vec3 r = v_ray + vec3( 0.0000001 );
    vec3 p = v_rayStart;

    // we need to ensure the vector components are not exactly 0.0

    // v_ray in cube coordinates is used to check against the unit cube borders
    // when will v_ray reach the front face?
    float tFront     = - p.z / r.z;                  // (x,x,0) = v_rayStart + t * v_ray
    float tBack      = ( 1.0 - p.z ) / r.z;          // (x,x,1) = v_rayStart + t * v_ray

    float tLeft      = - p.x / r.x;                  // (0,x,x) = v_rayStart + t * v_ray
    float tRight     = ( 1.0 - p.x ) / r.x;          // (1,x,x) = v_rayStart + t * v_ray

    float tBottom    = - p.y / r.y;                  // (x,0,x) = v_rayStart + t * v_ray
    float tTop       = ( 1.0 - p.y ) / r.y;          // (x,1,x) = v_rayStart + t * v_ray

    // get the nearest hit
    d = min( min( max( tFront, tBack ), max( tLeft, tRight ) ), max ( tBottom, tTop ) );
    return p + ( r * d );
}

/**
 * Distance between two points.
 * 
 * \param p1 point 1
 * \param p2 point 2
 * 
 * \return || p1-p2 ||
 */
float pointDistance( vec3 p1, vec3 p2 )
{
    return length( p1 - p2 );
}

/**
 * Gets the direction stored in tex1 at the given point.
 * 
 * \param point the point
 * 
 * \return the direction at point
 */
vec3 getDirection( vec3 point )
{
    return texture3DUnscaled( tex1, point, u_tex1Min, u_tex1Scale ).rgb;
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // please do not laugh, it is a very very very simple "isosurface" shader
    vec4 color = vec4( 1.0, 0.0, 0.0, 1.0 );
    gl_FragDepth = gl_FragCoord.z; 

    // First, find the rayEnd point. We need to do it in the fragment shader as the ray end point may be interpolated wrong
    // when done for each vertex.
    float totalDistance = 0.0;
    vec3 rayEnd = findRayEnd( totalDistance );

    // the point along the ray in cube coordinates
    vec3 curPoint = v_rayStart;

    // the current value inside the data
    float value;

    // the step counter
    int i = 0;
    float stepDistance = totalDistance / float( u_steps );
    while ( i < u_steps ) // we do not need to ch
    {
        // get current value
        value = texture3D( tex0, curPoint ).r;

        // is it the isovalue?
        if ( abs( value - u_isovalue ) < 0.1 )
        {
            // we need to know the depth value of the current point inside the cube
            // Therefore, the complete standard pipeline is reproduced here:
            
            // 1: transfer to world space and right after it, to eye space
            vec3 curPointProjected = project( curPoint );
            
            // 2: set depth value
            gl_FragDepth = curPointProjected.z;

            // 3: Project direction to image space
            vec2 projectedDirection = projectVector( getDirection( curPoint ) ).xy;
            float angle = float( u_animationTime ) / 15.0;
            mat2 rot = mat2( vec2( cos( angle ) , sin( angle ) ), vec2( -sin( angle ), cos( angle )  ) );
            projectedDirection = rot * projectedDirection;
            
            float test = ( texture3D( tex2, curPoint ).r );
            float testInv = 1.0 - ( texture3D( tex2, curPoint ).r );

            // 4: initial particle distribution
            // The values need to be transferred to the next (image space based) steps.

            // the current point is now relating to the coordinate system, which is in { (x,y,z) | x in [-1,1], y in [-1,1] and z in [-1,1] } 
            // (the texture coordinate system). We increase the resolution by scaling the point:
            curPoint *= u_gridResolution;

            // each point can than be defined as offset to a voxels root coordinate:
            vec3 rasterPoint = curPoint - floor( curPoint );
            rasterPoint -= vec3( 0.5, 0.5, 0.5 );

            // the raster point is then, for each voxel in  { (x,y,z) | x in [-1,1], y in [-1,1] and z in [-1,1] }, interpreted as sphere surface:
            float sphere = ( rasterPoint.x * rasterPoint.x + rasterPoint.y * rasterPoint.y  ) * 4.0 * u_particleSize;

            // 5: Set all outputs
            // Tex0: Isosurface, Depth, Particle Distribution, Alpha
            gl_FragData[0] = vec4( curPointProjected.z, curPointProjected.z, 1.0 - sphere, u_alpha );
            // Tex1: Projected Directions X, Projected Directions Y, Projected Directions Z
            gl_FragData[1] = vec4( projectedDirection.r, 0.0, 0.0, 1.0 );
            //gl_FragData[1] = vec4( normalize( abs( projectedDirection ) ), 0.0, 1.0 );

            int timeStep = u_animationTime;
            // this creates a triangle function
            
            float anim1 = ( ( timeStep * 1 ) % 100) * 0.012;
            float anim2 = ( ( timeStep * 1 ) % 100) * 0.012;

            vec4 ocol = vec4( 0.0, 0.0, 0.0, 1.0);

            int t = int( abs( anim1 - test ) * 10.0 ) % 10;
            float t2 = ( t % 2 ) / 2.0;
        
            bool doRed = abs( anim1 - test ) <= 0.075;
            bool doGreen = abs( anim2 - testInv ) <= 0.01;
            if ( doRed )
            {
                ocol.r +=  test * test * 10.0;
            }
            if ( doGreen )
            {
                ocol.r = 0.0;
                ocol.g += testInv * testInv * 10.0;
            }

            //ocol.b =test ;
            gl_FragData[0] = ocol;

            break;
        }
        else 
        {
            // no it is not the iso value
            // -> continue along the ray
            curPoint += stepDistance * v_ray;
        }

        // do not miss to count the steps already done
        i++;
    }

    // the ray did never hit the surface --> discard the pixel
    if ( i == u_steps )
    {
        discard;
    }
}

