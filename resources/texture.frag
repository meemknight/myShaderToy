void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    fragColor = vec4(texture(iChannel0, fragCoord/iResolution.rg).rgb,1.0);
}