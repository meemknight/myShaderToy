const int Count=8;
const float Pixelise=2.;

vec3 Palette[Count]=vec3[Count](
        vec3(72,45,72),
        vec3(93,63,84),
        vec3(115,81,97),
        vec3(137,100,109),
        vec3(160,120,122),
        vec3(184,140,134),
        vec3(209,160,147),
        vec3(234,181,159)
);

vec3 Dither(vec3 InColour, vec2 Pos, float Count);

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    fragCoord=vec2(iResolution.x-fragCoord.x,fragCoord.y);
    fragCoord=floor(fragCoord/Pixelise)*Pixelise;
    vec2 uv = fragCoord/iResolution.xy;

    // Time varying pixel color
    vec3 col = texture(iChannel0,uv).rgb;
    
    col=vec3(col.r*.299+col.g*.587+col.b*.114);
    
    col=Dither(col,fragCoord,float(Count));
    
    col=(Palette[int(floor(col*float(Count)))]+1.)/255.;

    // Output to screen
    fragColor = vec4(col,1.0);
}

vec3 Dither(vec3 InColour, vec2 Pos, float CCount){
    mat4 Dith;
    Dith[0]=vec4(0,8,2,10);
    Dith[1]=vec4(12,4,14,6);
    Dith[2]=vec4(3,11,1,9);
    Dith[3]=vec4(15,7,13,5);
    Dith/=16.*CCount;
    
    return floor((InColour+Dith[int(Pos.x)%4][int(Pos.y)%4])*CCount)/CCount;
}