#include <Shared.fxh>

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 vNormal = normalize(input.Norm);
    float3 vTangent = normalize(input.TangentWorld);
    float3 vBiTanget = cross(vNormal, vTangent);

    // ≥Î∏ª
    float3 vNormalTangentSpace = txNormal.Sample(samLinear, input.TexCoord).rgb * 2.0f - 1.0f;
    float3x3 WorldTransform = float3x3(vTangent, vBiTanget, vNormal);
    if (UsingNormalMap)
    {
        vNormal = mul(vNormalTangentSpace, WorldTransform);
        vNormal = normalize(vNormal);
    }

    // diffuse
    float4 texDiffuse = (float4)1;
    if (UsingDiffuseMap)
    {
        texDiffuse = txDiffuse.Sample(samLinear, input.TexCoord);
    }

    // emmisive
    float4 emmisive = 0;
    if (UsingEmissiveMap)
    {
        emmisive = txEmissive.Sample(samLinear, input.TexCoord);
    }

    // opacity
    float opacity = 1.0f;
    if (UsingOpacityMap)
    {
        opacity = txOpacity.Sample(samLinear, input.TexCoord).a;
    }

    // Ω∫∆Â≈ß∑Ø
    float NdotL = max(dot(vNormal, -LightDirection), 0);
    float3 vView = normalize(EyePosition - input.WorldPos.xyz);
    
    float4 ambient = LightAmbient * MaterialAmbient;
    
    float4 diffuse = LightDiffuse * MaterialDiffuse * texDiffuse * NdotL;

    float3 vHalf = normalize(-LightDirection + vView);
    float SDot = max(dot(vNormal, vHalf), 0);

    float power = pow(SDot, MaterialSpecularPower);

    float4 specular = power * LightSpecular * MaterialSpecular;
    if (UsingSpecularMap)
    {
        specular *= txSpecular.Sample(samLinear, input.TexCoord);
    }

    float4 finalColor = diffuse + specular + ambient + emmisive;

    return float4(finalColor.xyz, opacity);
}