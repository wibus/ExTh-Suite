#include "Processor.h"

#include <sstream>

#include <GL/glu.h>

#include <QApplication>
#include <QDesktopWidget>

#include <CellarWorkbench/Misc/Log.h>
#include <CellarWorkbench/Image/Image.h>
#include <CellarWorkbench/GL/GlProgram.h>
#include <CellarWorkbench/GL/GlToolkit.h>

#include <PropRoom3D/Team/ArtDirector/Film/ConvergentFilm.h>

using namespace cellar;
using namespace prop3;


QGLFormat getGlFormat()
{
    QGLFormat format;
    return format;
}


Processor::Processor(const glm::ivec2& srcResolution,
                     const glm::ivec2& dstResolution) :
    QGLWidget(getGlFormat()),
    _srcResolution(srcResolution),
    _dstResolution(dstResolution),
    _film(new ConvergentFilm()),
    _image(new Image()),
    _frameBufferId(0),
    _depthBufferId(0),
    _colorBufferRawId(0),
    _colorBufferBlurId(0)
{

}

Processor::~Processor()
{

}

bool Processor::feed(const std::string& filmName, bool useLastAsRef)
{
    //if(useLastAsRef)
    //    _film->backupAsReferenceShot();
    //else
        _film->clearReferenceShot();

    if(!_film->loadRawFilm(filmName))
    {
        getLog().postMessage(new Message('E', false,
            "Film not found: " + filmName, "Processor"));
    }

    const std::vector<glm::vec3>& color =
        _film->colorBuffer(Film::ColorOutput::ALBEDO);

    glBindTexture(GL_TEXTURE_2D, _colorBufferRawId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _srcResolution.x, _srcResolution.y,
                 0, GL_RGB, GL_FLOAT, color.data());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBlurId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _colorBufferTonemapedId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _colorBufferRawId);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(_vao);

//    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferId);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
//                           GL_TEXTURE_2D, _colorBufferTonemapedId, 0);
    _tonemappingPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _tonemappingPass->popProgram();

//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
//                           GL_TEXTURE_2D, _colorBufferBlurId, 0);
//    _bloomBlurPass->pushProgram();
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    _bloomBlurPass->popProgram();

//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    _bloomBlendPass->pushProgram();
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    _bloomBlendPass->popProgram();


    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    GlToolkit::takeFramebufferShot(*_image);

    swapBuffers();
}

// QGLWidget interface
void Processor::initializeGL()
{
    if(gl3wInit() != 0)
    {
        getLog().postMessage(new Message('E', true,
            "Cannot initialize GL3W library...", "Processor"));
        return ;
    }

    std::ostringstream oss;
    oss << "OpenGL version: " << glGetString(GL_VERSION);
    getLog().postMessage(new Message('I', false, oss.str(), "QGlWidgetView"));

    oss.str("");
    oss << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
    getLog().postMessage(new Message('I', false, oss.str(), "QGlWidgetView"));

    oss.str("");
    int nbBuffers;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &nbBuffers);
    oss << "Multisampling number of buffers: " << nbBuffers;
    getLog().postMessage(new Message('I', false, oss.str(), "QGlWidgetView"));

    oss.str("");
    int nbSamples;
    glGetIntegerv(GL_SAMPLES, &nbSamples);
    oss << "Multisampling number of samples: " << nbSamples;
    getLog().postMessage(new Message('I', false, oss.str(), "QGlWidgetView"));


    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);



    glGenTextures(1, &_colorBufferRawId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferRawId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _srcResolution.x, _srcResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenTextures(1, &_colorBufferTonemapedId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferTonemapedId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _srcResolution.x, _srcResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenTextures(1, &_colorBufferBlurId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBlurId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _srcResolution.x, _srcResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);


    glGenRenderbuffers(1, &_depthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32,
                          _srcResolution.x, _srcResolution.y);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);


    glGenFramebuffers(1, &_frameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _colorBufferBlurId, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, _depthBufferId);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        getLog().postMessage(new Message('E', true,
            "Framebuffer is broken... " + std::to_string(status), "Processor"));
        return ;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Fullscreen triangle
    GLfloat backTriangle[] = {
        -1.0f, -1.0f,
         3.0f, -1.0f,
        -1.0f,  3.0f
    };

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backTriangle), backTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // Passes
    _tonemappingPass.reset(new GlProgram());
    _tonemappingPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _tonemappingPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Tonemapping.frag");
    _tonemappingPass->link();
    _tonemappingPass->pushProgram();
    _tonemappingPass->setInt("SourceImage", 0);
    _tonemappingPass->setFloat("MidGray", 0.5f);
    _tonemappingPass->popProgram();

    _bloomBlurPass.reset(new GlProgram());
    _bloomBlurPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _bloomBlurPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/BloomBlur.frag");
    _bloomBlurPass->link();
    _bloomBlurPass->pushProgram();
    _bloomBlurPass->setInt("BloomBase", 1);
    _bloomBlurPass->popProgram();

    _bloomBlendPass.reset(new GlProgram());
    _bloomBlendPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _bloomBlendPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/BloomBlend.frag");
    _bloomBlendPass->link();
    _bloomBlendPass->pushProgram();
    _bloomBlendPass->setInt("BloomBase", 1);
    _bloomBlendPass->setInt("BloomBlur", 2);
    _bloomBlendPass->popProgram();


    // Make sure film's color buffer is set to albedo
    _film->resizeFrame(_srcResolution);
    _film->colorBuffer(Film::ColorOutput::ALBEDO);

    resize(_dstResolution.x, _dstResolution.y);


    QPoint br = QApplication::desktop()->availableGeometry(this).bottomRight();
    move(br.x() - size().width(), br.y() - size().height());
}

void Processor::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Processor::paintGL()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
