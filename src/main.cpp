import "base.h";
import std;
import gl;

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_Window* window = SDL_CreateWindow("blueprints-prototype", 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(0);

  gl::init();

  SDL_ShowWindow(window);

  SDL_Log("GL version: %s", (const char*)glGetString(GL_VERSION));
  SDL_Log("GL renderer: %s", (const char*)glGetString(GL_RENDERER));
  SDL_Log("GL vendor: %s", (const char*)glGetString(GL_VENDOR));

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  const float window_scale = SDL_GetWindowDisplayScale(window);
  ImGui::GetIO().Fonts->AddFontFromFileTTF("content/Roboto-Medium.ttf", SDL_floorf(15.0f * window_scale));
  ImGui::GetStyle().ScaleAllSizes(window_scale);

  glClearColor(0.2f, 0.4f, 0.8f, 1.0f);

  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL2_Init();

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);

      switch (event.type) {
        case SDL_EVENT_QUIT: {
          running = false;
        } break;
        case SDL_EVENT_KEY_DOWN: {
          if (event.key.scancode == SDL_SCANCODE_ESCAPE)
            running = false;
        } break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
          if (event.window.windowID == SDL_GetWindowID(window)) running = false;
        } break;
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
          const int viewport_w = event.window.data1;
          const int viewport_h = event.window.data2;
          SDL_Log("Window resized: %d x %d", viewport_w, viewport_h);

          glViewport(0, 0, viewport_w, viewport_h);
        } break;
      }
    }

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

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

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    assert(glGetError() == GL_NO_ERROR);

    SDL_GL_SwapWindow(window);
  }

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
