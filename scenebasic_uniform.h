#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/torus.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/teapot.h"
#include "helper/cube.h"
#include "helper/objmesh.h"
#include "helper/sphere.h"
#include "helper/skybox.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    std::unique_ptr<ObjMesh> mesh1;
    std::unique_ptr<ObjMesh> mesh2;
    GLuint hdrFBO;
    GLuint quad;
    GLuint hdrTex, avgTex;
    SkyBox sky;
    float angle, tPrev, rotSpeed;
    void setMatrices();
    void compile();
    void setupFBO();
    void pass1();
    void pass2();
    void computeLogAveLuminance();
    void drawScene();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
