#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"

#include <iostream>
using namespace std;


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core  {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();

  protected:
    virtual void initializeGL ( );
    virtual void paintGL ( );
    virtual void resizeGL(int, int);

    virtual void keyPressEvent (QKeyEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mousePressEvent (QMouseEvent *e);

    virtual void treeTransform(int i);
    virtual void LukeTransform();

    virtual void terraTransform();
  
    virtual void iniEscena ();
    virtual void iniCamera ();
    virtual void viewTransform ();
    virtual void projectPerspectiveTransform ();
    virtual void projectOrthoTransform ();

   glm::vec3 *posicioArbres;
   glm::vec3 *escalaArbres;
   float *rotacioArbres;
   int angleCanviat = 0;
   int numtrees;
  
   glm::vec3 dirLuke = glm::vec3(0, 0, 1), posLuke = glm::vec3(0, 0, 0);
   float rotLuke = 0;

    void calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &CentreBase);
    // creaBuffersModels - Carreguem els fitxers obj i fem la inicialització dels diferents VAOS i VBOs
    void creaBuffersModels ();
    // creaBuffersTerra - Inicialitzem el VAO i els VBO d'un model fet a mà que representa un terra
    void creaBuffersTerra ();
    // carregaShaders - Carreguem els shaders, els compilem i els linkem. També busquem els uniform locations que fem servir.
    void carregaShaders ();

    void setTrees(int numTrees);


  private:
  
    int printOglError(const char file[], int line, const char func[]);
   
    // variables per interacció de càmera amb angles d'Euler
    float angleX = 0, angleY = 45, angleYortho = 90, angleXortho = 0;
    
    // bools per controlar si càmera en planta o no
    bool ortho = false;


    GLuint vertexLoc, colorLoc;

    // uniform locations
    GLuint transLoc, viewLoc, projLoc;

    // enum models - els models estan en un array de VAOs (VAO_models), aquest enum és per fer més llegible el codi.
    typedef enum { TREE = 0, LUKE = 1, NUM_MODELS = 2} ModelType;

    // VAO names
    GLuint VAO_models[NUM_MODELS];
    GLuint VAO_Terra;


     QOpenGLShaderProgram *program;

    // Viewport
    GLint ample, alt;

    // Mouse interaction
    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float factorAngleX = 5, factorAngleY = 5;

    // Internal vars
    float radiEscena;
    float fov, ra=1, znear, zfar, d;
    glm::vec3 centreEscena;

    // Models, capses contenidores i escales
    Model models[NUM_MODELS];
    glm::vec3 centreBaseModels[NUM_MODELS];
    float escalaModels[NUM_MODELS];

    public slots:
      void changeNumTrees(int s);
      void changePM();
      void reset();
      void rotateTrees(int ang);
      void checkBox();
      void dial();

    signals:
      void mySignal(bool ortho);
      void mySignal2(int value);




};
