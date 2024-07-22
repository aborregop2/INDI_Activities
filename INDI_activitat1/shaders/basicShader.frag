#version 330 core

out vec4 FragColor;
in vec4 vertex_color;

//uniform vec3 forats[3];

in vec3 fragpos;
uniform bool esNuvol;
uniform vec3 nuvolPoints[5];
uniform bool esBase;



void main() {
    
    if (!esNuvol && !esBase) {
        FragColor = vertex_color; 
        return;
    }
    else if (esNuvol) {
        bool dins = false;
    
        for (int i = 0; i < 5; ++i) {
            float distance = length(fragpos - nuvolPoints[i]); 
            if (distance < 0.08) {
                FragColor = vertex_color;
                return;
            }
        }

        discard;
    }else {
        if ((int(gl_FragCoord.x) % 8 < 4 && int(gl_FragCoord.y) % 8 < 4)) discard;
        else FragColor = vertex_color;
        
    }
}

