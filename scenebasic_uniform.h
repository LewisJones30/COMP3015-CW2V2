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
#include "helper/frustum.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram volumeProg, renderProg, compProg;
    GLuint colorDepthFBO, fsQuad;
    GLuint spotTex, brickTex;

    Plane plane;
    std::unique_ptr<ObjMesh> spot;
    glm::vec4 lightPos;
    float angle, tPrev, rotSpeed;

    void setMatrices(GLSLProgram& prog);
    void compile();
    void setupFBO();
    void drawScene(GLSLProgram& prog, bool onlyShadowCasters);
    void pass1();
    void pass2();
    void pass3();
    void updateLight();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
