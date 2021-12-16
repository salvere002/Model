// ==================================================================
#version 330 core
out vec4 color;

vec3 theColor; 

void main()
{
  theColor.x = 100;
  theColor.y = 200;
  theColor.z = 200;

  color = vec4(theColor, 1.0);
}
// ==================================================================
