#version 450
#extension GL_ARB_separate_shader_objects : enable

vec2 iResolution = vec2(800, 600);

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURFACE_DIST .01

float dBox(vec3 p, float s){
    return length(max(abs(p) - s, 0.));
}

float GetDist(vec3 p){
    float d = dBox(vec3(0, 5, 6), 1.);
    return d;
}

vec3 GetNormal(vec3 p){
    float d = GetDist(p);
    
    vec3 n = d - vec3(
        GetDist(p- vec3(.01,0,0)),
        GetDist(p- vec3(0,.01,0)),
        GetDist(p- vec3(0,0,.01)));
    
    return normalize(n);
}

float RayMarch(vec3 ro, vec3 rd) {
    float dO = 0.;
    for(int i=0; i<MAX_STEPS; i++){
        vec3 p = ro + dO*rd;
        float dS = GetDist(p);
        dO += dS;
        if(dS < SURFACE_DIST || dO > MAX_DIST) break;
    }
    return dO;
}

float GetLight(vec3 p) {
    vec3 lightPos = vec3(0,5,6);
    
    vec3 l = normalize(lightPos - p);
    vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n, l), 0.01, 1.);
    
    float d = RayMarch(p + n*SURFACE_DIST*2., l);
    if(d < length(lightPos-p)) {
      dif *=.1;
    }
    
    return dif;
}

layout(location = 0) out vec4 diffuseColor;

void main() {    
    vec2 uv = (.5*iResolution.xy - gl_FragCoord.xy)/iResolution.y;

    vec3 ray_ori = vec3(0, 1, 0);
    vec3 ray_dir = normalize(vec3(uv.x, uv.y, 1));

    float d = RayMarch(ray_ori, ray_dir);
    
    vec3 p = ray_ori + ray_dir * d;
    float dif = GetLight(p);
    vec3 col = vec3(dif);
    
    diffuseColor = vec4(1.0,0.0,0.0,1.0);
}
