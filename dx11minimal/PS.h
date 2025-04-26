cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer params : register(b1)
{
    float r, g, b;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL1;
    float4 wnorm : NORMAL2;
    float4 bnorm : NORMAL3;
    float2 uv : TEXCOORD0;
};

float nrand(float2 n)
{
    return frac(sin(dot(n.xy, float2(19.9898, 78.233))) * 43758.5453);
}

float sincosbundle(float val)
{
    return sin(cos(2. * val) + sin(4. * val) - cos(5. * val) + sin(3. * val)) * 0.05;
}

float3 color(float2 uv)
{
    float2 coord = floor(uv);
    float2 gv = frac(uv);

    float movingValue = -sincosbundle(coord.y) * 2.;

    float offset = floor(fmod(uv.y, 2.0)) * (1.5);
    float verticalEdge = abs(cos(uv.x + offset));

    float3 brick = float3(0.45, 0.29, 0.23) - movingValue;

    bool vrtEdge = step(1. - 0.01, verticalEdge) == 1.0;
    bool hrtEdge = gv.y > (0.9) || gv.y < (0.1);

    if (hrtEdge || vrtEdge)
        return float3(0.845, 0.845, 0.845);

    return brick;
}

float lum(float2 uv)
{
    float3 rgb = color(uv);
    return 0.2126 * rgb.r + 0.7152 * rgb.g + 0.0722 * rgb.b;
}


float3 normal(float2 uv)
{
    float r = 0.03;

    float x0 = lum(uv + float2(r, 0.0));
    float x1 = lum(uv - float2(r, 0.0));
    float y0 = lum(uv + float2(0.0, r));
    float y1 = lum(uv - float2(0.0, r));

    float s = 1.0;
    float3 n = normalize(float3(x1 - x0, y1 - y0, s));

    float3 p = float3(uv * 2.0 - 1.0, 0.0);
    float3 v = float3(0.0, 0.0, 1.0);

    float3 l = v - p;
    float d_sqr = dot(l, l);
    l *= (1.0 / sqrt(d_sqr));

    float3 h = normalize(l + v);

    float dot_nl = clamp(dot(n, l), 0.0, 1.0);
    float dot_nh = clamp(dot(n, h), 0.0, 1.0);

    float color = lum(uv) * pow(dot_nh, 14.0) * dot_nl * (1.0 / d_sqr);
    color = pow(color, 1.0 / 2.2);

    return (n * 0.5 + 0.5);
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 posLight = float3(1, 0, 0);
    float3 nrml = input.vnorm.xyz;
   
    float3 pos = input.wpos.xyz;
    float3 cameraPos = float3(0, 0, -1);
    
    float3 L = normalize(posLight - pos);
    float3 V = normalize(cameraPos - pos);
    float3 H = normalize(L + V);

    float NL = saturate(dot(nrml, L));
    float NH = saturate(dot(nrml, H));

    float gloss = 106;
    float k = 1;

    float diffuse = NL;
    float lightBlinnPhong = pow(NH, gloss) * k;
    float ambient = 0.1;
    float lighting = lightBlinnPhong;


    return float4(lighting, lighting, lighting, 1.0);
    
 //   float3 pos = input.wpos.xyz;
    //float3 cameraPos = float3(3.5, 0, 0);
    //float3 cameraPos = view[1][3].xyz;
   // float3 cameraPos = float3(0,0,1);


  //  float3 lightPos = -normalize(float3(0, 1, 0));

 /*   float3 L = normalize(lightPos - pos);
    float3 V = normalize(cameraPos - pos);
    float3 H = normalize(L + V);
*/
 //   float NL = saturate(dot(finalNormal, L));
 //   float NH = saturate(dot(finalNormal, H));

 //   float ambient = 0.;
 //   float diffuse = NL;
 //   float specular = pow(NH, 64.0) * 8.0;

 //   float lighting = ambient + diffuse + specular;

    //baseColor = 1;
    // Итоговый цвет
 //   float3 fragColor = baseColor * lighting;

 //   return float4(fragColor, 1.0);
 //   return float4(finalNormal / 2 + .5, 1.0);
}