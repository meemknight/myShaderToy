precision mediump float;

vec3 Blue = vec3(0.6, 0.67, 0.9);
vec3 Grey = vec3(0.7);
vec3 Red = vec3(1.0, 0.0, 0.0);
vec3 White = vec3(1.0);
    
struct Hit
{
    int ObjectId;
    float Distance;
    vec3 Normals;
    vec3 P;
};
struct Material
{
    int Color;
    float Refraction;
    float Reflection;
};

struct Object
{
    vec3 Position;
    Material Material;
};

Object[4] g_Objects;

void SetupObjects()
{
    // Wood Sphere
    g_Objects[0].Position = vec3(-1.75, 0.75 * (sin(iTime) + 1.5), 1.5);
    g_Objects[0].Material.Refraction = 0.0;
    g_Objects[0].Material.Reflection = 10.;
    g_Objects[0].Material.Color = 0;
    
    // Marble Sphere
    g_Objects[1].Position = vec3(1.5,0.75 * (cos(iTime) + 1.5), 1.3);
    g_Objects[1].Material.Refraction = 0.0;
    g_Objects[1].Material.Reflection = 2.5;
    g_Objects[1].Material.Color = 1;
    
    // Translucent Sphere
    g_Objects[2].Position = vec3(1.75, 0.75 * (sin(iTime) + 1.5), -0.5);
    g_Objects[2].Material.Refraction = 0.1;
    g_Objects[2].Material.Reflection = 0.0;
    g_Objects[2].Material.Color = 2;
    
    
    // Plane
    g_Objects[3].Position = vec3(0.0, -.5, 0.0);
    g_Objects[3].Material.Refraction = 0.0;
    g_Objects[3].Material.Reflection = 1.5;
    g_Objects[3].Material.Color = 3;
    
}


vec2 CenterUVs(vec2 fragCoord)
{
    //Normalized device coordinates and aspect correction   
    vec2 uv = fragCoord.xy / iResolution.xy;   
    uv = uv * 2.0 - 1.0; // remap range from 0...1 to -1...1
    float aspectRatio = iResolution.x/ iResolution.y;
    
    uv.x *= aspectRatio; //aspect correction
    return uv;
}

struct Sphere
{
    vec3 Position;
    float Radius;
};

bool RaySphere(in Sphere sphere ,in vec3 origin, in vec3 direction, inout Hit hit)
{
    float t1 = dot(sphere.Position - origin, direction);
    vec3 K = origin + t1 * direction;
    float l1 = length(K - sphere.Position);
    float l2 = sqrt(sphere.Radius * sphere.Radius - l1 * l1);
    float t = t1 - l2;
    vec3 P = origin + t * direction;
    
    if(l1 < sphere.Radius && t > 0.01)
    {
        hit.Normals = normalize(P - sphere.Position);
        hit.Distance = t;
        hit.P = P;
        return true;
    }
    
    return false;
}

bool RayPlane(in vec3 position, in vec3 origin, in vec3 direction, inout Hit hit)
{
    float t = (position.y - origin.y) / direction.y;
    if(t > 0.01)
    {
        hit.Distance = t;
        hit.Normals = vec3(0.0, 1.0, 0.0);
        hit.P = origin + direction * t;
        
        return true;
    }
    
    return false;
}

bool RayCast(in vec3 origin, in vec3 direction, inout Hit hit)
{
    direction = normalize(direction);
    float nearestT = 999999999.0;
    bool hitted = false;
    
    // Sphere intersection check
    Hit sphereHit;
    for(int i = 0; i < 3; i++)
    {
        Sphere sphere;
        sphere.Position = g_Objects[i].Position;
        sphere.Radius = 1.0;
        
        
        if(RaySphere(sphere, origin, direction, sphereHit))
        {
            if(sphereHit.Distance < nearestT)
            {
                hit = sphereHit;
                hit.ObjectId = i;
                nearestT = hit.Distance;
            }
            hitted = true;
        }
        
    }
    
    // Plane intersection check
    vec3 plane = g_Objects[3].Position;
    Hit planeHit;
    if(RayPlane(plane, origin, direction, planeHit))
    {
        if(planeHit.Distance < nearestT)
        {
            hit = planeHit;
            hit.ObjectId = 3;
            nearestT = planeHit.Distance;
        }
        hitted = true;
    }
    
    return hitted;
}

vec3 Chessboard(in vec3 p)
{
    const float SIZE = 0.5;
    bool inx = mod(p.x*SIZE, 1.0) > 0.5;
    bool inz = mod(p.z*SIZE, 1.0) > 0.5;
    bool ing = inx ^^ inz;
    
    return vec3(1.0) * (ing ? 1.0 : 0.0);
}

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}

vec3 SkyAndClouds(vec3 dir, in float time)
{
    dir *= 10.0;
    const int octaves = 8;
    float value = 0.1;
    float amplitude = 0.25;
    //
    // Loop of octaves
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise(dir.yz + time * 0.5);
        dir *= 2.;
        amplitude *= .5;
    }
    return mix(White, Blue, value);
}
// hash functions from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

vec2 hash( vec2 p ){
	p = vec2( dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3)));
	return fract(sin(p)*43758.5453);
}

float voronoi( in vec2 x ){
	vec2 n = floor( x );
	vec2 f = fract( x );
	float t = 5000.0;
	float F1 = 8.0;
	float F2 = 8.0;
	float distance_type	= mod(t / 16.0,4.0);
	for( int j=-1; j<=1; j++ )
		for( int i=-1; i<=1; i++ ){
			vec2 g = vec2(i,j);
			vec2 o = hash( n + g );

			o = 0.5 + 0.41*sin( t + 6.2831*o );	
			vec2 r = g - f + o;

		float d = 	distance_type < 1.0 ? dot(r,r)  :				// euclidean^2
				  	distance_type < 2.0 ? sqrt(dot(r,r)) :			// euclidean
					distance_type < 3.0 ? abs(r.x) + abs(r.y) :		// manhattan
					distance_type < 4.0 ? max(abs(r.x), abs(r.y)) :	// chebyshev
					0.0;

		if( d<F1 ) { 
			F2 = F1; 
			F1 = d; 
		} else if( d<F2 ) {
			F2 = d;
		}
    }
	float function 			= mod(t,4.0);
	float c = function < 1.0 ? F1 : 
			  function < 2.0 ? F2 : 
			  function < 3.0 ? F2-F1 :
			  function < 4.0 ? (F1+F2)/2.0 : 
			  0.0;
    bool  multiply_by_F1	= mod(t,8.0)  >= 4.0;
    bool  inverse				= mod(t,16.0) >= 8.0;
	if( multiply_by_F1 )	c *= F1;
	if( inverse )			c = 1.0 - c;
	
    return c;
}

float fbm( in vec2 p, int octaves){
	float s = 0.0;
	float m = 0.0;
	float a = 0.5;
	for( int i=0; i<octaves; i++ ){
		s += a * voronoi(p);
		m += a;
		a *= 0.5;
		p *= 2.0;
	}
	return s/m;
}

// Wood texture generator from https://www.shadertoy.com/view/dsdGWM
// value noise
// Inigo Quilez (MIT License)
// https://www.shadertoy.com/view/lsf3WH
float hash1(in vec2 p) {
    ivec2 texp = ivec2(
        int(mod(p.x, 256.)),
        int(mod(p.y, 256.))
    );
    // return number between -1 and 1
    return -1.0 + 2.0*texelFetch(iChannel0, texp, 0).x;
}
vec2 hash2(in vec2 p)
{
    // return numbers between -1 and 1
    return vec2(hash1(p), hash1(p + vec2(32., 18.)));
}

float noise1(in vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
	
	vec2 u = f*f*(3.0 - 2.0*f);

    return mix(mix(hash1(i + vec2(0.0, 0.0)), 
                   hash1(i + vec2(1.0, 0.0)), u.x),
               mix(hash1(i + vec2(0.0, 1.0)), 
                   hash1(i + vec2(1.0, 1.0)), u.x), u.y);
}
float noise2(in vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    #if 1
    // quintic smoothstep
    vec2 u = f*f*f*(f*(f*6.0-15.0)+10.0);
    #else
    // cubic smoothstep
    vec2 u = f*f*(3.0-2.0*f);
    #endif    

    return mix(mix(dot(hash2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)), 
                   dot(hash2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
               mix(dot(hash2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)), 
                   dot(hash2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

// simplex noise
// Inigo Quilez (MIT License)
// https://www.shadertoy.com/view/Msf3WH
float noise3(in vec2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor(p + (p.x+p.y)*K1);
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max(0.5-vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
	vec3  n = h*h*h*h*vec3(dot(a, hash2(i+0.0)), dot(b, hash2(i+o)), dot(c, hash2(i+1.0)));
    return dot(n, vec3(70.0));
}

vec3 stripescol1(in float f)
{
    return .47 + .4 * sin(1.3*f*f + vec3(2.3) + vec3(0., .55, 1.4));
}
float treerings1(in vec2 p, in float a, in float b, in float c, in float d)
{
    float i = floor(p.x);
    float x = fract(p.x) - 0.5;
    
    float n1 = 0.5*noise1(5.*vec2(x, p.y) + vec2(10.*i, 0)) + 0.5;
    float n2 = 0.5*noise1(0.4*vec2(0, p.y) + vec2(0, 5.*i)) + 0.5;
    float f = .5 + 0.5*cos(a*x*x + b*n1 + c*p.y + d*n2);
    f *= f; 
    return f;
}
float fbm2(in vec2 p, in int octaves)
{
    // rotation matrix for fbm
    mat2 m = 2.*mat2(4./5., 3./5., -3./5., 4./5.);  
     
    float scale = 0.5;
    float f = scale * noise2(p);
    float norm = scale;
    for (int i = 0; i < octaves; i++) {
        p = m * p;
        scale *= .5;
        norm += scale;
        f += scale * noise2(p);
    }
	return 0.5 + 0.5 * f/norm;
}
float fbm3(in vec2 p, in int octaves)
{
    // rotation matrix for fbm
    mat2 m = 2.*mat2(4./5., 3./5., -3./5., 4./5.);  
     
    float scale = 0.5;
    float f = scale * noise3(p);
    float norm = scale;
    for (int i = 0; i < octaves; i++) {
        p = m * p;
        scale *= .5;
        norm += scale;
        f += scale * noise3(p);
    }
	return 0.5 + 0.5 * f/norm;
}
float finegrain(in vec2 uv, in float tr)
{
    vec2 p = 3.*vec2(50., 4.)*uv;
    float f = fbm3(0.5*p, 4) - 0.2*tr;
    return 1. - .4*f*(1. - smoothstep(.35, .45, f));
}
vec3 discoloration(in vec2 uv)
{
    float i = floor(uv.x); // panel index
    vec2 p = .2*vec2(2., 1.)*uv + i * vec2(234., 123.);
    float f = fbm2(p, 2);
    return .5 + .5 * sin(2.3*f + vec3(1.4) + vec3(0., .4, 1.));
}

vec3 Wood(in vec2 p, in float lightness, in float discolor)
{
    float tr = treerings1(p, 160., 1.5, 10., 3.);
    vec3 col = stripescol1(0.3 + 0.3*tr);
    col *= finegrain(p, tr);
    col *= mix(vec3(lightness), discoloration(p), discolor);
    //col *= panelgap(p);
    return col;
}

vec3 RenderColor(in vec3 origin, in vec3 dir, inout Hit hit)
{
    // Sky
    vec3 color = SkyAndClouds(dir, iTime);
    color = mix(Blue, color, dir.y * 2.0);
    color = mix(White, color, dir.y * 5.0);
   
    
    vec3 lightDir = vec3(0.0, 1.0, 0.0);
    vec3 lightColor = vec3(1.0);
    vec3 lightPos = vec3(10.0);
    
    // Raycasting
    if(RayCast(origin, dir, hit))
    {
        vec3 colors[4];
        colors[0] = Wood(hit.Normals.xz, 1.2, 0.5);
        colors[1] = vec3(fbm(hit.Normals.xz * 5.0 + 10.0, 1));
        colors[2] = White;
        colors[3] = Chessboard(hit.P);
    
        vec3 objectColor = colors[g_Objects[hit.ObjectId].Material.Color];
        // Light calculations
        vec3 ambient = lightColor * vec3(0.1);
        
        // diffuse
        vec3 light_dir = normalize(lightPos - hit.P);
        float diff = max(dot(hit.Normals, light_dir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // specular
        float specularStrenght = 0.5;
        vec3 reflectDir = reflect(-light_dir, hit.Normals);
        float spec = pow(max(dot(-dir, reflectDir), 0.0),64.0);
        vec3 specular = specularStrenght * spec * lightColor;
         
         // Shadows
         float shadowFactor = 0.0;
         if(RayCast(hit.P + (hit.Normals * 0.00001), lightDir, hit))
         {
             shadowFactor = 1.0;
         }
         
         color = (ambient + diffuse + specular) * objectColor  * (1.0 - shadowFactor);
    }
    return color;
}

vec3 RenderReflection(in vec3 origin, in vec3 dir, inout Hit hit)
{
    if(RayCast(origin, dir, hit))
    {
        vec3 rOrigin = hit.P + 0.0001 * hit.Normals;
        vec3 rDir = reflect(dir, hit.Normals);
        
        if(g_Objects[hit.ObjectId].Material.Refraction > 0.0)
        {
            rDir = refract(dir, hit.Normals, g_Objects[hit.ObjectId].Material.Refraction);
        }
        return RenderColor(rOrigin, rDir, hit);
    }
    return vec3(0.0);
}

vec3 Fresnel(in vec3 origin, in vec3 dir, inout Hit hit)
{
    if(RayCast(origin, dir, hit))
    {  
        float exponent = g_Objects[hit.ObjectId].Material.Reflection;
        float bias = 0.01;
        float scale = 1.0;
        
        float fresnel = bias + scale * pow(1.0 + dot(dir, hit.Normals), exponent);
        
        return vec3(fresnel);
    }
    
    return vec3(0.0);
}



void mainImage(out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = CenterUVs(fragCoord);
    vec3 origin = vec3(0.0, 0.0, 5.0);
    vec3 dir = vec3(uv.x, uv.y, 0.0) - origin;
    
    float aspectRatio = iResolution.x/ iResolution.y;
    
    //Extracted from https://www.shadertoy.com/view/4stGzX
    //Mouse values for navigation or other shenanigans. Normalized device coords and aspect correction to match UVs
    vec2 daMouse = iMouse.xy/ iResolution.xy;
    daMouse = daMouse * 2.0 - 1.0;
    daMouse.x *= aspectRatio;
   
    // camera controls (horizontal mouse = rotate, vertical mouse = elevation)
    vec3 camControls;
    camControls.x = sin(daMouse.x * 2.0) * 5.0;
    camControls.y = (daMouse.y * 0.5 + 0.5) * 9.0;
    camControls.z = cos(daMouse.x * 2.0) * 5.0;
    
    //mapping camera to UV cordinates
    vec3 cameraOrigin = vec3(camControls); //cam controls
    vec3 cameraTarget = vec3(0.0, 0.0, 0.0);
    vec3 upVector = vec3(0.0, 1.0, 0.0);
    vec3 cameraDirection = normalize(cameraTarget - cameraOrigin);
    vec3 cameraRight = normalize(cross(upVector, cameraOrigin));
    vec3 cameraUp = cross(cameraDirection, -cameraRight); //negate cameraRight to flip properly?
   
   
    SetupObjects();
   
    vec3 color = vec3(0.0);
    // Anti-aliasing
    Hit hit;
    const int antialasingSamples = 4;
    for(int i = 0; i < antialasingSamples; i++)
    {
        for(int j = 0; j < antialasingSamples; j++)
        {
            vec2 offset = vec2(float(i) + 0.5 / float(antialasingSamples), float(j) + 0.5 / float(antialasingSamples)) * (1.0 / iResolution.xy);
            vec2 uvAA = uv + offset;
            
            vec3 rayDir = normalize(cameraRight * uvAA.x + cameraUp * uvAA.y + cameraDirection);
            
            
            vec3 reflectColor = RenderReflection(cameraOrigin, rayDir, hit);
            vec3 sceneColor = RenderColor(cameraOrigin, rayDir, hit);
            vec3 fresnel = Fresnel(cameraOrigin, rayDir, hit);

            color += mix(sceneColor, reflectColor, fresnel);
        }
    }
    // Average of AA
    color /= float(antialasingSamples * antialasingSamples);

    // Fog
    vec3 fogColor = vec3(1.0);
	float fog = exp(-0.001 * hit.Distance * hit.Distance); // exponentioal fog equation
    color = mix(fogColor, color,fog);
    
    // Output to screen
    fragColor = vec4(color, 1.0);
}