#version 430 core
out vec4 FragColor;

// in vec2 TexCoord;
// in vec3 Normal;
// in vec3 FragPos;
// in vec4 FragPosLightSpace;
in vec4 vs_color;

void main()
{
    // vec3 norm = normalize(Normal);
    // vec3 viewDir = normalize(viewPos - FragPos);
    // float shadow = ShadowCalculation(norm, FragPosLightSpace);
    
    // vec3 result = CalcDirLight(dirLight, norm, viewDir, shadow);
    // result += CalcPointLight(pointLight, norm, FragPos, viewDir, shadow);

    FragColor = vec4(vec3(vs_color.xyz), 1.0);

    // vec3 fColor = max(vec3(0), FragColor.xyz - 0.004);
    // vec3 temp1 = fColor * (fColor * 6.2 + 0.5);
    // vec3 temp2 = fColor * (fColor * 6.2 + 1.7) + 0.06;
    // FragColor = vec4(temp1 / temp2, 1.0); 
}
