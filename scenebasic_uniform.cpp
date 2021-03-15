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
#include "helper/torus.h"
#include "helper/texture.h"


using glm::vec3;
using glm::mat4;
using glm::vec4;
using glm::mat3;
SceneBasic_Uniform::SceneBasic_Uniform() : plane(30.0f, 30.0f, 100, 100), angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), sky(100.0f)
{
    mesh = ObjMesh::load("media/Trex.obj", true);
    mesh1 = ObjMesh::load("media/Raptor.obj", true);
    mesh2 = ObjMesh::load("media/Raptor.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(-20.0f, 10.5f, -20.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    angle = glm::radians(90.0f);
    float x, z;
    prog.setUniform("lights[0].Position", vec3(10.0f, 10.0f, 10.0f));
    prog.setUniform("lights[0].L", vec3(0.0f, 0.8f, 0.0f)); //Set the light colour
    prog.setUniform("lights[0].La", vec3(0.0f, 0.05f, 0.0f)); //Set the ambient light colour
    //Setup first spotlight
    prog.setUniform("Spot[0].L", vec3(2.0f));
    prog.setUniform("Spot[0].La", vec3(0.0f));
    prog.setUniform("Spot[0].Exponent", 20.0f);
    prog.setUniform("Spot[0].Cutoff", glm::radians(3.14f * 4.0f));
    //Setup second spotlight on the Raptor
    prog.setUniform("Spot[1].L", vec3(2.0f, 0.2f, 0.2f));
    prog.setUniform("Spot[1].La", vec3(0.0f));
    prog.setUniform("Spot[1].Exponent", 20.0f);
    prog.setUniform("Spot[1].Cutoff", glm::radians(3.14f * 4.0f));
    //Bind the texture to the spotlight
    GLuint texID = Texture::loadTexture("media/texture/Raptor_low_Corpo_Roughness.png");
    GLuint cubeTex =
        Texture::loadCubeMap("media/texture/cube/pisa/forest", ".png");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);


}
void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/Blinn-Phong_Vertex_Shader.vert");
		prog.compileShader("shader/Blinn-Phong_Fragment.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update(float t)
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f)
        deltaT = 0.0f;
    tPrev = t;
    angle += rotSpeed * deltaT;
    if (angle > glm::two_pi<float>())
        angle -= glm::two_pi<float>();


   
}

void SceneBasic_Uniform::render()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
vec4 lightPos = vec4(1.0f, 28.5f, 0.0f, 1.0f);
prog.setUniform("Spot[0].Position", vec3(view * lightPos));
mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
prog.setUniform("Spot[0].Direction", normalMatrix * vec3(-lightPos));
//Set the second spotlight's uniforms
lightPos = vec4(-4.0f, 7.5f, 3.0f, 1.0f); //As there is no view matrix applied - this will be based on the camera.
prog.setUniform("Spot[1].Position", vec3(lightPos));
prog.setUniform("Spot[1].Direction", normalMatrix * vec3(-lightPos));

//Set material uniforms for the first mesh, apply transformations and render. (T-Rex)
 prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
 prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
 prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
 prog.setUniform("Material.Shininess", 180.0f);
 prog.setUniform("test1", vec3(3.0f));
 model = mat4(1.0f);
 model = glm::rotate(model, glm::radians(3.14f * 0.5f), vec3(0.0f, 1.0f, 0.0f));
 model = glm::scale(model, vec3(0.4f, 0.4f, 0.4f));
 model = glm::translate(model, vec3(-2.0f, 5.0f, -1.0f));
 setMatrices();
 mesh->render();
 //Set material uniforms for the second model, apply transformations and render. (Raptor #1)
 prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
 prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
 prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
 prog.setUniform("Material.Shininess", 55.0f);
 prog.setUniform("test1", vec3(5.0f));

 model = mat4(1.0f);
 model = glm::rotate(model, glm::radians(3.14f * 0.5f), vec3(0.0f, 1.0f, 0.0f));
 model = glm::translate(model, vec3(-10.0f, 1.2f, 1.0f));
 model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
 setMatrices();
 mesh1->render();

 //Set material uniforms for the third model, apply transformations and render. This also includes the fog.(Raptor #2)
 prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
 prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
 prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
 prog.setUniform("Material.Shininess", 300.0f);
 prog.setUniform("Fog.MaxDist", 30.0f);
 prog.setUniform("Fog.MinDist", 0.1f);
 prog.setUniform("Fog.Color", vec3(0.2f, 0.2f, 0.2f));
 prog.setUniform("test1", vec3(0.0f));
 model = mat4(1.0f);
 model = glm::rotate(model, glm::radians(3.14f * 0.5f), vec3(0.0f, 1.0f, 0.0f));
 model = glm::translate(model, vec3(10.0f, 1.2f, 1.0f));
 model = glm::scale(model, vec3(0.5f, 0.5f, 0.5f));
 setMatrices();
 mesh2->render();



 //Render the plane.
 model = mat4(1.0f);
 prog.setUniform("test1", vec3(4.0f));
 setMatrices();
 plane.render();


 vec3 cameraPos = vec3(15.0f * cos(angle), 2.0f, 15.0f * sin(angle));
 view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f,
     0.0f));
 // Draw sky
 prog.use();
 model = mat4(1.0f);
 prog.setUniform("test1", vec3(2.0f));
 setMatrices();
 sky.render();
}


void SceneBasic_Uniform::setMatrices()
{
    mat4 mv;
    mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setupFBO()
{
    GLuint depthBuf;
    // Create and bind the FBO
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // The depth buffer
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // The HDR color buffer
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &hdrTex);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Attach the images to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        hdrTex, 0);
    GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(2, drawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
void SceneBasic_Uniform::drawScene()
{
    vec3 intense = vec3(1.0f);
    prog.setUniform("Lights[0].L", intense);
    prog.setUniform("Lights[1].L", intense);
    prog.setUniform("Lights[2].L", intense);
    vec4 lightPos = vec4(0.0f, 4.0f, 2.5f, 1.0f);
    lightPos.x = -7.0f;
    prog.setUniform("Lights[0].Position", view * lightPos);
    lightPos.x = 0.0f;
    prog.setUniform("Lights[1].Position", view * lightPos);
    lightPos.x = 7.0f;
    prog.setUniform("Lights[2].Position", view * lightPos);
    prog.setUniform("Material.Kd", 0.9f, 0.3f, 0.2f);
    prog.setUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 100.0f);
    // The backdrop plane
    model = glm::rotate(mat4(1.0f), glm::radians(90.0f), vec3(1.0f, 0.0f,0.0f));
    setMatrices();
    plane.render();
    // The bottom plane
    model = glm::translate(mat4(1.0f), vec3(0.0f, -5.0f, 0.0f));
    setMatrices();
    plane.render();
    // Top plane
    model = glm::translate(mat4(1.0f), vec3(0.0f, 5.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
    plane.render();
    prog.setUniform("Material.Kd", vec3(0.4f, 0.9f, 0.4f));
    model = glm::translate(mat4(1.0f), vec3(-3.0f, -3.0f, 2.0f));
    setMatrices();
    prog.setUniform("Material.Kd", vec3(0.4f, 0.4f, 0.9f));
    model = glm::translate(mat4(1.0f), vec3(3.0f, -5.0f, 1.5f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices();
}
