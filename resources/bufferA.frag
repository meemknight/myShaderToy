void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    fragColor = vec4(texture(iChannel0, fragCoord/iResolution.rg).rgb*0.5,1.0);
}