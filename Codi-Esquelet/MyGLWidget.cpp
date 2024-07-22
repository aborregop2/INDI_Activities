#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  scale = 1.0f;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  
  
  glEnable(GL_DEPTH_TEST); 

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffers();
}

void MyGLWidget::paintGL () 
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
  
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT);

  // Carreguem la transformació de model
  modelTransform ();

  if (ortho) projectOrthogonalTransform();
  else projectPerspectiveTransform();

  viewTransform();

  // Activem el VAO per a pintar la caseta 
  glBindVertexArray (VAO_Homer);

  // pintem
  glDrawArrays(GL_TRIANGLES, 0, Homer.faces().size () * 3);

  glBindVertexArray (0);
}

glm::vec3 MyGLWidget::boxBase(float *pos)
{
  glm::vec3 base;
  float xmin = pos[0], xmax = 0,  ymin = pos[1],  zmin = pos[2], zmax = 0;

  for (unsigned int i = 0; i < sizeof(GLfloat) * Homer.faces ().size () * 3; i+=3) {
    if (pos[i] < xmin) {
      xmin = pos[i];
    }
    if (pos[i] > xmax) {
      xmax = pos[i];
    }


    if (pos[i + 1] < ymin) {
      ymin = pos[i + 1];
    }


    if (pos[i + 2] < zmin) {
      zmin = pos[i + 2];
    }
    if (pos[i + 2] > zmax) {
      zmax = pos[i + 2];
    }
  }

  base.x = (xmax+xmin)/2.f;
  base.y = ymin;
  base.z = (zmax+zmin)/2.f;

  return base;
}

void MyGLWidget::modelTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::translate(transform, -pos);
  transform = glm::scale(transform, glm::vec3(scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) 
{
  ample = w;
  alt = h;

  ra = float(w)/float(h);

  if (ra >= 1) FOV = 2 * glm::asin(R/d);
  else FOV = 2 * glm::atan(glm::tan(glm::asin(R/d))/ra);

}

void MyGLWidget::mouseMoveEvent (QMouseEvent *e) 
{
  angleh = -(e->x() - xant)/200;
  anglev = (e->y() - yant)/200;
  viewTransform();
  update();
}

void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
  xant = e->x();
  yant = e->y();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_D: { 
      angleh -= 0.1;
      break;
    }
    case Qt::Key_S: { 
      anglev -= 0.1;
      break;
    }
    case Qt::Key_W: { 
      anglev += 0.1;
      break;
    }
    case Qt::Key_A: { 
      angleh += 0.1;
      break;
    }
    case Qt::Key_Z: { 
      FOV += 0.1;
      break;
    }
    case Qt::Key_X: { 
      FOV -= 0.1;
      break;
    }
    case Qt::Key_Q: { 
      if (ortho) ortho = false;
      else ortho = true;
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::creaBuffers () 
{
  Homer.load("Models/HomerProves.obj");
  float* posicio = Homer.VBO_vertices();
  pos = boxBase(posicio);

  float* color = Homer.VBO_matdiff();


  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  //sizeof(GLfloat) * m.faces ().size () * 3 * 3
  GLuint VBO_Homer[2];
  glGenBuffers(2, VBO_Homer);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * Homer.faces ().size () * 3 * 3, posicio, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * Homer.faces ().size () * 3 * 3, color, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
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
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
}

void MyGLWidget::projectPerspectiveTransform() {
    glm::mat4 Proj = glm::perspective (FOV, ra, zN, zF);

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::projectOrthogonalTransform () {
    glm::mat4 Proj;
    if(ample >=alt) Proj = glm::ortho (-R, R, -R / ra, R / ra, zN, zF);
    else Proj = glm::ortho (-R * ra, R * ra, -R, R, zN, zF);

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
    glm::mat4 VM(1.f);
    VM = glm::translate (VM, glm::vec3(0.f, -1.f, -d - 0.5));
    VM = glm::rotate(VM, anglev, glm::vec3(1,0,0)); 
    VM = glm::rotate(VM, -angleh, glm::vec3(0,1,0)); 
    

    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &VM[0][0]);

}



