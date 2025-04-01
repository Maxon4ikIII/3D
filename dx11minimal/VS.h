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

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    uint faceID : COLOR0;
};

// ������� ������� ��� ���������
static const float tau = 1.61803398875;

// �������� 12 ������ ��������� (�� ���������)
static const float3 icoVertices[12] = {
    { -1,  tau,  0 }, {  1,  tau,  0 }, { -1, -tau,  0 }, {  1, -tau,  0 },
    {  0, -1,  tau }, {  0,  1,  tau }, {  0, -1, -tau }, {  0,  1, -tau },
    {  tau,  0, -1 }, {  tau,  0,  1 }, { -tau,  0, -1 }, { -tau,  0,  1 }
};

// �������� 20 �������������
static const int3 icoIndices[20] = {
    {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};

// ������� �������� �� �����
float3 normalizeToSphere(float3 p)
{
    return normalize(p);
}

// ������� ��������� ������������� (������ ����� �� 4)
float3 getSubdividedVertex(int triID, int vertID)
{
    int3 tri = icoIndices[triID];

    float3 v0 = icoVertices[tri[0]];
    float3 v1 = icoVertices[tri[1]];
    float3 v2 = icoVertices[tri[2]];

    // ������� ����� ������� ����� ���������
    float3 mid01 = normalizeToSphere((v0 + v1) * 0.5);
    float3 mid12 = normalizeToSphere((v1 + v2) * 0.5);
    float3 mid20 = normalizeToSphere((v2 + v0) * 0.5);

    // ������ ������ ����� ��������� (4 ����� ������������)
    static const int newTris[4][3] = {
        {0,  3,  5}, // ����������� 1
        {3,  1,  4}, // ����������� 2
        {5,  4,  2}, // ����������� 3
        {3,  4,  5}  // ����������� 4 (�����������)
    };

    // �������� ������� ������ ���������
    int localTri = vertID / 3;
    int localVert = vertID % 3;

    int idx = newTris[localTri][localVert];

    if (idx == 0) return normalizeToSphere(v0);
    if (idx == 1) return normalizeToSphere(v1);
    if (idx == 2) return normalizeToSphere(v2);
    if (idx == 3) return normalizeToSphere(mid01);
    if (idx == 4) return normalizeToSphere(mid12);
    if (idx == 5) return normalizeToSphere(mid20);

    return float3(0, 0, 0); // ��������
}

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    int triID = vID / 12;  // ����� ��������� ������������
    int vertID = vID % 12; // ������� ������ ������������

    float3 p = getSubdividedVertex(triID, vertID);

    output.pos = mul(float4(p, 1.0f), mul(view[0], proj[0]));
    output.normal = normalize(p);
    output.uv = float2(atan2(p.x, p.z) / (2 * 3.1415) + 0.5, p.y * 0.5 + 0.5);
    output.faceID = triID; // ��� �����

    return output;
}
