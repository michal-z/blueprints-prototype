export module gl;

import "base.h";

export PFNGLGETSTRINGPROC glGetString;
export PFNGLCLEARCOLORPROC glClearColor;
export PFNGLVIEWPORTPROC glViewport;
export PFNGLCLEARPROC glClear;
export PFNGLBEGINPROC glBegin;
export PFNGLENDPROC glEnd;
export PFNGLVERTEX2FPROC glVertex2f;
export PFNGLCOLOR3FPROC glColor3f;
export PFNGLGETERRORPROC glGetError;

namespace gl {

export void init() {

#define GET_FUNC(type, name) \
  name = reinterpret_cast<type>(SDL_GL_GetProcAddress(#name)); \
  assert(name)

  GET_FUNC(PFNGLGETSTRINGPROC, glGetString);
  GET_FUNC(PFNGLCLEARCOLORPROC, glClearColor);
  GET_FUNC(PFNGLVIEWPORTPROC, glViewport);
  GET_FUNC(PFNGLCLEARPROC, glClear);
  GET_FUNC(PFNGLBEGINPROC, glBegin);
  GET_FUNC(PFNGLENDPROC, glEnd);
  GET_FUNC(PFNGLVERTEX2FPROC, glVertex2f);
  GET_FUNC(PFNGLCOLOR3FPROC, glColor3f);
  GET_FUNC(PFNGLGETERRORPROC, glGetError);

#undef GET_FUNC
}

} // namespace gl
