// -------------------------------------------------------------------------------
// .NAME     frag.c
// .AUTHOR   Thomas Mueller
// .DATE     04.12.2009
// -----------------------------------------------------------------------------

uniform int   useFog;
uniform float fogFactor;

uniform vec3  col1;
uniform vec3  col2;

uniform float freq;

uniform int   colortype;

// ---------------------------------------------------
//    m a i n
// ---------------------------------------------------
void main()
{ 
  vec2 tc = gl_TexCoord[0].st;
  
  float val1 = 0.5*(sin(tc.y*freq)+1.0);
  float val2 = 0.5*(cos(tc.y*freq+1.5707963)+1.0);

  vec4 color = vec4(0.0,0.0,0.0,1.0);
  if (colortype==0)
    color.rgb = vec3(val1,val1,val1);
  else if (colortype==1)
    color.rgb = vec3(val2,val2,val2);
  else if (colortype==2)
  {
    color.rgb = col1*val1 + col2*val2;
  }

  if (useFog==1)
  {
    float fog = exp2( gl_FogFragCoord*gl_FogFragCoord*fogFactor );
    fog = clamp(fog,0.0,1.0);
    color = mix(vec4(0.0,0.0,0.0,1.0),color,fog);
  }
  gl_FragColor = color;
}

