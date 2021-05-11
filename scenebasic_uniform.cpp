#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
#include <iostream>
#include <sstream>

using std::cerr;
using std::endl;


#include "helper/glutils.h"
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "helper/torus.h"
#include "helper/texture.h"
#include <random>
#include "helper/particleutils.h"
#include "helper/random.h"
#include <GLFW/glfw3.h>
//#include "imgui-1.79/imgui.h"
//#include "imgui-1.79/examples/imgui_impl_opengl3.h"
//#include "imgui-1.79/examples/imgui_impl_glfw.h"
//#include "imgui-1.79/examples/imgui_impl_win32.h"
using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;
SceneBasic_Uniform::SceneBasic_Uniform() : rotSpeed(0.25f), drawBuf(1), time(0), deltaT(0), nParticles(10000),
particleLifetime(6.0f), emitterPos(1, 0, 0), emitterDir(0.0f, 2.0f, 0), startTimer(false), sky(100.0f)
{
    spot = ObjMesh::loadWithAdjacency("media/raptor.obj");
    vent = ObjMesh::loadWithAdjacency("media/vent.obj");
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    angle = glm::half_pi<float>();

    prog.use();
    prog.setUniform("EdgeWidth", 0.015f);
    prog.setUniform("PctExtend", 0.25f);
    prog.setUniform("LineColor", vec4(0.05f, 0.0f, 0.05f, 1.0f));
    prog.setUniform("material.Kd", 0.7f, 0.5f, 0.2f);
    prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    prog.setUniform("material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Light.Intensity", 1.0f, 1.0f, 1.0f);

    //Particle setup

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("media/texture/fire.png");

    glActiveTexture(GL_TEXTURE1);
    ParticleUtils::createRandomTex1D(nParticles * 3);
    particleProg.use();
    initBuffers();
    particleProg.setUniform("RandomTex", 1);
    particleProg.setUniform("ParticleTex", 0);
    particleProg.setUniform("ParticleLifetime", particleLifetime);
    particleProg.setUniform("Accel", vec3(0.5f, 0.0f, 0.0f));
    particleProg.setUniform("ParticleSize", 0.10f);
    particleProg.setUniform("Emitter", emitterPos);
    particleProg.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));

    flatProg.use();
    flatProg.setUniform("Color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

    //Setup for skybox noise
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    projection = mat4(1.0f);
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quad);
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    skyboxProg.use();
    skyboxProg.setUniform("NoiseTex", 2);

    GLuint noiseTex = NoiseTex::generate2DTex(6.0f);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTex);



}


void SceneBasic_Uniform::updateLight()
{
    lightPos = vec4(5.0f * vec3(cosf(angle) * 5.0f, 1.5f, sinf(angle) * 5.0f), 1.0f);
}

void SceneBasic_Uniform::compile()
{
    try
    {

        prog.compileShader("shader/SilhouetteLinesFrag.frag");
        prog.compileShader("shader/SilhouetteLinesVert.vert");
        prog.compileShader("shader/SilhouetteGeometryShader.geom");
        prog.link();

        skyboxProg.compileShader("shader/noiseFragShader.frag");
        skyboxProg.compileShader("shader/noiseVertShader.vert");
        skyboxProg.link();

        particleProg.compileShader("shader/particlesFragShader.frag");
        particleProg.compileShader("shader/particlesVertShader.vert");
        GLuint progHandle = particleProg.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);
        particleProg.link();
        particleProg.use();

        flatProg.compileShader("shader/flat_frag.glsl");
        flatProg.compileShader("shader/flat_vert.glsl");
        flatProg.link();

    }
    catch (GLSLProgramException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}


void SceneBasic_Uniform::initBuffers()
{
    glGenBuffers(2, posBuf);
    glGenBuffers(2, velBuf);
    glGenBuffers(2, age);


    int size = nParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    std::vector<GLfloat> tempData(nParticles);
    float rate = particleLifetime / nParticles;
    for (int i = 0; i < nParticles; i++)
    {
        tempData[i] = rate * (i - nParticles);
    }
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), tempData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenVertexArrays(2, particleArray);

    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glGenTransformFeedbacks(2, feedback);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void SceneBasic_Uniform::update(float t)
{
    if (startTimer)
    {
        deltaT = t - holderT;
        if (deltaT > 10.0f)
        {
            deltaT = 0.0f;
        }
        holderT = t;
        time += deltaT;
        angle += deltaT * rotSpeed;
        if (angle > glm::two_pi<float>())
        {
            angle -= glm::two_pi<float>();
            enableParticles = !enableParticles;
        }
    }
    else
    {
        if ((int)time % 13 > 0)
        {
            enableParticles = !enableParticles;
        }
        deltaT = t - holderT;
        holderT = t;
    }
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    prog.use();


    vec3 cameraPos(4.0f, 2.0f, -12.0f);
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.2f, -0.85f), vec3(1.0f, 0.0f, 0.0f));

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-0.5f, -0.05f, -0.5f));
    model = glm::scale(model, (vec3(0.1f)));
    model = glm::rotate(model, glm::radians(270.0f), vec3(0, 0.0f, 1.0f));
    setMatrices(prog);
    vent->render();

    //model = mat4(1.0f);
    //model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
    //model = glm::scale(model, (vec3(0.1f)));
    //model = glm::rotate(model, glm::radians(270.0f), vec3(0, 0.0f, 1.0f));
    //setMatrices(prog);
    //spot->render();

    //model = mat4(1.0f);
    //model = glm::scale(model, (vec3(0.1f)));
    //model = glm::translate(model, vec3(5.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, glm::radians(270.0f), vec3(0, 0.0f, 1.0f));
    //setMatrices(prog);
    //spot->render();

    model = mat4(1.0f);
    model = glm::scale(model, (vec3(0.1f)));
    model = glm::translate(model, vec3(-4.0f, 0.0f, -3.0f));
    model = glm::rotate(model, glm::radians(270.0f), vec3(0, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices(prog);
    spot->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-0.5f, -0.1f, -0.5f));
    model = glm::scale(model, (vec3(0.1f)));

    
        if (!startTimer)
        {
            startTimer = true;
        }
        particleProg.use();
        particleProg.setUniform("Time", time);
        particleProg.setUniform("DeltaT", deltaT);

        particleProg.setUniform("Pass", 1);
        //glEnable(GL_BLEND);
        glEnable(GL_RASTERIZER_DISCARD);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
        glBeginTransformFeedback(GL_POINTS);

        glBindVertexArray(particleArray[1 - drawBuf]);
        glVertexAttribDivisor(0, 0);
        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glDrawArrays(GL_POINTS, 0, nParticles);
        glBindVertexArray(0);

        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);
        if (enableParticles)
        {
            particleProg.setUniform("Pass", 2);
            //view = glm::lookAt(vec3(4.0f * cos(angle), 1.5f, 4.0f * sin(angle)), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
            setMatrices(particleProg);
            glDepthMask(GL_FALSE);
            glBindVertexArray(particleArray[drawBuf]);
            glVertexAttribDivisor(0, 1);
            glVertexAttribDivisor(1, 1);
            glVertexAttribDivisor(2, 1);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
            glBindVertexArray(0);
            glDepthMask(GL_TRUE);
            drawBuf = 1 - drawBuf;
        }

    glActiveTexture(GL_TEXTURE2);
    skyboxProg.use();
    model = mat4(1.0f);
    setMatrices(skyboxProg);
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    sky.render();
    glFinish();

}


void SceneBasic_Uniform::setupFBO()
{
    /*GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    GLuint ambBuf;
    glGenRenderbuffers(1, &ambBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, ambBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

    glActiveTexture(GL_TEXTURE0);
    GLuint diffSpecTex;
    glGenTextures(1, &diffSpecTex);
    glBindTexture(GL_TEXTURE_2D, diffSpecTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &colorDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ambBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffSpecTex, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete.\n");
    }
    else
    {
        printf("Framebuffer is not complete.\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    float c = 1.5f;
    projection = glm::ortho(-0.4f * c, 0.4f * c, -0.3f * c, 0.3f * c, 0.1f, 100.0f);
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& prog)
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("ProjMatrix", projection);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("MV", mv);
    prog.setUniform("Proj", projection);
}


