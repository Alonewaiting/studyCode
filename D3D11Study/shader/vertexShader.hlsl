Texture2D colorMap : register(t0);
SamplerState colorMapSampler_ : register(s0);
struct VS_Input
{
    float4 pos : POSITION;
    float2 tex0 : TEXCOORD0;
};
struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
};
PS_Input VSMain(VS_Input vertex)
{
    PS_Input vsOut = (PS_Input)0;
    vsOut.pos = vertex.pos;
    vsOut.tex0 = vertex.tex0;
    return vsOut;
}