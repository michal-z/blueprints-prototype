export module game;
import "base.h";
import std;
import gl;

namespace game {

constexpr int msaa_num_sample = 8;

struct {
  SDL_Window* window;
  int viewport_w;
  int viewport_h;
  GLuint msaa_fbo;
  GLuint msaa_to;
} gstate;

export void init(SDL_Window* window) {
  gstate.window = window;
}

export void update() {
  {
    int w, h;
    SDL_GetWindowSizeInPixels(gstate.window, &w, &h);
    if (w != gstate.viewport_w || h != gstate.viewport_h) {
      glDeleteTextures(1, &gstate.msaa_to);
      glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &gstate.msaa_to);
      glTextureStorage2DMultisample(gstate.msaa_to, msaa_num_sample, GL_RGBA8, w, h, GL_FALSE);
      glClearTexImage(gstate.msaa_to, 0, GL_RGBA, GL_UNSIGNED_BYTE, std::array{0.0f, 0.0f, 0.0f, 0.0f}.data());

      glDeleteFramebuffers(1, &gstate.msaa_fbo);
      glCreateFramebuffers(1, &gstate.msaa_fbo);
      glNamedFramebufferTexture(gstate.msaa_fbo, GL_COLOR_ATTACHMENT0, gstate.msaa_to, 0);
      glClearNamedFramebufferfv(gstate.msaa_fbo, GL_COLOR, 0, std::array{0.0f, 0.0f, 0.0f, 0.0f}.data());

      glViewport(0, 0, w, h);
      gstate.viewport_w = w;
      gstate.viewport_h = h;

      SDL_Log("Window resized: %d x %d", w, h);
    }
  }

  ImGui::ShowDemoWindow(nullptr);

  glBindFramebuffer(GL_FRAMEBUFFER, gstate.msaa_fbo);

  glClearBufferfv(GL_COLOR, 0, std::array{0.2f, 0.4f, 0.8f, 1.0f}.data());

  glBegin(GL_TRIANGLES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex2f(-0.5f, -0.5f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex2f(0.5f, -0.5f);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex2f(0.0f, 0.5f);
  glEnd();

  glLineWidth(15.0f);

  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_LINES);
  glVertex2f(-0.5f, -0.5f);
  glVertex2f(0.75f, 0.35f);
  glEnd();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBlitNamedFramebuffer(gstate.msaa_fbo, 0, 0, 0, gstate.viewport_w, gstate.viewport_h, 0, 0, gstate.viewport_w, gstate.viewport_h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

export void shutdown() {
}

} // namespace game
