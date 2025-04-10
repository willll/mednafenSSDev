
#include "main.h"
#include "video.h"
#include "opengl.h"

//==============

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

#include "med_imgui.h"
#include "debugui.h"

#include "profiler.h"
#include "elf_parser.h"

GLuint fb_tex_id;

//==============
SDL_Window *window = NULL;
static int med_init = 0;

static void med_init_textures()
{
    glGenTextures(1, &fb_tex_id);

    glBindTexture(GL_TEXTURE_2D, fb_tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void med_imgui_kill()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

static void _med_imgui_debug_register_render()
{
    debugui_cpu_tab_t tab;
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    char *tabs[] = {
        "Master SH2",
        "Slave SH2"};

    if (ImGui::BeginTabBar("Registers", tab_bar_flags))
    {
        for (int tab_n = 0; tab_n < sizeof(tabs) / sizeof(tabs[0]); tab_n++)
        {
            if (ImGui::BeginTabItem(tabs[tab_n]))
            {
                debugui_get_cpu(tab_n, &tab);

                ImGui::BeginTable(tabs[tab_n], 2);

                for (int row = 0; row < tab.regs_count; row++)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text(tab.regs[row].name);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%08x", tab.regs[row].value);
                }

                ImGui::EndTable();
                ImGui::EndTabItem();
            }
        }
    }

    ImGui::EndTabBar();
}

static void _med_imgui_dev_register_render()
{
    debugui_reg_tab_t tab;
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    char *tabs[] = {
        "VDP2 Common"};

    if (ImGui::BeginTabBar("Registers", tab_bar_flags))
    {
        for (int tab_n = 0; tab_n < sizeof(tabs) / sizeof(tabs[0]); tab_n++)
        {
            if (ImGui::BeginTabItem(tabs[tab_n]))
            {
                debugui_get_dev_regs(tab_n, &tab);

                ImGui::BeginTable(tabs[tab_n], 4);
                for (int row = 0; row < tab.regs_count; row++)
                {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", tab.regs[row].adr);

                    ImGui::TableNextColumn();
                    ImGui::Text("%s", tab.regs[row].name);

                    ImGui::TableNextColumn();
                    ImGui::Text(tab.regs[row].dec.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%04x", tab.regs[row].value);
                }

                ImGui::EndTable();
                ImGui::EndTabItem();
            }
        }
    }

    ImGui::EndTabBar();
}

static int last_w;
static int last_h;

extern OpenGL_Blitter *ogl_blitter;

void _med_imgui_get_texture_size(GLuint texture, int &width, int &height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Fonction pour copier une texture
void _med_imgui_copy_texture(GLuint sourceTexture, GLuint destinationTexture)
{
    int width, height;
    // _med_imgui_get_texture_size(sourceTexture, width, height);

    last_w = 906;
    last_h = 693;

    glBindTexture(GL_TEXTURE_2D, destinationTexture);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, last_w, last_h, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void _med_imgui_render_profiler()
{
    ImGui::Begin("prof");

    ImGui::BeginTable("cpu_perf", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg);
    ImGui::TableSetupColumn("line", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("# cycles", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("# call", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();
    dbg_profiler.frame();
    ImGui::EndTable();
    ImGui::End();
    dbg_profiler.reset();
}

static void _med_imgui_render_profiler_item(uint32_t adr, uint64_t cycles_count, uint64_t call_count)
{
    // ImGui::Text("Addr: %08x [%d] -- [%d]", adr, cycles_count, call_count);
    // printf("Addr: %08x [%d]\n", adr, call_count);

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    std::string line;
    if (elf_parser_adr2line(adr, line))
        ImGui::Text(line.c_str());
    else
        ImGui::Text("%08x", adr);

    ImGui::TableNextColumn();
    ImGui::Text("%llu", cycles_count);

    ImGui::TableNextColumn();
    ImGui::Text("%llu", call_count);
}

void med_imgui_render_frame(const MDFN_Surface *src_surface, const MDFN_Rect *src_rect, const MDFN_Rect *dest_rect, const MDFN_Rect *original_src_rect, int InterlaceField, int UsingIP, int rotated)
{
    MDFN_Rect tex_src_rect = *src_rect;
    float src_coords[4][2];
    int dest_coords[4][2];
    unsigned int tmpwidth;
    unsigned int tmpheight;
    const bool ShaderIlace = false;
    const void *src_pixies;

    if (tex_src_rect.w == 0 || tex_src_rect.h == 0 || dest_rect->w == 0 || dest_rect->h == 0 || original_src_rect->w == 0 || original_src_rect->h == 0)
    {
        printf("[BUG] OpenGL blitting nothing? --- %d:%d %d:%d %d:%d\n", tex_src_rect.w, tex_src_rect.h, dest_rect->w, dest_rect->h, original_src_rect->w, original_src_rect->h);
        return;
    }

    if (src_surface->pixels16)
        src_pixies = src_surface->pixels16 + tex_src_rect.x + (tex_src_rect.y + (InterlaceField & ShaderIlace)) * src_surface->pitchinpix;
    else
        src_pixies = src_surface->pixels + tex_src_rect.x + (tex_src_rect.y + (InterlaceField & ShaderIlace)) * src_surface->pitchinpix;
    tex_src_rect.x = 0;
    tex_src_rect.y = 0;
    tex_src_rect.h >>= ShaderIlace;

    glBindTexture(GL_TEXTURE_2D, fb_tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, UsingIP ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, UsingIP ? GL_LINEAR : GL_NEAREST);

    {
        tmpwidth = tex_src_rect.w;
        tmpheight = tex_src_rect.h;

        if (tmpwidth != last_w || tmpheight != last_h)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tmpwidth, tmpheight, 0, ogl_blitter->PixelFormat, ogl_blitter->PixelType, NULL);
            last_w = tmpwidth;
            last_h = tmpheight;
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, src_surface->format.opp);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, src_surface->pitchinpix << ShaderIlace);

    glTexSubImage2D(GL_TEXTURE_2D, 0, tex_src_rect.x, tex_src_rect.y, tex_src_rect.w, tex_src_rect.h, ogl_blitter->PixelFormat, ogl_blitter->PixelType, src_pixies);
}

void med_imgui_render_start()
{
    if (med_init == 0)
        return;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    // draw game
    ImGui::Begin("Emulation");
    // Scale target to fit window
    ImVec2 parent = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)(intptr_t)fb_tex_id, parent);
    ImGui::End();

    // draw debug
    _med_imgui_debug_register_render();
    _med_imgui_dev_register_render();
    _med_imgui_render_profiler();

    glClear(GL_COLOR_BUFFER_BIT);
}

void med_imgui_render_end()
{
    if (med_init == 0)
        return;
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::Render();
    //  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    // glClear(GL_COLOR_BUFFER_BIT);
    // SDL_GL_SwapWindow(window);
}

void med_imgui_process_event(SDL_Event *event)
{
    if (med_init == 0)
        return;
    ImGui_ImplSDL2_ProcessEvent(event);
}

void med_imgui_init(SDL_Window *_window, SDL_GLContext glcontext)
{
    window = _window;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking

    //

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    SDL_GL_MakeCurrent(window, glcontext);

    ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
    ImGui_ImplOpenGL2_Init();

    med_init_textures();

    dbg_profiler.cb = [](uint32_t adr, uint64_t cycles_count, uint64_t call_count)
    { _med_imgui_render_profiler_item(adr, cycles_count, call_count); };

    med_init = 1;
}