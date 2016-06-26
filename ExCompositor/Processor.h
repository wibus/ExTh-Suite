#ifndef ExCompositor_PROCESSOR_H
#define ExCompositor_PROCESSOR_H

#include <memory>

#include <GLM/glm.hpp>

#include <GL3/gl3w.h>

#include <QGLWidget>

namespace cellar
{
    class Image;
    class GlProgram;
}

namespace prop3
{
    class Film;
}


class Processor : public QGLWidget
{
    Q_OBJECT

public:
    Processor(const glm::ivec2& srcResolution,
              const glm::ivec2& dstResolution);
    virtual ~Processor();

    virtual bool feed(const std::string& filmName, bool useLastAsRef = true);

    virtual std::shared_ptr<cellar::Image> yield() const;

protected:
    // QGLWidget interface
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
    glm::ivec2 _srcResolution;
    glm::ivec2 _dstResolution;
    std::shared_ptr<prop3::Film> _film;
    std::shared_ptr<cellar::Image> _image;

    glm::ivec2 _fireFliesResolution;
    glm::ivec2 _luminanceResolution;
    glm::ivec2 _tonemappingResolution;
    glm::ivec2 _bloomBlurResolution;
    glm::ivec2 _bloomBlendResolution;
    glm::ivec2 _gammatizeResolution;

    std::shared_ptr<cellar::GlProgram> _fireFliesPass;
    std::shared_ptr<cellar::GlProgram> _luminancePass;
    std::shared_ptr<cellar::GlProgram> _tonemappingPass;
    std::shared_ptr<cellar::GlProgram> _bloomBlurPass;
    std::shared_ptr<cellar::GlProgram> _bloomBlendPass;
    std::shared_ptr<cellar::GlProgram> _gammatizePass;

    GLuint _colorBufferSrcId;

    GLuint _colorBufferFireFliesId;
    GLuint _frameBufferFireFliesId;
    GLuint _colorBufferLuminanceId;
    GLuint _frameBufferLuminanceId;
    GLuint _colorBufferTonemappedId;
    GLuint _frameBufferTonemapedId;
    GLuint _colorBufferBloomBlurId;
    GLuint _frameBufferBloomBlurId;
    GLuint _colorBufferBloomBlendId;
    GLuint _frameBufferBloomBlendId;

    GLuint _vao;
    GLuint _vbo;
};



// IMPLEMENTATION //
inline std::shared_ptr<cellar::Image> Processor::yield() const
{
    return _image;
}

#endif //ExCompositor_PROCESSOR_H
