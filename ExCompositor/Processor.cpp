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
    _image(new Image())
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

    glBindTexture(GL_TEXTURE_2D, _colorBufferSrcId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _srcResolution.x, _srcResolution.y,
                 0, GL_RGB, GL_FLOAT, color.data());

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBloomBlendId);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBloomBlurId);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _colorBufferTonemappedId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _colorBufferLuminanceId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _colorBufferFireFliesId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _colorBufferSrcId);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(_vao);

    // TODO mipmaps

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferFireFliesId);
    glViewport(0, 0, _fireFliesResolution.x, _fireFliesResolution.y);
    _fireFliesPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _fireFliesPass->popProgram();

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferLuminanceId);
    glViewport(0, 0, _luminanceResolution.x, _luminanceResolution.y);
    _luminancePass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _luminancePass->popProgram();

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferTonemapedId);
    glViewport(0, 0, _tonemappingResolution.x, _tonemappingResolution.y);
    _tonemappingPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _tonemappingPass->popProgram();

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferBloomBlurId);
    glViewport(0, 0, _bloomBlurResolution.x, _bloomBlurResolution.y);
    _bloomBlurPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _bloomBlurPass->popProgram();

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferBloomBlendId);
    glViewport(0, 0, _bloomBlendResolution.x, _bloomBlendResolution.y);
    _bloomBlendPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _bloomBlendPass->popProgram();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _gammatizeResolution.x, _gammatizeResolution.y);
    _gammatizePass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _gammatizePass->popProgram();


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


    glGenTextures(1, &_colorBufferSrcId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferSrcId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _srcResolution.x, _srcResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    _fireFliesResolution = _srcResolution;
    glGenTextures(1, &_colorBufferFireFliesId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferFireFliesId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _fireFliesResolution.x, _fireFliesResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &_frameBufferFireFliesId);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferFireFliesId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _colorBufferFireFliesId, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    _luminanceResolution = _srcResolution / 8;
    glGenTextures(1, &_colorBufferLuminanceId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferLuminanceId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _luminanceResolution.x, _luminanceResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &_frameBufferLuminanceId);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferLuminanceId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _colorBufferLuminanceId, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    _tonemappingResolution = _srcResolution;
    glGenTextures(1, &_colorBufferTonemappedId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferTonemappedId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _tonemappingResolution.x, _tonemappingResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &_frameBufferTonemapedId);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferTonemapedId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _colorBufferTonemappedId, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    _bloomBlurResolution = _srcResolution;
    glGenTextures(1, &_colorBufferBloomBlurId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBloomBlurId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _bloomBlurResolution.x, _bloomBlurResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &_frameBufferBloomBlurId);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferBloomBlurId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _colorBufferBloomBlurId, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    _bloomBlendResolution = _srcResolution;
    glGenTextures(1, &_colorBufferBloomBlendId);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBloomBlendId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _bloomBlendResolution.x, _bloomBlendResolution.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &_frameBufferBloomBlendId);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferBloomBlendId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, _colorBufferBloomBlendId, 0);


    _gammatizeResolution = _dstResolution;
    glBindTexture(GL_TEXTURE_2D, 0);
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
    _fireFliesPass.reset(new GlProgram());
    _fireFliesPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _fireFliesPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/FireFlies.frag");
    _fireFliesPass->link();
    _fireFliesPass->pushProgram();
    _fireFliesPass->setInt("SourceImage", 0);
    _fireFliesPass->setFloat("Threshold", 1.0f);
    _fireFliesPass->popProgram();

    _luminancePass.reset(new GlProgram());
    _luminancePass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _luminancePass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Luminance.frag");
    _luminancePass->link();
    _luminancePass->pushProgram();
    _luminancePass->setInt("FireFlies", 1);
    _luminancePass->popProgram();

    _tonemappingPass.reset(new GlProgram());
    _tonemappingPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _tonemappingPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Tonemapping.frag");
    _tonemappingPass->link();
    _tonemappingPass->pushProgram();
    _tonemappingPass->setInt("FireFlies", 1);
    _tonemappingPass->setInt("Luminance", 2);
    _tonemappingPass->setFloat("ExposureGain", 1.0f);
    _tonemappingPass->popProgram();

    _bloomBlurPass.reset(new GlProgram());
    _bloomBlurPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _bloomBlurPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/BloomBlur.frag");
    _bloomBlurPass->link();
    _bloomBlurPass->pushProgram();
    _bloomBlurPass->setInt("Tonemap", 3);
    _bloomBlurPass->popProgram();

    _bloomBlendPass.reset(new GlProgram());
    _bloomBlendPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _bloomBlendPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/BloomBlend.frag");
    _bloomBlendPass->link();
    _bloomBlendPass->pushProgram();
    _bloomBlendPass->setInt("Tonemap", 3);
    _bloomBlendPass->setInt("BloomBlur", 4);
    _bloomBlendPass->popProgram();

    _gammatizePass.reset(new GlProgram());
    _gammatizePass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _gammatizePass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Gammatize.frag");
    _gammatizePass->link();
    _gammatizePass->pushProgram();
    _gammatizePass->setInt("Bloom", 5);
    _gammatizePass->popProgram();


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
