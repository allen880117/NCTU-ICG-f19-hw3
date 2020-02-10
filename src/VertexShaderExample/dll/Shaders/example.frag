#version 430

//in vec2 Texcoord; // From vertex shader
//in vec3 L; // To Source
in vec3 V; // To Viewer
//in vec3 Eye; // Eye space
in vec3 N; // Normal
//in vec3 R; // Reflector 

out vec4 frag_color;
in  vec4 gouraud_color;

uniform sampler2D earthTexture;
uniform vec3      LightPosition;
in      vec3      light_position;
uniform vec3      EarthColor;
uniform int       opt;
uniform int       border_opt;

uniform float ks;
uniform float kd;

const float ka=0.5;
const float alpha = 3.6;
const vec3 la= vec3(0.2f, 0.2f, 0.2f);
const vec3 ld= vec3(0.5f, 0.5f, 0.5f);
const vec3 ls= vec3(0.8f, 0.8f, 0.8f);

const vec4 RED = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 GEREN = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 BLUE = vec4(0.0, 0.0, 1.0, 1.0);

void main() {
    vec4 earth_color = vec4(EarthColor, 1.0);
    vec3 Eye;// = normalize(-V);
    vec3 L  ;// = normalize(vec3(light_position - V));
    vec3 R  ;// = normalize(reflect(-L,N));  
    vec3 H  ;// = normalize(L+Eye);

    if (opt == 0) { // Phong Shading
        Eye = normalize(-V);
        L   = normalize(vec3(light_position - V));
        H   = normalize(L+Eye);

        vec3 Iambient  = vec3(ka * la);
        // Max for Avoid Negative (The area that light is untouchable)
        vec3 Idiffuse  = vec3(kd * ld * max(dot(L, N),0.0)); 
        Idiffuse = clamp(Idiffuse, 0.0, 1.0);
        // Max for Avoid Negative (The area that light is untouchable)
        //vec3 Ispecular = vec3(ks * ls * pow(max(dot(R, Eye),0.0), alpha));
        vec3 Ispecular = vec3(ks * ls * pow(max(dot(H, N),0.0), alpha));
        Ispecular = clamp(Ispecular, 0.0, 1.0);
        Ispecular *= smoothstep(0, 0.35, dot(L, N)); // Avoid Back Highlight & Smooth the Edge

        vec4 sum = vec4(Iambient+Idiffuse+Ispecular, 1.0);
        frag_color = sum * earth_color;
        //frag_color = texture2D(earthTexture, Texcoord);
    
    } else if(opt == 1) { // Gouraud Shading
        frag_color = gouraud_color;

    } else if(opt == 2) { // Tone Shading
        Eye = normalize(-V);
        L   = normalize(vec3(light_position - V));
        H   = normalize(L+Eye);

        float intensity = max(dot(L, N), 0.0);
        vec4  color;
  
        if(intensity > 0.95) color = vec4(1.0, 1.0, 1.0, 1.0);
        else if (intensity > 0.80) color = vec4(0.95,0.95,0.95,1.0);
        //else if (intensity > 0.5) color = vec4(0.8,0.8,0.8,1.0);
        else if (intensity > 0.25) color = vec4(0.5,0.5,0.5,1.0);
        //else if (intensity > 0.0) color = vec4(0.2,0.2,0.2,1.0);
        else color = vec4(0.1,0.1,0.1,1.0);
        
        frag_color = color * earth_color;
    } 

    // Border Enhancement at Fragment Shader
    if (border_opt == 0){ // Border Enhancement Off
        ;
    } else { // Border Enhancement On
        if( abs(dot(V, N)) <= 1.0 ){
            frag_color = vec4(1.0, 1.0, 1.0, 1.0);
        }
    }
}
