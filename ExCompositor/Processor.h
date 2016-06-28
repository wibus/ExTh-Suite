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

    virtual void genFramebuffer(
            GLuint& frameId,
            GLuint& texId,
            const glm::ivec2& size,
            GLenum coponents,
            bool mipmap);

private:
    glm::ivec2 _srcResolution;
    glm::ivec2 _dstResolution;
    std::shared_ptr<prop3::Film> _film;
    std::shared_ptr<cellar::Image> _image;

    glm::ivec2 _denoiseResolution;
    glm::ivec2 _preprocessResolution;
    glm::ivec2 _bloomMipSumResolution;
    glm::ivec2 _lumaMipSumResolution;
    glm::ivec2 _tonemappingResolution;
    glm::ivec2 _gammatizeResolution;

    std::shared_ptr<cellar::GlProgram> _denoisePass;
    std::shared_ptr<cellar::GlProgram> _preprocessPass;
    std::shared_ptr<cellar::GlProgram> _bloomMipSumPass;
    std::shared_ptr<cellar::GlProgram> _lumaMipSumPass;
    std::shared_ptr<cellar::GlProgram> _tonemappingPass;
    std::shared_ptr<cellar::GlProgram> _gammatizePass;

    GLuint _colorBufferSrcId;

    GLuint _colorBufferDenoiseId;
    GLuint _frameBufferDenoiseId;
    GLuint _colorBufferPreprocessId;
    GLuint _frameBufferPreprocessId;
    GLuint _colorBufferBloomMipSumId;
    GLuint _frameBufferBloomMipSumId;
    GLuint _colorBufferLumaMipSumId;
    GLuint _frameBufferLumaMipSumId;
    GLuint _colorBufferTonemappingId;
    GLuint _frameBufferTonemappingId;

    GLuint _vao;
    GLuint _vbo;
};



// IMPLEMENTATION //
inline std::shared_ptr<cellar::Image> Processor::yield() const
{
    return _image;
}

#endif //ExCompositor_PROCESSOR_H
