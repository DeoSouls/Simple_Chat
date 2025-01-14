
#ifdef __APPLE__
#  include <OpenGLES/ES2/gl.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GLES2/gl2.h>
#endif

int main(int, char **) {
    glUniform1f(1, GLfloat(1.0));
    glClear(GL_COLOR_BUFFER_BIT);
}
