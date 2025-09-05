import "base.h";
import gl;
import game;

#ifdef _WIN32
extern "C" __declspec(dllexport) unsigned int NvOptimusEnablement = 1;
#endif

static float update_frame_stats(SDL_Window* window, const char* name) {
  static double previous_time = -1.0;
  static double header_refresh_time = 0.0;
  static int num_frames = 0;

  if (previous_time < 0.0) {
    previous_time = SDL_NS_TO_SECONDS(static_cast<double>(SDL_GetTicksNS()));
    header_refresh_time = previous_time;
  }

  const double time = SDL_NS_TO_SECONDS(static_cast<double>(SDL_GetTicksNS()));
  const float delta_time = static_cast<float>(time - previous_time);
  previous_time = time;

  if ((time - header_refresh_time) >= 1.0) {
    const double fps = num_frames / (time - header_refresh_time);
    const double ms = (1.0 / fps) * 1000.0;
    char header[128];
    SDL_snprintf(header, sizeof(header), "[%.1f fps  %.3f ms] %s", fps, ms, name);
    SDL_SetWindowTitle(window, header);
    header_refresh_time = time;
    num_frames = 0;
  }
  num_frames++;
  return delta_time;
}

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

  SDL_Log("GL version: %s", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  SDL_Log("GL renderer: %s", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
  SDL_Log("GL vendor: %s", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));

  {
    const bool has_path_rendering = SDL_GL_ExtensionSupported("GL_NV_path_rendering");
    const bool has_mesh_shader = SDL_GL_ExtensionSupported("GL_NV_mesh_shader");

    if (has_path_rendering == false || has_mesh_shader == false) {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unsupported GPU", "Sorry but this application requires modern NVIDIA GPU with latest graphics drivers.", window);
      SDL_GL_DestroyContext(gl_context);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }
  }

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui::GetIO().Fonts->AddFontFromFileTTF("content/Roboto-Medium.ttf", 15.0f);
  const float window_scale = SDL_GetWindowDisplayScale(window);
  ImGui::GetStyle().ScaleAllSizes(window_scale);
  ImGui::GetStyle().FontScaleDpi = window_scale;

  glClearColor(0.2f, 0.4f, 0.8f, 1.0f);

  ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL2_Init();

  game::init(window);

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
          const int w = event.window.data1;
          const int h = event.window.data2;
          SDL_Log("Window resized: %d x %d", w, h);

          glViewport(0, 0, w, h);
        } break;
      }
    }
    update_frame_stats(window, "blueprints-prototype");

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    game::update();

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    SDL_assert(glGetError() == GL_NO_ERROR);

    SDL_GL_SwapWindow(window);
  }

  game::shutdown();

  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
