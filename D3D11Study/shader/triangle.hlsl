struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
};
// ������ɫ��
VertexOut VS_Main(VertexIn vIn)
{
    VertexOut vOut;
    vOut.posH = float4(vIn.pos, 1.0f);
    vOut.color = vIn.color; // ����alphaͨ����ֵĬ��Ϊ1.0
    return vOut;
}

// ������ɫ��
float4 PS_Main(VertexOut pIn) : SV_Target
{
    return pIn.color;
}