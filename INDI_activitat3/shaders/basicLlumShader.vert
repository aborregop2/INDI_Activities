#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

//
out vec3 LSCO;
out vec3 normalSCO;
out vec4 vertexSCO;
out vec3 vertexf;
out vec3 lightFar1;
out vec3 lightFar2;
out vec3 lightTorxes[6];

out vec3 matambfrag;
out vec3 matdifffrag;
out vec3 matspecfrag;
out float matshinfrag;


uniform vec4 lightPosSCO;
uniform mat3 NormalMatrix;
uniform vec4 lightPosFar1;
uniform vec4 lightPosFar2;
uniform vec4 torxesPos[6];

//


uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

out vec3 fcolor;
//******************************************************************

//******************************************************************
void main()
{
    vertexSCO = view * TG * vec4(vertex, 1);
    vertexf = vertex;

    LSCO = lightPosSCO.xyz - vertexSCO.xyz;
    lightFar1 = lightPosFar1.xyz - vertexSCO.xyz;
    lightFar2 = lightPosFar2.xyz - vertexSCO.xyz;

    lightTorxes[0] = torxesPos[0].xyz - vertexSCO.xyz;
    lightTorxes[1] = torxesPos[1].xyz - vertexSCO.xyz;
    lightTorxes[2] = torxesPos[2].xyz - vertexSCO.xyz;
    lightTorxes[3] = torxesPos[3].xyz - vertexSCO.xyz;
    lightTorxes[4] = torxesPos[4].xyz - vertexSCO.xyz;
    lightTorxes[5] = torxesPos[5].xyz - vertexSCO.xyz;


    normalSCO = normalize(NormalMatrix * normal);

    matambfrag = matamb;
    matdifffrag = matdiff;
    matspecfrag = matspec;
    matshinfrag = matshin;


    gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}

