float4 PS_Main(PS_Input frag) : SV_TARGET
{
    return colorMap_.Sample( colorSampler_, frag.tex0 );
}