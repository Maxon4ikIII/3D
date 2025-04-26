cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
    float2 iResolution;
    float2 pad;
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

cbuffer objParams : register(b0)
{
    float gx;
    float gy;
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

float3 rotY(float3 pos, float a)
{
    float3x3 m = float3x3(
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    );
    return mul(pos, m);
}

float3 rotX(float3 pos, float a)
{
    float3x3 m = float3x3(
        1, 0, 0,
        0, cos(a), -sin(a),
        0, sin(a), cos(a)
    );
    return mul(pos, m);
}

float3 rotZ(float3 pos, float a)
{
    float3x3 m = float3x3(
        cos(a), sin(a), 0,
        -sin(a), cos(a), 0,
        0, 0, 1
    );
    return mul(pos, m);
}

#define PI 3.1415926535897932384626433832795
float length(float3 c)
{
    float x = c.x;
    float y = c.y;
    float z = c.z;
    float l = sqrt(x * x + y * y + z * z);
    return l;
}

float3 calcGeom(float2 a)
{
    float R = 1.8;
    float r = .3;

    int p = 7;
    int q = 5;

    float3 pos = float3((R + cos(a.x * q)) * cos(a.x * p), -sin(a.x * q), (R + cos(a.x * q)) * sin(a.x * p));

    float delta = 0.0001;
    float2 a_dx = a + float2(delta, 0);
    float3 pos_dx = float3((R + cos(a_dx.x * q)) * cos(a_dx.x * p), -sin(a_dx.x * q), (R + cos(a_dx.x * q)) * sin(a_dx.x * p));
    float3 tangent = normalize(pos_dx - pos);

    float3 up = float3(0, 1, 0);
    float3 B = normalize(cross(tangent, up));
    float3 N = normalize(cross(B, tangent));

    pos += r * (cos(a.y) * N + sin(a.y) * B);


    pos = -pos.xzy;

    pos = rotY(pos * 0.9, time.x * 0.1);

    return pos;
}

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float2 quad[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float2 p = quad[vID % 6];
    int qID = vID / 6;

    //float x = (qID % (uint)gx + p.x * 0.5) / gx + 0.5;
    //float y = (qID / (uint)gy + p.y * 0.5) / gy + 0.5;

    float x = (p.x*.5 + qID % (uint)(gx))/(gx)-.5;
    float y = (p.y * .5 + qID / (uint)gx) / gy - .5;;

    

    float3 mPos = float3(cos(x*2*PI), y, sin(x*2*PI));

    mPos.xz *= cos(PI*y)/2;
    mPos.y = sin(PI * y)/2;

    float stepX = 1.0 / gx;
    float stepY = 1.0 / gy;

    float2 a = float2(x, y) * PI * 2.0;
    a.x *= -1.0;
    float2 a1 = a + float2(stepX * PI * 2.0, 0);
    float2 a2 = a + float2(0, stepY * PI * 2.0);
    float3 pos = calcGeom(a);
    float3 pos1 = calcGeom(a1);
    float3 pos2 = calcGeom(a2);
    float3 binormal = normalize(pos2 - pos);
    float3 tangent = normalize(pos1 - pos);
    float3 norm = normalize(cross(tangent, binormal));


    output.pos = mul(mul(float4(mPos, 1.0), view[0]), proj[0]);
    float2 uv = float2(x, y);
    output.uv = uv * float2(52, 6);
    output.vnorm = float4(norm, 1.0);
    output.bnorm = float4(binormal, 1.0);
    output.wnorm = float4(tangent, 1.0);

    output.vnorm = float4(normalize(mPos), 1.0);

    return output;
}