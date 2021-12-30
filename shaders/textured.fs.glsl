#version 330
#define ITERATIONS 128
#define RADIUS .3

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform vec3 resolution;
uniform bool b;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
    if (!b) {
        color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
    }
    else {
        
        vec2 uv = texcoord.xy;
        vec3 sum = texture(sampler0, uv).xyz;
            
        for(int i = 0; i < ITERATIONS / 4; i++) {
             
            sum += texture(sampler0, uv + vec2(float(i) / resolution.x, 0.) * RADIUS).xyz;
                
        }
            
        for(int i = 0; i < ITERATIONS / 4; i++) {
             
            sum += texture(sampler0, uv - vec2(float(i) / resolution.x, 0.) * RADIUS).xyz;
                
        }
            
        for(int i = 0; i < ITERATIONS / 4; i++) {
             
            sum += texture(sampler0, uv + vec2(0., float(i) / resolution.y) * RADIUS).xyz;
                
        }
            
        for(int i = 0; i < ITERATIONS / 4; i++) {
             
            sum += texture(sampler0, uv - vec2(0., float(i) / resolution.y) * RADIUS).xyz;
                
        }
            
        color = vec4(sum / float(ITERATIONS + 1), 1.);
    }
}
