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

    std::shared_ptr<cellar::GlProgram> _bloomBlurPass;
    std::shared_ptr<cellar::GlProgram> _bloomBlendPass;
    std::shared_ptr<cellar::GlProgram> _tonemappingPass;

    GLuint _frameBufferId;
    GLuint _depthBufferId;
    GLuint _colorBufferRawId;
    GLuint _colorBufferTonemapedId;
    GLuint _colorBufferBlurId;
    GLuint _vao;
    GLuint _vbo;
};



// IMPLEMENTATION //
inline std::shared_ptr<cellar::Image> Processor::yield() const
{
    return _image;
}

#endif //ExCompositor_PROCESSOR_H
