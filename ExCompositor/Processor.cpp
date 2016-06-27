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


    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, _colorBufferTonemappedId);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _colorBufferMipMapSumId);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _colorBufferLuminanceId);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBlurYBaseId);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _colorBufferBlurXBaseId);

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
    glGenerateTextureMipmap(_colorBufferFireFliesId);

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferBlurXBaseId);
    glViewport(0, 0, _blurXBaseResolution.x, _blurXBaseResolution.y);
    _blurXPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _blurXPass->popProgram();

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferBlurYBaseId);
    glViewport(0, 0, _blurYBaseResolution.x, _blurYBaseResolution.y);
    _blurYPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _blurYPass->popProgram();


    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferLuminanceId);
    glViewport(0, 0, _luminanceResolution.x, _luminanceResolution.y);
    _luminancePass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _luminancePass->popProgram();
    glGenerateTextureMipmap(_colorBufferLuminanceId);

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferMipmapSumId);
    glViewport(0, 0, _mipmapSumResolution.x, _mipmapSumResolution.y);
    _mipmapSumPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _mipmapSumPass->popProgram();


    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferTonemapedId);
    glViewport(0, 0, _tonemappingResolution.x, _tonemappingResolution.y);
    _tonemappingPass->pushProgram();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _tonemappingPass->popProgram();

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
    genFramebuffer(
        _frameBufferFireFliesId,
        _colorBufferFireFliesId,
        _fireFliesResolution,
        true);

    _blurXBaseResolution = _dstResolution / 2;
    genFramebuffer(
        _frameBufferBlurXBaseId,
        _colorBufferBlurXBaseId,
        _blurXBaseResolution,
        false);

    _blurYBaseResolution = _dstResolution / 2;
    genFramebuffer(
        _frameBufferBlurYBaseId,
        _colorBufferBlurYBaseId,
        _blurYBaseResolution,
        false);


    _luminanceResolution = _srcResolution / 2;
    genFramebuffer(
        _frameBufferLuminanceId,
        _colorBufferLuminanceId,
        _luminanceResolution,
        true);

    _mipmapSumResolution = _srcResolution / 2;
    genFramebuffer(
        _frameBufferMipmapSumId,
        _colorBufferMipMapSumId,
        _mipmapSumResolution,
        false);

    _tonemappingResolution = _dstResolution;
    genFramebuffer(
        _frameBufferTonemapedId,
        _colorBufferTonemappedId,
        _tonemappingResolution,
        false);


    _gammatizeResolution = _dstResolution;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    double w = _luminanceResolution.x;
    double h = _luminanceResolution.y;
    int lumaLod = glm::floor(glm::log2(glm::max(w, h)));

    // Passes
    _fireFliesPass.reset(new GlProgram());
    _fireFliesPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _fireFliesPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/FireFlies.frag");
    _fireFliesPass->link();
    _fireFliesPass->pushProgram();
    _fireFliesPass->setInt("Source", 0);
    _fireFliesPass->setFloat("Threshold", 1.0f);
    _fireFliesPass->popProgram();

    _blurXPass.reset(new GlProgram());
    _blurXPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _blurXPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/BlurX.frag");
    _blurXPass->link();
    _blurXPass->pushProgram();
    _blurXPass->setInt("Source", 1);
    _blurXPass->popProgram();

    _blurYPass.reset(new GlProgram());
    _blurYPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _blurYPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/BlurY.frag");
    _blurYPass->link();
    _blurYPass->pushProgram();
    _blurYPass->setInt("Source", 2);
    _blurYPass->popProgram();

    _luminancePass.reset(new GlProgram());
    _luminancePass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _luminancePass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Luminance.frag");
    _luminancePass->link();
    _luminancePass->pushProgram();
    _luminancePass->setInt("Source", 1);
    _luminancePass->popProgram();

    _mipmapSumPass.reset(new GlProgram());
    _mipmapSumPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _mipmapSumPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/MipmapSum.frag");
    _mipmapSumPass->link();
    _mipmapSumPass->pushProgram();
    _mipmapSumPass->setInt("LodCount", lumaLod);
    _mipmapSumPass->setInt("Mipmaps", 4);
    _mipmapSumPass->popProgram();

    _tonemappingPass.reset(new GlProgram());
    _tonemappingPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _tonemappingPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Tonemapping.frag");
    _tonemappingPass->link();
    _tonemappingPass->pushProgram();
    _tonemappingPass->setInt("Source", 1);
    _tonemappingPass->setInt("BaseBlur", 3);
    _tonemappingPass->setInt("LumiBlur", 5);
    _tonemappingPass->setFloat("ExposureGain", 1.0f);
    _tonemappingPass->popProgram();

    _gammatizePass.reset(new GlProgram());
    _gammatizePass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _gammatizePass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Gammatize.frag");
    _gammatizePass->link();
    _gammatizePass->pushProgram();
    _gammatizePass->setInt("Source", 6);
    _gammatizePass->popProgram();


    // Make sure film's color buffer is set to albedo
    _film->resizeFrame(_srcResolution);
    _film->colorBuffer(Film::ColorOutput::ALBEDO);

    resize(_dstResolution.x, _dstResolution.y);


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

void Processor::genFramebuffer(
        GLuint& frameId,
        GLuint& texId,
        const glm::ivec2& size,
        bool mipmap)
{
    GLenum minFilt = mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilt);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size.x, size.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    glGenFramebuffers(1, &frameId);
    glBindFramebuffer(GL_FRAMEBUFFER, frameId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texId, 0);
}
