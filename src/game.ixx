export module game;
import "base.h";
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
  SDL_GetWindowSizeInPixels(window, &gstate.viewport_w, &gstate.viewport_h);
  gstate.window = window;

  glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &gstate.msaa_to);
  glTextureStorage2DMultisample(gstate.msaa_to, msaa_num_sample, GL_RGBA8, gstate.viewport_w, gstate.viewport_h, GL_FALSE);
}

export void update() {
  ImGui::ShowDemoWindow(nullptr);

  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_TRIANGLES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex2f(-0.5f, -0.5f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex2f(0.5f, -0.5f);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex2f(0.0f, 0.5f);
  glEnd();
}

export void shutdown() {
}

} // namespace game
