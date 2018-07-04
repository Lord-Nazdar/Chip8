
#include <GL/gl3w.h>
#include "chip8.h"
#include "program-reader.h"
#include <iostream>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"

#include <thread>
#include <filesystem>
#include <map>

static MemoryEditor mem_edit_1;

static uint8_t PrevV[15];

static bool FreeRunning = false;


void ShowRegisterWindow(bool *open, const CHIP8_INFO &info) {
    ImGui::Begin("Registers", open);

    ImGui::Text("Current location: 0x%02x", info.PC);
    ImGui::Text("Current opcode: 0x%02x", info.Opcode);
    ImGui::Text("VF: 0x%02x", info.VF);
    ImGui::Columns(15, "vregisters", true);
    ImGui::Separator();
    for (uint8_t i = 0; i < 15; ++i) {
        char label[32];
        if (info.V[i] != PrevV[i]) {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));

            sprintf(label, "V%u\n0x%02x", i, info.V[i]);
            ImGui::Text(label);
            ImGui::PopStyleColor(1);
        }
        else {

            sprintf(label, "V%u\n0x%02x", i, info.V[i]);
            ImGui::Text(label);
        }
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::End();
}

static bool PrevScreen[64][32];

void ShowScreen(bool *open, const CHIP8 &chp) {
    ImGui::Begin("Screen", open);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    static ImVec4 black = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    static ImVec4 white = ImVec4(0.1f, 0.1f, 0.1f, 0.2f);
    const ImVec2 p = ImGui::GetCursorScreenPos();

    double width = 10.0f;

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            draw_list->AddRectFilled(
                ImVec2(p.x + x * (width), p.y + y * (width)),
                ImVec2(p.x + x * (width)+8.0f, p.y + y * (width)+8.0f),
                ImColor(chp.screen[x][y] ? black : ImColor(PrevScreen[x][y] ? black : white)),
                0.0f
            );
        }
    }
    ImGui::End();
}

CHIP8 chp;

void CHIP8Loop() {
    auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        if (FreeRunning) {
            std::chrono::duration<float> temp = std::chrono::high_resolution_clock::now() - start;
            chp.Tick(temp.count());
        }
        std::this_thread::sleep_for(std::chrono::microseconds(200));
    }
}

static std::map<int, E_KEYS> KeyMapping;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        chp.Keys[KeyMapping[key]] = action;
    }
}

int main(void)
{
    KeyMapping[GLFW_KEY_X] = E_KEYS::KEY_0;
    KeyMapping[GLFW_KEY_1] = E_KEYS::KEY_1;
    KeyMapping[GLFW_KEY_2] = E_KEYS::KEY_2;
    KeyMapping[GLFW_KEY_3] = E_KEYS::KEY_3;
    KeyMapping[GLFW_KEY_A] = E_KEYS::KEY_4;
    KeyMapping[GLFW_KEY_Z] = E_KEYS::KEY_5;
    KeyMapping[GLFW_KEY_E] = E_KEYS::KEY_6;
    KeyMapping[GLFW_KEY_Q] = E_KEYS::KEY_7;
    KeyMapping[GLFW_KEY_S] = E_KEYS::KEY_8;
    KeyMapping[GLFW_KEY_D] = E_KEYS::KEY_9;
    KeyMapping[GLFW_KEY_W] = E_KEYS::KEY_A;
    KeyMapping[GLFW_KEY_C] = E_KEYS::KEY_B;
    KeyMapping[GLFW_KEY_4] = E_KEYS::KEY_C;
    KeyMapping[GLFW_KEY_R] = E_KEYS::KEY_D;
    KeyMapping[GLFW_KEY_F] = E_KEYS::KEY_E;
    KeyMapping[GLFW_KEY_V] = E_KEYS::KEY_F;

    // CHIP8 chip8;
    
    // chip8.Init();
    // while (chip8.Cycle()){}
    ProgramReader pr;
    pr.Load("PONG.ch8");
    for (uint16_t i = 0; i < pr.Program.size(); ++i) {
        chp.memory[0x200+i] = pr.Program[i];
    }
    chp.Init();

    //GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    //window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    //if (!window)
    //{
    //    glfwTerminate();
    //    return -1;
    //}
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "CHIP8 Emu", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    /* Make the window's context current */
    glfwMakeContextCurrent(window);




    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Setup style
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();

    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.1f, 0.55f, 0.60f, 1.00f);
    auto start = std::chrono::system_clock::now();

    std::thread t(&CHIP8Loop);

    glfwSetKeyCallback(window, key_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {


        auto info = chp.GetInfo();


        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            chp.DataGuard.lock();
            if (show_another_window) {
                ShowRegisterWindow(&show_another_window, info);
            }


            if (show_another_window) {
                ShowScreen(&show_another_window, chp);
            }
            chp.DataGuard.unlock();
        }

        ImGui::ShowDemoWindow(&show_demo_window);
        mem_edit_1.HighlightMin = info.PC;
        mem_edit_1.HighlightMax = info.PC + 2;
        mem_edit_1.HighlightColor = IM_COL32(255, 0, 0, 90);
        mem_edit_1.DrawWindow("Memory Editor", chp.memory, 4096, 0x0000);

        ImGui::Begin("Controls");
        if (ImGui::Button("Step")) {
            for (uint8_t i = 0; i < 15; ++i) {
                PrevV[i] = info.V[i];
            }
            chp.Cycle();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            FreeRunning = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Run")) {
            FreeRunning = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Restart")) {
            chp.Init();
        }
        ImGui::SliderFloat("Clock Speed", &chp.ClockSpeed, 0.0f, 0.4f, "%.8f");
        ImGui::End();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                std::string path = "./GAMES";
                for (auto & p : std::experimental::filesystem::directory_iterator(path)) {
                    auto path = p.path();
                    if (ImGui::MenuItem(path.string().c_str())) {
                        chp.ClearMemory();
                        pr.Load(path.string());
                        for (uint16_t i = 0; i < pr.Program.size(); ++i) {
                            chp.memory[0x200 + i] = pr.Program[i];
                        }
                        chp.Init();
                    }
                }

                if (ImGui::MenuItem("PONG")) {
                    chp.ClearMemory();
                    pr.Load("PONG.ch8");
                    for (uint16_t i = 0; i < pr.Program.size(); ++i) {
                        chp.memory[0x200 + i] = pr.Program[i];
                    }
                    chp.Init();
                }

                if (ImGui::MenuItem("MAZE")) {
                    chp.ClearMemory();
                    pr.Load("MAZE.ch8");
                    for (uint16_t i = 0; i < pr.Program.size(); ++i) {
                        chp.memory[0x200 + i] = pr.Program[i];
                    }
                    chp.Init();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.9f, 0.9f, 0.9f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    t.join();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//int main(int argc, char**argv) {
//    // CHIP8 chip8;
//
//    // chip8.Init();
//    // while (chip8.Cycle()){}
//    ProgramReader pr;
//    pr.Load("MAZE.ch8");
//
//    CHIP8 chp;
//    for (uint16_t i = 0; i < pr.Program.size(); ++i) {
//        chp.memory[0x200+i] = pr.Program[i];
//    }
//
//    chp.Init();
//
//    for (int i = 0; i < 1024; ++i) {
//        if(!chp.Cycle())
//            break;
//        std::cin.get();
//    }
//
//    chp.PrintScreen();
//
//    return 0;
//}
