#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"

using namespace std;

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *e);
    
    //Camera
    virtual void projectPerspectiveTransform ();
    virtual void projectOrthogonalTransform ();
    
    virtual void viewTransform ();
    void mousePressEvent(QMouseEvent *e);


    virtual glm::vec3 boxBase(float *pos);

  private:
    void creaBuffers ();
    void carregaShaders ();
    void modelTransform ();

	Model Homer;
		
    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // VAO i VBO names
    GLuint VAO_Homer;
    // Program
    QOpenGLShaderProgram *program;
    // Viewport
    GLint ample, alt;
    // Internal vars
    float scale, angleh = 0, anglev = 0;
    glm::vec3 pos;
    bool ortho = false;


    //Camera vars
    glm::vec3 Pmin = glm::vec3(-1,0,-1), Pmax = glm::vec3(1,2,1), centre = (Pmin+Pmax)/2.f;
    float R = distance(centre, Pmax), d = 2*R; 

    float FOV, ra, zN = d-R, zF = d+R;
    glm::vec3 VRP = centre;
    float xant, yant;
};

