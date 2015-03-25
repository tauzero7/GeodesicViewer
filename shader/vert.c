// -------------------------------------------------------------------------------
// .NAME     vert.c
// .AUTHOR   Thomas Mueller
// .DATE     11.12.2009
// -----------------------------------------------------------------------------

uniform int  useFog;
uniform vec3 eyePos;

// ---------------------------------------------------
//    m a i n
// ---------------------------------------------------
void main()
{
   gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
   gl_TexCoord[0] = gl_MultiTexCoord0;

   if (useFog==1)
     gl_FogFragCoord = length(eyePos-gl_Vertex.xyz);
}
