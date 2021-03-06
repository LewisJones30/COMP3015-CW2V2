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
#include "helper/noisetex.h"
#include "helper/random.h"
#include "helper/grid.h"
#include <GLFW/glfw3.h>

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram particleProg, flatProg, prog, skyboxProg;
    GLuint colorDepthFBO, fsQuad;
    GLuint spotTex, brickTex;
    GLuint pass1IndexNV, pass2IndexNV;
    GLuint renderFBO;
    GLuint renderTex;
    GLuint noiseTex;
    bool startTimer;
    bool enableParticles = true;
    
    Random rand;
    glm::vec3 emitterPos, emitterDir;

    GLuint posBuf[2], velBuf[2], age[2];
    GLuint particleArray[2];
    GLuint feedback[2];
    
    GLuint drawBuf;

    Grid grid;

    int nParticles;
    float particleLifetime;
    float time, timer, deltaT, holderT;

    SkyBox sky;
    GLuint quad;

    std::unique_ptr<ObjMesh> spot, vent;
    glm::vec4 lightPos;
    float angle, tPrev, rotSpeed;

    void setMatrices(GLSLProgram& prog);
    void setMatrices();
    void compile();
    void setupFBO();
    void initBuffers();
    void drawScene(GLSLProgram& prog, bool onlyShadowCasters);
    void pass1();
    void pass2();
    void pass3();
    void pass1NV();
    void pass2NV();
    void updateLight();
    void testFunc(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
