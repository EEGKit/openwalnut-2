varying vec4 myColor;
varying vec4 VaryingTexCoord0;

void main()
{
    VaryingTexCoord0 = gl_Vertex;
    
	myColor = gl_Color;

	gl_Position = ftransform(); //< store final position
}
