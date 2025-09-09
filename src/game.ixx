export module game;
import "base.h";
import std;
import gl;

namespace game {

constexpr int msaa_num_sample = 8;
constexpr float screen_size_y = 12.0f;

struct {
  SDL_Window* window;
  int viewport_w;
  int viewport_h;
  GLuint msaa_fbo;
  GLuint msaa_color_to;
  GLuint msaa_ds_to;

  GLuint test_pho = 1;
} gstate;

export void init(SDL_Window* window) {
  gstate.window = window;

  {
    const GLubyte commands[] = {GL_ROUNDED_RECT_NV};
    const GLfloat coords[] = {-1.0f, -2.0f, 2.0f, 2.0f, 0.5f};
    glPathCommandsNV(gstate.test_pho, 1, commands, 5, GL_FLOAT, coords);
    glPathParameterfNV(gstate.test_pho, GL_PATH_STROKE_WIDTH_NV, 0.1f);
    //glPathParameteriNV(gstate.test_pho, GL_PATH_JOIN_STYLE_NV, GL_ROUND_NV);
  }
}

export void update() {
  if (int w, h; SDL_GetWindowSizeInPixels(gstate.window, &w, &h) && (w != gstate.viewport_w || h != gstate.viewport_h)) {
    glDeleteTextures(1, &gstate.msaa_color_to);
    glDeleteTextures(1, &gstate.msaa_ds_to);
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &gstate.msaa_color_to);
    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &gstate.msaa_ds_to);
    glTextureStorage2DMultisample(gstate.msaa_color_to, msaa_num_sample, GL_SRGB8_ALPHA8, w, h, GL_FALSE);
    glTextureStorage2DMultisample(gstate.msaa_ds_to, msaa_num_sample, GL_DEPTH32F_STENCIL8, w, h, GL_FALSE);

    glDeleteFramebuffers(1, &gstate.msaa_fbo);
    glCreateFramebuffers(1, &gstate.msaa_fbo);
    glNamedFramebufferTexture(gstate.msaa_fbo, GL_COLOR_ATTACHMENT0, gstate.msaa_color_to, 0);
    glNamedFramebufferTexture(gstate.msaa_fbo, GL_DEPTH_STENCIL_ATTACHMENT, gstate.msaa_ds_to, 0);

    glViewport(0, 0, w, h);
    gstate.viewport_w = w;
    gstate.viewport_h = h;

    const float aspect = static_cast<float>(w) / h;
    glMatrixLoadIdentityEXT(GL_PROJECTION);
    glMatrixOrthoEXT(GL_PROJECTION, -0.5f * screen_size_y * aspect, 0.5f * screen_size_y * aspect, -0.5f * screen_size_y, 0.5f * screen_size_y, -1.0f, 1.0f);

    SDL_Log("Window resized: %d x %d", w, h);
  }

  ImGui::ShowDemoWindow(nullptr);

  glBindFramebuffer(GL_FRAMEBUFFER, gstate.msaa_fbo);
  glEnable(GL_FRAMEBUFFER_SRGB);

  glClearNamedFramebufferfv(gstate.msaa_fbo, GL_COLOR, 0, std::array{0.2f, 0.4f, 0.8f, 1.0f}.data());
  glClearNamedFramebufferfi(gstate.msaa_fbo, GL_DEPTH_STENCIL, 0, 1.0f, 0);

  glLineWidth(15.0f);
  glPointSize(45.0f);

  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
  glVertex2f(-8.0f, 5.0f);
  glVertex2f(8.0f, 5.0f);

  glVertex2f(-8.0f, -5.0f);
  glVertex2f(8.0f, -5.0f);
  glEnd();

  glBegin(GL_POINTS);
  glVertex2f(0.0f, 0.0f);
  {
    const float aspect = static_cast<float>(gstate.viewport_w) / gstate.viewport_h;
    glVertex2f(-0.5f * screen_size_y * aspect, -0.5f * screen_size_y);
    glVertex2f(0.5f * screen_size_y * aspect, -0.5f * screen_size_y);
    glVertex2f(-0.5f * screen_size_y * aspect, 0.5f * screen_size_y);
    glVertex2f(0.5f * screen_size_y * aspect, 0.5f * screen_size_y);
  }
  glEnd();

  //glStencilFillPathNV(gstate.test_pho, GL_COUNT_UP_NV, 0x1f);
  glStencilStrokePathNV(gstate.test_pho, GL_COUNT_UP_NV, 0x1f);

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 0, 0x1f);
  glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
  glColor3f(1.0f, 1.0f, 0.0f);
  glCoverStrokePathNV(gstate.test_pho, GL_BOUNDING_BOX_NV);
  glDisable(GL_STENCIL_TEST);

  glDisable(GL_FRAMEBUFFER_SRGB);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBlitNamedFramebuffer(gstate.msaa_fbo, 0, 0, 0, gstate.viewport_w, gstate.viewport_h, 0, 0, gstate.viewport_w, gstate.viewport_h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

export void shutdown() {
  glDeleteTextures(1, &gstate.msaa_color_to);
  glDeleteTextures(1, &gstate.msaa_ds_to);
  glDeleteFramebuffers(1, &gstate.msaa_fbo);
}

} // namespace game
