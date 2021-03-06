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
    _tonePersistence(0.0),
    _bloomPersistence(0.0),
    shading()
{

}

Processor::~Processor()
{

}

bool Processor::feed(const std::string& filmName, bool useLastAsRef)
{
    if(useLastAsRef)
        _film->backupAsReferenceShot();
    else
        _film->clearReferenceShot();

    if(!_film->loadRawFilm(filmName))
    {
        getLog().postMessage(new Message('E', false,
            "Film not found: " + filmName, "Processor"));

        return false;
    }

    render();

    return true;
}

void Processor::denoiseThresholdChanged(double threshold)
{
    shading["DenoiseThreshold"] = threshold;

    _denoisePass->pushProgram();
    _denoisePass->setFloat("Threshold", threshold);
    _denoisePass->popProgram();

    render();
}

void Processor::preExposureChanged(double exposure)
{
    shading["Pre-Exposure"] = exposure;

    _preprocessPass->pushProgram();
    _preprocessPass->setFloat("PreExpose", exposure);
    _preprocessPass->popProgram();

    render();
}

void Processor::aberrationChanged(double aberration)
{
    shading["Aberration"] = aberration;

    _preprocessPass->pushProgram();
    _preprocessPass->setFloat("Aberration", aberration);
    _preprocessPass->popProgram();

    render();
}

void Processor::relaxationChanged(double relaxation)
{
    shading["Relaxation"] = relaxation;

    _mipmapSumPass->pushProgram();
    _mipmapSumPass->setFloat("Relaxation", relaxation);
    _mipmapSumPass->popProgram();

    render();
}

void Processor::tonePersistenceChanged(double persistence)
{
    _tonePersistence = persistence;
    shading["Tone-Persistence"] = persistence;

    render();
}

void Processor::bloomPersistenceChanged(double persistence)
{
    _bloomPersistence = persistence;
    shading["Bloom-Persistence"] = persistence;

    render();
}

void Processor::postExposureChanged(double exposure)
{
    shading["Post-Exposure"] = exposure;

    _tonemappingPass->pushProgram();
    _tonemappingPass->setFloat("ExposureGain", exposure);
    _tonemappingPass->popProgram();

    render();
}

void Processor::bloomChanged(double bloom)
{
    shading["Bloom"] = bloom;

    _tonemappingPass->pushProgram();
    _tonemappingPass->setFloat("BloomGain", bloom);
    _tonemappingPass->popProgram();

    render();
}

void Processor::gammaChanged(double gamma)
{
    shading["Gamma"] = gamma;

    _gammatizePass->pushProgram();
    _gammatizePass->setFloat("Gamma", gamma);
    _gammatizePass->popProgram();

    render();
}

// QGLWidget interface
void Processor::initializeGL()
{
    _film.reset(new ConvergentFilm());
    _image.reset(new Image());

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


    // Denoise
    _denoiseResolution = _srcResolution;
    genFramebuffer(
        _frameBufferDenoiseId,
        _colorBufferDenoiseId,
        _denoiseResolution,
        GL_RGB32F,
        false);


    // Preprocessing
    _preprocessResolution = _dstResolution;
    genFramebuffer(
        _frameBufferPreprocessId,
        _colorBufferPreprocessId,
        _preprocessResolution,
        GL_RGBA32F,
        true);


    // Mipmap blur
    _mipmapBlurResolution = _dstResolution;
    genFramebuffer(
        _frameBufferMipmapBlurId,
        _colorBufferMipmapBlurId,
        _mipmapBlurResolution,
        GL_RGBA32F,
        true);


    // Mipmap sum
    _mipmapSumResolution = _dstResolution;
    genFramebuffer(
        _frameBufferMipmapSumId,
        _colorBufferMipmapSumId,
        _mipmapSumResolution,
        GL_RGBA32F,
        false);


    // Tonemapping
    _tonemappingResolution = _dstResolution;
    genFramebuffer(
        _frameBufferTonemappingId,
        _colorBufferTonemappingId,
        _tonemappingResolution,
        GL_RGB32F,
        false);


    // Gammatize
    _gammatizeResolution = _dstResolution;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);


    double baseW = _preprocessResolution.x;
    double baseH = _preprocessResolution.y;
    _mipmapLodCount = 1 + glm::floor(glm::log2(glm::max(baseW, baseH)));


    // Passes
    _denoisePass.reset(new GlProgram());
    _denoisePass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _denoisePass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Denoise.frag");
    _denoisePass->link();
    _denoisePass->pushProgram();
    _denoisePass->setInt("Source", 0);
    _denoisePass->popProgram();

    _preprocessPass.reset(new GlProgram());
    _preprocessPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _preprocessPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Preprocess.frag");
    _preprocessPass->link();
    _preprocessPass->pushProgram();
    _preprocessPass->setInt("Source", 1);
    _preprocessPass->popProgram();

    _mipmapBlurPass.reset(new GlProgram());
    _mipmapBlurPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _mipmapBlurPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/MipmapBlur.frag");
    _mipmapBlurPass->link();
    _mipmapBlurPass->pushProgram();
    _mipmapBlurPass->setInt("Source", 2);
    _mipmapBlurPass->setInt("CurrentLod", 0);
    _mipmapBlurPass->popProgram();

    _mipmapSumPass.reset(new GlProgram());
    _mipmapSumPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _mipmapSumPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/MipmapSum.frag");
    _mipmapSumPass->link();
    _mipmapSumPass->pushProgram();
    _mipmapSumPass->setInt("Source", 3);
    _mipmapSumPass->setInt("LodCount", _mipmapLodCount);
    _mipmapSumPass->popProgram();

    _tonemappingPass.reset(new GlProgram());
    _tonemappingPass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _tonemappingPass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Tonemapping.frag");
    _tonemappingPass->link();
    _tonemappingPass->pushProgram();
    _tonemappingPass->setInt("Source", 2);
    _tonemappingPass->setInt("MipmapSum", 4);
    _tonemappingPass->popProgram();

    _gammatizePass.reset(new GlProgram());
    _gammatizePass->addShader(GL_VERTEX_SHADER, ":/ExCompositor/shaders/Fullscreen.vert");
    _gammatizePass->addShader(GL_FRAGMENT_SHADER, ":/ExCompositor/shaders/Gammatize.frag");
    _gammatizePass->link();
    _gammatizePass->pushProgram();
    _gammatizePass->setInt("Source", 5);
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

void Processor::render()
{
    const std::vector<glm::vec3>& color =
        _film->colorBuffer(Film::ColorOutput::ALBEDO);

    glBindTexture(GL_TEXTURE_2D, _colorBufferSrcId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _srcResolution.x, _srcResolution.y,
                 0, GL_RGB, GL_FLOAT, color.data());


    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _colorBufferTonemappingId);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _colorBufferMipmapSumId);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _colorBufferMipmapBlurId);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _colorBufferPreprocessId);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _colorBufferDenoiseId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _colorBufferSrcId);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(_vao);



    // Denoise
    _denoisePass->pushProgram();
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferDenoiseId);
    glViewport(0, 0, _denoiseResolution.x, _denoiseResolution.y);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _denoisePass->popProgram();


    // Preprocessing
    _preprocessPass->pushProgram();
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferPreprocessId);
    glViewport(0, 0, _preprocessResolution.x, _preprocessResolution.y);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _preprocessPass->popProgram();
    glGenerateTextureMipmap(_colorBufferPreprocessId);


    // Mipmap blur
    _mipmapBlurPass->pushProgram();
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferMipmapBlurId);

    glEnable(GL_BLEND);
    double tonePersist = _tonePersistence;
    double bloomPersist = _bloomPersistence;
    glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_COLOR);
    glm::ivec2 lodDimensions = _mipmapBlurResolution;
    for(int lod=0; lod < _mipmapLodCount; ++lod)
    {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, _colorBufferMipmapBlurId, lod);

        glBlendColor(bloomPersist, bloomPersist, bloomPersist, tonePersist);

        glViewport(0, 0, lodDimensions.x, lodDimensions.y);
        _mipmapBlurPass->setInt("CurrentLod", lod);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        tonePersist = 1.0 - glm::pow(1.0 - _tonePersistence, lod + 2.0);
        bloomPersist = 1.0 - glm::pow(1.0 - _bloomPersistence, lod + 2.0);
        lodDimensions = glm::max(lodDimensions / 2, glm::ivec2(1));
    }
    glDisable(GL_BLEND);



    _mipmapBlurPass->popProgram();


    // Mipmap sum
    _mipmapSumPass->pushProgram();
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferMipmapSumId);
    glViewport(0, 0, _mipmapSumResolution.x, _mipmapSumResolution.y);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _mipmapSumPass->popProgram();



    // Tonemapping
    _tonemappingPass->pushProgram();
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferTonemappingId);
    glViewport(0, 0, _tonemappingResolution.x, _tonemappingResolution.y);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _tonemappingPass->popProgram();


    // Gammatize
    _gammatizePass->pushProgram();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _gammatizeResolution.x, _gammatizeResolution.y);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    _gammatizePass->popProgram();
//*/


    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    GlToolkit::takeFramebufferShot(*_image);

    swapBuffers();
}

void Processor::genFramebuffer(
        GLuint& frameId,
        GLuint& texId,
        const glm::ivec2& size,
        GLenum coponents,
        bool mipmap)
{
    GLenum minFilt = mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilt);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, coponents, size.x, size.y,
                 0, GL_RGB, GL_FLOAT, nullptr);

    if(mipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glGenFramebuffers(1, &frameId);
    glBindFramebuffer(GL_FRAMEBUFFER, frameId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texId, 0);
}
