#include "A3GLWidget.h"

#include <iostream>

A3GLWidget::A3GLWidget (QWidget* parent) : MyGLWidget(parent)
{
}

A3GLWidget::~A3GLWidget()
{
}


void A3GLWidget::iniEscena ()
{
	creaBuffersModels();
	spinBoxChanged(hour);


	glm::vec3 ambient = glm::vec3(0.1, 0.1, 0.1);
	glUniform3fv(ambientLoc, 1, &ambient[0]);

	glm::vec3 lightColor = glm::vec3(0.6, 0.6, 0.6);
	glUniform3fv(colLlumLoc, 1, &lightColor[0]);

	VaixellPos = glm::vec3(-10, 0.0, 0.0);


	//--------------------------------------------------
	centreEsc = glm::vec3 (0, 0, 0);

	radiEsc = 20;// Tret de la màniga :-P

}


void A3GLWidget::paintGL ()
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif

	// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
	// useu els paràmetres que considereu (els que hi ha són els de per defecte)
	//  glViewport (0, 0, ample, alt);

	wavePainting = 0;
	glUniform1i(wavePaintingLoc, wavePainting);

	projectTransform ();
	viewTransform ();

	glm::vec4 lightPosSCO = View * glm::vec4(lightPos, 1);
	glUniform4fv(lightPosLoc, 1, &lightPosSCO[0]);

	// Esborrem el frame-buffer i el depth-buffer

	glClearColor(0.8f, 0.8f, 1.0f, 1.f);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//--------------------------------------------------------
	// Activem el VAO per a pintar el vaixell
	glBindVertexArray (VAO_models[VAIXELL]);
	// Transformació geometrica
	modelTransformVaixell();

	glm::mat3 NormalMatrix = glm::inverseTranspose(glm::mat3(View*Vaixell_TG));
	NormalMatrixLoc = glGetUniformLocation (program->programId(), "NormalMatrix");
	glUniformMatrix3fv(NormalMatrixLoc, 1, GL_FALSE, &NormalMatrix[0][0]);

	glm::vec4 torxa1 = View * Vaixell_TG * glm::vec4 (-7.39, 1.95,-6.68, 1);
	glm::vec4 torxa2 = View * Vaixell_TG * glm::vec4 (-9.95, 1.95,-0.56, 1);
	glm::vec4 torxa3 = View * Vaixell_TG * glm::vec4 (-7.47, 1.95, 5.64, 1);
	glm::vec4 torxa4 = View * Vaixell_TG * glm::vec4 (4.38, 1.95, 5.26, 1);
	glm::vec4 torxa5 = View * Vaixell_TG * glm::vec4 (6.68, 1.95, 0.38, 1);
	glm::vec4 torxa6 = View * Vaixell_TG * glm::vec4 (4.15, 1.95,-6.97, 1);
	glm::vec4 torxesPos[NUM_TORXES] = {torxa1, torxa2, torxa3, torxa4, torxa5, torxa6};
	glUniform4fv(torxesPosLoc, NUM_TORXES, &torxesPos[0][0]);


	glUniform1iv(torxesEncesesLoc, NUM_TORXES, torxesEnceses);

	glm::vec3 torxesColor = glm::vec3(red/255.0, green/255.0, blue/255.0);
	glUniform3fv(torxesColorLoc, 1, &torxesColor[0]);


	// pintem el vaixell
	glDrawArrays(GL_TRIANGLES, 0, models[VAIXELL].faces().size()*3);
	//--------------------------------------------------------
	// Activem el VAO per a pintar el moll
	glBindVertexArray (VAO_models[MOLL]);
	// pintem les espelmes, cadascuna amb el seu transform
	glm::mat4 TG = modelTransformMoll();

	NormalMatrix = glm::inverseTranspose(glm::mat3(View*TG));
	glUniformMatrix3fv(NormalMatrixLoc, 1, GL_FALSE, &NormalMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, models[MOLL].faces().size()*3);
	//--------------------------------------------------------
	// Activem el VAO per a pintar el far (primera part)
	glBindVertexArray (VAO_models[FAR_1]);
	// pintem les espelmes, cadascuna amb el seu transform
	TG = modelTransformFar1();

	NormalMatrix = glm::inverseTranspose(glm::mat3(View*TG));
	glUniformMatrix3fv(NormalMatrixLoc, 1, GL_FALSE, &NormalMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, models[FAR_1].faces().size()*3);
	//--------------------------------------------------------
	// Activem el VAO per a pintar el far (segona part)
	glBindVertexArray (VAO_models[FAR_2]);
	// pintem les espelmes, cadascuna amb el seu transform
	modelTransformFar2();

	glm::vec4 F1 = View * Far2_TG * glm::vec4 (0.363,4.695,0.357, 1);
	glUniform4fv(lightPosLocFar1, 1, &F1[0]);

	glm::vec4 F2 =  View * Far2_TG * glm::vec4 (-0.357,4.695,-0.348, 1);
	glUniform4fv(lightPosLocFar2, 1, &F2[0]);


	glDrawArrays(GL_TRIANGLES, 0, models[FAR_2].faces().size()*3);
	//------------------------------------------------------------
	//Pintem terra
	glBindVertexArray (VAO_Terra);
	TG = modelTransformTerra();

	NormalMatrix = glm::inverseTranspose(glm::mat3(View*TG));
	NormalMatrixLoc = glGetUniformLocation (program->programId(), "NormalMatrix");
	glUniformMatrix3fv(NormalMatrixLoc, 1, GL_FALSE, &NormalMatrix[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Pintem Mar
	glBindVertexArray (VAO_Mar);
	TG = modelTransformMar();

	NormalMatrix = glm::inverseTranspose(glm::mat3(View*TG));
	NormalMatrixLoc = glGetUniformLocation (program->programId(), "NormalMatrix");
	glUniformMatrix3fv(NormalMatrixLoc, 1, GL_FALSE, &NormalMatrix[0][0]);

	wavePainting = 1;
	glUniform1i(wavePaintingLoc, wavePainting);
	glUniform1f(timeLoc, temps);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	//--------------------------------------------------------

	glBindVertexArray(0);
}

glm::mat4 A3GLWidget::modelTransformFar1()
{
	// Codi per a la TG necessària
	glm::mat4 TG = glm::mat4(1.0f);
	TG = glm::translate(TG, glm::vec3(2, 1, 4));
	TG = glm::scale(TG, glm::vec3(escalaModels[FAR_1]) );
	TG = glm::translate(TG, -centreBaseModels[FAR_1]);
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);

	return TG;
}

void A3GLWidget::modelTransformFar2()
{
	// Codi per a la TG necessària
	Far2_TG = glm::mat4(1.0f);
	Far2_TG = glm::translate(Far2_TG, glm::vec3(2, 1, 4));
	Far2_TG = glm::rotate(Far2_TG, glm::radians(angleFar2), glm::vec3(0, 1, 0));
	Far2_TG = glm::scale(Far2_TG, glm::vec3(escalaModels[FAR_1]) );
	Far2_TG = glm::translate(Far2_TG, -centreBaseModels[FAR_1]);
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &Far2_TG[0][0]);
}


void A3GLWidget::modelTransformVaixell()
{
	// Codi per a la TG necessària
	Vaixell_TG = glm::mat4(1.0f);
	Vaixell_TG = glm::translate(Vaixell_TG, VaixellPos);
	Vaixell_TG = glm::scale(Vaixell_TG, glm::vec3(escalaModels[VAIXELL]) );
	Vaixell_TG = glm::translate(Vaixell_TG,
		                -glm::vec3(centreBaseModels[VAIXELL].x,
		                           0,
		                           centreBaseModels[VAIXELL].z)); // el vaixell ja esta a la línia de flotació....no toquem en alçada
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, &Vaixell_TG[0][0]);
}



void A3GLWidget::keyPressEvent(QKeyEvent* event)
{
	makeCurrent();
	
	switch (event->key()) {

		case Qt::Key_S: {
			VaixellPos[2]+=1;break;
		}
		case Qt::Key_W: {
			VaixellPos[2]-=1;break;
		}
		case Qt::Key_Up: {
			if (hour < HORA_MAX) {
				hour++;
				angleSol += 15;

				lightPos.x = 40 * cos(glm::radians(angleSol));
				lightPos.y = 40 * sin(glm::radians(angleSol));

				spinBoxChanged(hour);
			}
			
			break;
		}
		case Qt::Key_Down: {
			if (hour > HORA_MIN) {
				hour--;
				angleSol -= 15;

				lightPos.x = 40 * cos(glm::radians(angleSol));
				lightPos.y = 40 * sin(glm::radians(angleSol));

				spinBoxChanged(hour);
			}
			
			break;
		}
		case Qt::Key_A: {
			angleFar2 += 5;
			break;
		}
		case Qt::Key_D: {
			angleFar2 -= 5;
			break;
		}
		default: event->ignore(); break;
	}
	std::cout << hour << "i angle:" << angleSol << std::endl;
	update();
}


void A3GLWidget::carregaShaders()
{
	// Creem els shaders per al fragment shader i el vertex shader
	QOpenGLShader fs (QOpenGLShader::Fragment, this);
	QOpenGLShader vs (QOpenGLShader::Vertex, this);
	// Carreguem el codi dels fitxers i els compilem
	fs.compileSourceFile("./shaders/basicLlumShader.frag");
	vs.compileSourceFile("./shaders/basicLlumShader.vert");
	// Creem el program
	program = new QOpenGLShaderProgram(this);
	// Li afegim els shaders corresponents
	program->addShader(&fs);
	program->addShader(&vs);
	// Linkem el program
	program->link();
	// Indiquem que aquest és el program que volem usar
	program->bind();

	// Obtenim identificador per a l'atribut “vertex” del vertex shader
	vertexLoc = glGetAttribLocation (program->programId(), "vertex");
	vertexfLoc = glGetAttribLocation (program->programId(), "vertexf");
	// Obtenim identificador per a l'atribut “normal” del vertex shader
	normalLoc = glGetAttribLocation (program->programId(), "normal");
	// Obtenim identificador per a l'atribut “matamb” del vertex shader
	matambLoc = glGetAttribLocation (program->programId(), "matamb");
	// Obtenim identificador per a l'atribut “matdiff” del vertex shader
	matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
	// Obtenim identificador per a l'atribut “matspec” del vertex shader
	matspecLoc = glGetAttribLocation (program->programId(), "matspec");
	// Obtenim identificador per a l'atribut “matshin” del vertex shader
	matshinLoc = glGetAttribLocation (program->programId(), "matshin");

	// Demanem identificadors per als uniforms del vertex shader
	transLoc = glGetUniformLocation (program->programId(), "TG");
	projLoc = glGetUniformLocation (program->programId(), "proj");
	viewLoc = glGetUniformLocation (program->programId(), "view");

	// Bloc d'uniforms
	// .... Enjoy !

	
	ambientLoc = glGetUniformLocation (program->programId(), "lightAmbient");
	colLlumLoc = glGetUniformLocation (program->programId(), "lightColor");
	lightPosLoc = glGetUniformLocation (program->programId(), "lightPosSCO");
	lightPosLocFar1 = glGetUniformLocation (program->programId(), "lightPosFar1");
	lightPosLocFar2 = glGetUniformLocation (program->programId(), "lightPosFar2");
	torxesPosLoc = glGetUniformLocation (program->programId(), "torxesPos");
	torxesEncesesLoc = glGetUniformLocation (program->programId(), "torxesEnceses");

	
	torxesColorLoc = glGetUniformLocation (program->programId(), "torxesColor");
	wavePaintingLoc = glGetUniformLocation (program->programId(), "wavePainting");
	timeLoc = glGetUniformLocation (program->programId(), "time");


	// Bloc d'atributs
	// ... Oh yeah !!!
	LLoc = glGetAttribLocation (program->programId(), "LSCO");
	normalSCOLoc = glGetAttribLocation (program->programId(), "normalSCO");
	vertexSCOLoc = glGetAttribLocation (program->programId(), "vertexSCO");
	LightFar1Loc = glGetAttribLocation (program->programId(), "lightFar1");
	LightFar2Loc = glGetAttribLocation (program->programId(), "lightFar2");
	lightTorxesLoc = glGetAttribLocation (program->programId(), "lightTorxes");

	matambfragLoc = glGetAttribLocation (program->programId(), "matambfrag");
	matdifffragLoc = glGetAttribLocation (program->programId(), "matdifffrag");
	matspecfragLoc = glGetAttribLocation (program->programId(), "matspecfrag");
	matshinfragLoc = glGetAttribLocation (program->programId(), "matshinfrag");


	


}

void A3GLWidget::changeHour(int h)
{
	makeCurrent();

	hour = h;
	angleSol = 180 - 15 * (h - HORA_MIN);
	lightPos.x = 40 * cos(glm::radians(angleSol));
	lightPos.y = 40 * sin(glm::radians(angleSol));

	update();
}

void A3GLWidget::spinBoxChanged(int h)
{
	emit hourChanged(h);
}

void A3GLWidget::torxa1Changed(bool b)
{
	makeCurrent();
	torxesEnceses[0] = b;
	update();
}

void A3GLWidget::torxa2Changed(bool b)
{
	makeCurrent();
	torxesEnceses[1] = b;
	update();
}

void A3GLWidget::torxa3Changed(bool b)
{
	makeCurrent();
	torxesEnceses[2] = b;
	update();
}

void A3GLWidget::torxa4Changed(bool b)
{
	makeCurrent();
	torxesEnceses[3] = b;
	update();
}

void A3GLWidget::torxa5Changed(bool b)
{
	makeCurrent();
	torxesEnceses[4] = b;
	update();
}

void A3GLWidget::torxa6Changed(bool b)
{
	makeCurrent();
	torxesEnceses[5] = b;
	update();
}

void A3GLWidget::redChanged(int r)
{
	makeCurrent();
	red = r;
	update();
}

void A3GLWidget::greenChanged(int g)
{
	makeCurrent();
	green = g;
	update();
}

void A3GLWidget::blueChanged(int b)
{
	makeCurrent();
	blue = b;
	update();
}