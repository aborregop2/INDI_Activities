#version 330 core

in vec3 fcolor; 
out vec4 FragColor;

//
in vec3 LSCO;
in vec3 normalSCO;
in vec4 vertexSCO;
in vec3 vertexf;
in vec3 lightFar1;
in vec3 lightFar2;
in vec3 lightTorxes[6];

in vec3 matambfrag;
in vec3 matdifffrag;
in vec3 matspecfrag;
in float matshinfrag;

uniform vec3 lightAmbient;
uniform vec3 lightColor;
uniform vec4 lightPosFar1;
uniform vec4 lightPosFar2;
uniform vec3 torxesColor;
uniform bool torxesEnceses[6];
uniform bool wavePainting;
uniform mat3 NormalMatrix;
uniform float time;


//

uniform mat4 view;
uniform mat4 TG;




vec3 Ambient(vec3 llumAmbient) {
    return llumAmbient * matambfrag;
}

vec3 Difus (vec3 NormSCO, vec3 L, vec3 colFocus) 
{
    // Tant sols retorna el terme difús
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
    vec3 colRes = vec3(0);
    // Càlcul component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colFocus * matdifffrag * dot (L, NormSCO);
    return (colRes);
}

vec3 Especular (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 colFocus) 
{
    // Tant sols retorna el terme especular!
    // Els vectors rebuts com a paràmetres (NormSCO i L) estan normalitzats
    vec3 colRes = vec3 (0);
    // Si la llum ve de darrera o el material és mate no fem res
    if ((dot(NormSCO,L) < 0) || (matshinfrag == 0))
      return colRes;  // no hi ha component especular

    // Calculem R i V
    vec3 R = reflect(-L, NormSCO); // equival a: 2.0*dot(NormSCO,L)*NormSCO - L;
    vec3 V = normalize(-vertSCO.xyz); // perquè la càmera està a (0,0,0) en SCO

    if (dot(R, V) < 0)
      return colRes;  // no hi ha component especular
    
    // Calculem i retornem la component especular
    float shine = pow(max(0.0, dot(R, V)), matshinfrag);
    return (matspecfrag * colFocus * shine); 
}


vec3 WaveNormal(vec2 waveDirNN, float waveLength, vec3 vertex_world, float time){
        float steepness = .5;
	vec2 waveDir = normalize(waveDirNN);
	float wl = 2;
        float f = (2 * 3.14159/waveLength) * (dot(waveDir, vertex_world.xz) - 0.25 * time);

	vec3 B = vec3( 
		     1- (steepness*waveDir.x*waveDir.x * cos(f)) , 
		      waveDir.x * sin(f),
		     -steepness*waveDir.x*waveDir.y * cos(f)
			);
	vec3 T = vec3( 
		     -steepness*waveDir.x*waveDir.y * cos(f) , 
		      waveDir.y * sin(f),
		     1-steepness*waveDir.y*waveDir.y * cos(f)
			);	

	return normalize(cross(normalize(T), normalize(B)));
}



void main()
{
  vec3 amb = Ambient(lightAmbient);
  vec3 dif = Difus(normalSCO, normalize(LSCO), lightColor) 
       + Difus(normalSCO, normalize(lightFar1), lightColor) * pow(dot(normalize(lightPosFar1 - lightPosFar2).xyz, normalize(-lightFar1)), 4)
       + Difus(normalSCO, normalize(lightFar2), lightColor) * pow(dot(normalize(lightPosFar2 - lightPosFar1).xyz, normalize(-lightFar2)), 4);
  vec3 esp = Especular(normalSCO, normalize(LSCO), vertexSCO, lightColor) + 
         Especular(normalSCO, normalize(lightFar1), vertexSCO, lightColor) * pow(dot(normalize(lightPosFar1 - lightPosFar2).xyz, normalize(-lightFar1)), 4)
       + Especular(normalSCO, normalize(lightFar2), vertexSCO, lightColor) * pow(dot(normalize(lightPosFar2 - lightPosFar1).xyz, normalize(-lightFar2)), 4);

  for(int i = 0; i < 6; i++) {
    float d = length(lightTorxes[i]);
    float factor = d > 3.0 && torxesEnceses[i] ? exp(-(d - 3.0)) : 1.0;

    if (torxesEnceses[i]) dif += Difus(normalSCO, normalize(lightTorxes[i]), torxesColor) * factor;
    if (torxesEnceses[i]) esp += Especular(normalSCO, normalize(lightTorxes[i]), vertexSCO, torxesColor) * factor;
  }
  
  if (wavePainting) {
    vec3 waveNorm1 = WaveNormal(vec2(1, 1), 0.2, vertexf.xyz, time);
    vec3 waveNorm2 = WaveNormal(vec2(1, 1), 0.4, vertexf.xyz, time);
    vec3 waveNorm3 = WaveNormal(vec2(1, 1), 0.1, vertexf.xyz, time);

    vec3 normalw = waveNorm1 + waveNorm2 + waveNorm3;

    vec4 waveNormView1 = normalw.y > 0 ? normalize((view * vec4(normalw,0))) : normalize(vec4(normalw, 0));

    vec3 waveEsp1 = Especular(waveNormView1.xyz, normalize(LSCO), vertexSCO, lightColor);

    vec3 waveDif1 = Difus(waveNormView1.xyz, normalize(LSCO), lightColor);

    dif += waveDif1;
    esp += waveEsp1;
  }


  FragColor = vec4((amb + dif + esp), 1);
}


