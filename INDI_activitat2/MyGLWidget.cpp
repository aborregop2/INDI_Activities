#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

MyGLWidget::MyGLWidget(QWidget *parent=0): QOpenGLWidget(parent), program(NULL)
{
  srand (time(NULL));
}

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget()
{
}

void MyGLWidget::initializeGL ()
{
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
  update();
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  numtrees = 5;
  setTrees(numtrees);
  creaBuffersModels();
  creaBuffersTerra();
  iniEscena();
  iniCamera();
}

void MyGLWidget::setTrees(int numTrees)
{
  posicioArbres = new glm::vec3[numTrees];
  escalaArbres = new glm::vec3[numTrees];
  rotacioArbres = new float[numTrees];

  srand(time(0));
  for (int i = 0; i < numTrees; ++i) {
    float x = -5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 10.0f));
    float z = -5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 10.0f));

    float escala = 0.08f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (0.13f - 0.08f)));

    float angle = static_cast<float>(rand() % 361);

    posicioArbres[i] = glm::vec3(x,0,z);
    escalaArbres[i] = glm::vec3(1.f,1.f,1.f) * escala;
    rotacioArbres[i] = angle;

  }
}

void MyGLWidget::iniEscena ()
{
    glm::vec3 pMin = glm::vec3(-5.f, 0.f, -5.f);
    glm::vec3 pMax = glm::vec3(5.f, 2.f, 5.f);
    centreEscena = (pMin + pMax) / 2.f;

    radiEscena = glm::distance(pMax, centreEscena);
    d=2*radiEscena;
}

void MyGLWidget::iniCamera() {

    fov = 2 * glm::asin(radiEscena/d);
    znear = d - radiEscena;
    zfar  = d + radiEscena;

    viewTransform();
    projectPerspectiveTransform();
}

void MyGLWidget::viewTransform ()
{
    // Matriu de posició i orientació de l'observador
    glm::mat4 View(1.0f);

      //cout << angleX << " " << angleY << endl;

    View = glm::translate(View, glm::vec3(0, 0,-d));
    View = glm::rotate(View, glm::radians(angleY), glm::vec3(1,0,0));
    View = glm::rotate(View, glm::radians(angleX), glm::vec3(0,1,0));
    
    


    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::projectPerspectiveTransform ()
{
    glm::mat4 Proj(1.0f);
	  Proj = glm::perspective (fov, ra, znear, zfar);

    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::projectOrthoTransform () 
{
    glm::mat4 Proj;
    if(ample < alt) Proj = glm::ortho (-radiEscena, radiEscena, -radiEscena / ra, radiEscena / ra, znear, zfar);
    else Proj = glm::ortho (-radiEscena * ra, radiEscena * ra, -radiEscena, radiEscena, znear, zfar);

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::paintGL ()
{

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray (VAO_models[TREE]);
  for (int i = 0; i < numtrees; ++i) {
    treeTransform(i);
    glDrawArrays(GL_TRIANGLES, 0, models[TREE].faces().size()*3);
  }
  

  glBindVertexArray (VAO_models[LUKE]);
  LukeTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[LUKE].faces().size()*3);

  // Terra
  glBindVertexArray (VAO_Terra);
  terraTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}

void MyGLWidget::resizeGL (int w, int h)
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = float(w);
  alt = float(h);

  ra = float(w)/float(h);

  if (ra >= 1) fov = 2 * glm::asin(radiEscena/d);
  else fov = 2 * glm::atan(glm::tan(glm::asin(radiEscena/d))/ra);

  if (ortho) projectOrthoTransform ();
  else projectPerspectiveTransform ();

#endif
}


void MyGLWidget::treeTransform (int i)
{
  glm::mat4 TG(1.0f);

  TG = glm::translate(TG, posicioArbres[i]);
  TG = glm::rotate(TG, glm::radians(rotacioArbres[i]), glm::vec3(0,1,0));
  TG = glm::scale(TG, escalaArbres[i]);
  TG = glm::translate(TG, -centreBaseModels[TREE]);

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::LukeTransform()
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, posLuke);
  TG = glm::rotate(TG, glm::radians(rotLuke), glm::vec3(0,1,0));
  TG = glm::scale(TG, escalaModels[LUKE] * glm::vec3(1.f,1.f,1.f));
  TG = glm::translate(TG, -centreBaseModels[LUKE]);

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::terraTransform ()
{
  glm::mat4 TG(1.0f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: {
      if ((posLuke.x + dirLuke.x <= 5 and posLuke.x + dirLuke.x >= -5) and (posLuke.z + dirLuke.z <= 5 and posLuke.z + dirLuke.z >= -5)) posLuke += dirLuke;
      break;
    }
    case Qt::Key_Left: { 	
      dirLuke = glm::vec3(0, 0, 1);
      rotLuke -= 45;
      dirLuke = glm::vec3(dirLuke.x*cos(glm::radians(rotLuke)) + dirLuke.z*sin(glm::radians(rotLuke)), 0, -dirLuke.x*sin(glm::radians(rotLuke)) + dirLuke.z*cos(glm::radians(rotLuke)));
      break;
    }
    case Qt::Key_Right: {
      dirLuke = glm::vec3(0, 0, 1);
      rotLuke += 45;
      dirLuke = glm::vec3(dirLuke.x*cos(glm::radians(rotLuke)) + dirLuke.z*sin(glm::radians(rotLuke)), 0, -dirLuke.x*sin(glm::radians(rotLuke)) + dirLuke.z*cos(glm::radians(rotLuke)));
      break;
    }
    case Qt::Key_C: { 
      makeCurrent();
      ortho = !ortho;

      swap(angleY, angleYortho);
      swap(angleX, angleXortho);
      viewTransform();

      if (ortho) projectOrthoTransform ();
      else projectPerspectiveTransform();

      checkBox();

      update();

      break;
    }
    case Qt::Key_R: { // reset
      if (ortho) {
        ortho = false;
        projectPerspectiveTransform();
      }

      angleY = 45;
      angleX = 0;
      angleYortho = 90;
      angleXortho = 0;
      posLuke = glm::vec3(0, 0, 0);
      dirLuke = glm::vec3(0, 0, 1);
      rotLuke = 0;
      angleCanviat = 0;

      viewTransform ();
      checkBox();
      dial();

      break;
    }
    case Qt::Key_Plus: {
      for (int i = 0; i < numtrees; ++i) {
        angleCanviat = (angleCanviat + 1)%361;
        rotacioArbres[i]++;
      }
      dial();
      update();
      break;
    }
    case Qt::Key_Minus: {
      for (int i = 0; i < numtrees; ++i) {
        angleCanviat = (angleCanviat - 1)%361;
        rotacioArbres[i]--;
      }
      dial();
      update();

      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();
  

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  if (!ortho) {
    makeCurrent();
    angleX += (e->x() - xClick) / factorAngleX;
    angleY += (e->y() - yClick) / factorAngleY;

    xClick = e->x();
    yClick = e->y();
        
    viewTransform();
    update();
  }
}

void MyGLWidget::changeNumTrees(int s)
{
  numtrees = s;
  setTrees(s);
  update();
}

void MyGLWidget::changePM()
{
  makeCurrent();
  ortho = !ortho;

  swap(angleY, angleYortho);
  swap(angleX, angleXortho);
  viewTransform();

  if (ortho) projectOrthoTransform ();
  else projectPerspectiveTransform();

  update();
}

void MyGLWidget::reset() 
{
  makeCurrent();
  if (ortho) {
    ortho = false;
    projectPerspectiveTransform();
  }

  angleY = 45;
  angleX = 0;
  angleYortho = 90;
  angleXortho = 0;
  posLuke = glm::vec3(0, 0, 0);
  dirLuke = glm::vec3(0, 0, 1);
  rotLuke = 0;
  angleCanviat = 0;

  viewTransform ();
  checkBox();
  dial();

  update();
}

void MyGLWidget::rotateTrees(int ang)
{
  makeCurrent();

  for (int i = 0; i < numtrees; ++i) {
    rotacioArbres[i] += ang;
  }
  update();
} 


void MyGLWidget::creaBuffersTerra ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posTerra[4] = {
        glm::vec3(-5.0, 0.0, -5.0),
        glm::vec3(-5.0, 0.0,  5.0),
        glm::vec3( 5.0, 0.0, -5.0),
        glm::vec3( 5.0, 0.0,  5.0)
  };

  glm::vec3 c(0.65, 0.2, 0.05);
  glm::vec3 colTerra[4] = { c, c, c, c };

  // VAO
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);

  // geometria
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posTerra), posTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colTerra), colTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }

  escala = alcadaDesitjada/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersModels ()
{
  // Càrrega dels models
  models[TREE].load("./models/tree.obj");
  models[LUKE].load("./models/luke_jedi.obj");

  // Creació de VAOs i VBOs per pintar els models
  glGenVertexArrays(NUM_MODELS, &VAO_models[0]);

  float alcadaDesitjada[NUM_MODELS] = {1,1};//,1,1,1.5,1};

  for (int i = 0; i < NUM_MODELS; i++)
  {
	  // Calculem la capsa contenidora del model
	  calculaCapsaModel (models[i], escalaModels[i], alcadaDesitjada[i], centreBaseModels[i]);

	  glBindVertexArray(VAO_models[i]);

	  GLuint VBO[2];
	  glGenBuffers(2, VBO);

	  // geometria
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_vertices(), GL_STATIC_DRAW);
	  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(vertexLoc);

	  // color
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_matdiff(), GL_STATIC_DRAW);
	  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(colorLoc);
  }

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile(":shaders/basicShader.frag");
  vs.compileSourceFile(":shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Identificador per als  atributs
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");

  // Identificadors dels uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc  = glGetUniformLocation (program->programId(), "Proj");
  viewLoc  = glGetUniformLocation (program->programId(), "View");
}

void MyGLWidget::checkBox()
{
  emit mySignal(ortho);
}

void MyGLWidget::dial()
{
  emit mySignal2(angleCanviat);
}


