#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

//out vec2 Texcoord; // To Fragment Shader
//out vec3 L; // To Source
out vec3 V; // To Viewer
//out vec3 Eye; // Eye space
out vec3 N; // Normal
//out vec3 R; // Reflector 

uniform mat4 Projection;
uniform mat4 ModelView;

uniform vec3 EarthColor;
uniform vec3 LightPosition;
out     vec3 light_position;
uniform int  opt;
uniform int  border_opt;

uniform float ks;
uniform float kd;

const float ka=0.5;
//const float ka=1.0;
const float alpha = 3.6;
const vec3 la= vec3(0.2f, 0.2f, 0.2f);
const vec3 ld= vec3(0.5f, 0.5f, 0.5f);
const vec3 ls= vec3(0.8f, 0.8f, 0.8f);

out vec4 gouraud_color;

uniform float angle;

void main() {
  //Texcoord = texcoord;
  mat4 rotationY  = mat4 ( 
    cos(angle), 0, sin(angle), 0,
    0, 1, 0, 0,
    -sin(angle), 0, cos(angle), 0,
    0, 0, 0, 1
  );

  mat4 RotateModelView = ModelView * transpose(rotationY);

  mat4 NormalMatrix = transpose(inverse(RotateModelView));
  light_position = vec3(ModelView * vec4(LightPosition,1.0));
  vec4 earth_color = vec4(EarthColor, 1.0);

  if(opt == 0) { // Phong Shading  // Calculate Position & Normal Here, Calculate Color in Fragment
    V   = vec3(RotateModelView * vec4(position,1.0));  
    N   = normalize(vec3(NormalMatrix * vec4(normal, 1.0)));
    gl_Position = Projection * RotateModelView * vec4(position, 1.0);

  } else if(opt == 1) { // Gouraud Shading // Calculate Color Here
    V   = vec3(RotateModelView * vec4(position,1.0));  
    N   = normalize(vec3(NormalMatrix * vec4(normal, 1.0)));
    gl_Position = Projection * RotateModelView * vec4(position, 1.0); 
  
    vec3 Eye = normalize(-V);
    vec3 L   = normalize(vec3(light_position - V));
    vec3 H   = normalize(L+Eye);

    vec4 Iambient  = vec4(ka * la, 1.0);
    vec4 Idiffuse  = vec4(kd * ld * max(dot(L, N),0.0), 1.0); 
    Idiffuse = clamp(Idiffuse, 0.0, 1.0);
    vec4 Ispecular = vec4(ks * ls * pow(max(dot(H, N),0.0), alpha), 1.0);
    Ispecular = clamp(Ispecular, 0.0, 1.0);
    Ispecular *= smoothstep(0, 0.35, dot(L, N)); // Avoid Back Highlight & Smooth the Edge

    gouraud_color = (Iambient + Idiffuse + Ispecular) * earth_color;
 
  } else if(opt == 2) { // Tone Shading
    V   = vec3(RotateModelView * vec4(position,1.0));  
    N   = normalize(vec3(NormalMatrix * vec4(normal, 1.0)));
    gl_Position = Projection * RotateModelView * vec4(position, 1.0); 
  } 

  // Border Enhancement at Fragment Shader
}
