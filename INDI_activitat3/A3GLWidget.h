#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "model.h"
#include "MyGLWidget.h"

class A3GLWidget : public MyGLWidget
{
  Q_OBJECT

#define NUM_TORXES 6
#define HORA_MIN 8
#define HORA_MAX 20
#define NUM_FOCUS_FAR 2

  public:
    A3GLWidget (QWidget *parent=0);
    ~A3GLWidget ();

  protected:

    glm::mat4 modelTransformFar1();
    void modelTransformFar2();
    void modelTransformVaixell();
    void iniEscena ();
    void carregaShaders();

    virtual void paintGL ( );
    virtual void keyPressEvent (QKeyEvent *event);

    int hour = 14;
    float angleSol = 90;
    //--------------------------------------------------------------
    // uniform locations
    GLuint ambientLoc, diffLoc, especLoc, lightPosLocFar1, lightPosLocFar2;
    GLuint colLlumLoc;
    

    //--------------------------------------------------------------
    // Posicions
    glm::vec3 VaixellPos, lightPos = glm::vec3(0, 40, 0);
    GLuint vertexfLoc, lightPosLoc, LLoc, normalSCOLoc, NormalMatrixLoc, vertexSCOLoc, LightFar1Loc, LightFar2Loc;;
    GLuint torxesPosLoc, lightTorxesLoc, timeLoc;
    float angleFar2 = 0;
    


    //--------------------------------------------------------------
    // Colors
    GLuint matambfragLoc, matdifffragLoc, matescfragLoc, matspecfragLoc, matshinfragLoc;
    GLuint torxesColorLoc, torxesEncesesLoc, wavePaintingLoc;

    int torxesEnceses[NUM_TORXES] = {false, false, false, false, false, false};
    int red = 255, green = 255, blue = 0;
    int wavePainting = 0;
     

    //--------------------------------------------------------------
    // MATRIUS DE TRANSFORMACIÓ
    glm::mat4 Vaixell_TG, Far2_TG;

    public slots:
      void changeHour(int h);
      void spinBoxChanged(int h);

      void torxa1Changed(bool b);
      void torxa2Changed(bool b);
      void torxa3Changed(bool b);
      void torxa4Changed(bool b);
      void torxa5Changed(bool b);
      void torxa6Changed(bool b); 

      void redChanged(int r);
      void greenChanged(int g);
      void blueChanged(int b);
      
    
    signals:
      void hourChanged(int hour);

};

