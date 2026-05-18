
#include "main.h"

#include <inttypes.h>
#include "video.h"
#include "opengl.h"

//==============

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"

#include "med_imgui.h"
#include "med_imgui_logs.h"
#include "debugui.h"

#include "profiler.h"
#include "elf_parser.h"

#include <mednafen/FileStream.h>

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>

GLuint fb_tex_id;

//==============
SDL_Window *window = NULL;
static int med_init = 0;

static float med_imgui_get_ui_scale()
{
    int ww = 0, wh = 0;
    int dw = 0, dh = 0;
    float scale = 1.0f;

    SDL_GetWindowSize(window, &ww, &wh);
    SDL_GL_GetDrawableSize(window, &dw, &dh);

    if (ww > 0 && wh > 0 && dw > 0 && dh > 0)
    {
        const float sx = (float)dw / (float)ww;
        const float sy = (float)dh / (float)wh;
        scale = std::max(1.0f, std::max(sx, sy));
    }

    const int dindex = SDL_GetWindowDisplayIndex(window);
    if (dindex >= 0)
    {
        float ddpi = 0.0f;
        if (SDL_GetDisplayDPI(dindex, &ddpi, NULL, NULL) == 0 && ddpi > 0.0f)
            scale = std::max(scale, ddpi / 96.0f);
    }

    return std::min(scale, 2.5f);
}

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

    const char *tabs[] = {
        "Master SH2",
        "Slave SH2"};

    const bool game_loaded = (CurGame != nullptr);
    const bool is_paused   = IsGameLoopPaused();

    if (!game_loaded || !is_paused)
        ImGui::BeginDisabled();
    if (ImGui::Button("Play"))
        SendCEventToGT(CEVT_EMU_RUN, NULL, NULL);
    if (!game_loaded || !is_paused)
        ImGui::EndDisabled();

    ImGui::SameLine();

    if (!game_loaded || is_paused)
        ImGui::BeginDisabled();
    if (ImGui::Button("Pause"))
        SendCEventToGT(CEVT_EMU_PAUSE, NULL, NULL);
    if (!game_loaded || is_paused)
        ImGui::EndDisabled();

    ImGui::SameLine();

    if (!game_loaded)
        ImGui::BeginDisabled();
    if (ImGui::Button("Reset"))
        SendCEventToGT(CEVT_EMU_RESET, NULL, NULL);
    if (!game_loaded)
        ImGui::EndDisabled();

    ImGui::SameLine();
    if (!game_loaded)
        ImGui::TextDisabled("(no game)");
    else if (is_paused)
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.1f, 1.0f), "Paused");
    else
        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "Running");

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
                    ImGui::Text("%s", tab.regs[row].name);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%08x", tab.regs[row].value);
                }

                ImGui::EndTable();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

static void _med_imgui_dev_register_render()
{
    debugui_reg_tab_t tab;
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    const char *tabs[] = {
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
                    ImGui::Text("%s", tab.regs[row].dec.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%04x", tab.regs[row].value);
                }

                ImGui::EndTable();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
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

// Assuming you have a buffer of logs
std::vector<std::string> logBuffer;
std::string logLine;

// Function to add a log to the buffer
void med_imgui_render_logs_addLog(const unsigned char* log) {
    if (*log == '\n') {
        logBuffer.push_back(logLine);
        logLine.clear();
    } else {
        logLine += *log;
    }
}

// Example function to render the logs window
static void _med_imgui_render_logs()
{
    // Simple console window with logs
    static bool show_logs = true;
    static bool auto_follow = true;
    static size_t prev_log_count = 0;
    static size_t prev_partial_len = 0;

    if (show_logs) {
        ImGui::Begin("Logs", &show_logs);
        ImGui::Checkbox("Auto-follow", &auto_follow);
        ImGui::BeginChild("LogsChild");

        // Only keep following output when the user is already at the bottom.
        const float scroll_max_before = ImGui::GetScrollMaxY();
        const bool was_at_bottom = (scroll_max_before <= 0.0f) || (ImGui::GetScrollY() >= (scroll_max_before - 1.0f));
        const bool has_new_content = (logBuffer.size() != prev_log_count) || (logLine.size() != prev_partial_len);

        // Display the entire log buffer
        for (const auto& log : logBuffer) {
            ImGui::Text("%s", log.c_str());
        }
        
        // Display any incomplete line instantly
        if (!logLine.empty()) {
            ImGui::Text("%s", logLine.c_str());
        }

        if (auto_follow && has_new_content && was_at_bottom) {
            ImGui::SetScrollHereY(1.0f);
        }

        prev_log_count = logBuffer.size();
        prev_partial_len = logLine.size();
        ImGui::EndChild();
        ImGui::End();
    }
}


static void _med_imgui_render_profiler()
{
    {
        if (ImGui::Button("Reset")) {
            dbg_profiler.reset();
        }
        // ImGui::SameLine(); 
        // if (ImGui::Button("Pause")) {
        //     MDFN_IEN_SS::SS_Toggle();
        // }
    }
    {
        if (ImGui::BeginTable("cpu_perf", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("line", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("# cycles", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("# call", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            // if (ImGui::TableGetSortSpecs())
            // {
            //     ImGuiTableSortSpecs *sortSpecs = ImGui::TableGetSortSpecs();
            //     int sortColumn = -1;
            //     bool sortAscending = true;

            //     for (int i = 0; i < sortSpecs->SpecsCount; i++)
            //     {
            //         // ImGuiTableColumnSortSpecs *spec = &sortSpecs->Specs[i];
            //         // if (spec->ColumnUserID == 0)
            //         // { // Assuming 0 as the ID for your sortable column
            //         //     sortColumn = spec->ColumnIndex;
            //         //     sortAscending = (spec->SortOrder == 0);
            //         //     SortTable(rows, sortColumn, sortAscending);
            //         //     break;
            //         // }
            //     }
            // }

            dbg_profiler.frame();
        }
        ImGui::EndTable();

        //dbg_profiler.reset();
    }
}

static void _med_imgui_render_profiler_item(uint32_t adr, uint64_t cycles_count, uint64_t call_count)
{
    // ImGui::Text("Addr: %08x [%d] -- [%d]", adr, cycles_count, call_count);
    // printf("Addr: %08x [%d]\n", adr, call_count);

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    std::string line;
    if (elf_parser_adr2line(adr, line))
        ImGui::Text("%s", line.c_str());
    else
        ImGui::Text("%#08x", adr);

    ImGui::TableNextColumn();
    ImGui::Text("%" PRIu64, cycles_count);

    ImGui::TableNextColumn();
    ImGui::Text("%" PRIu64, call_count);
}

static bool _med_parse_hex_address(const char* text, uint64& out)
{
    if (!text)
        return false;

    while (*text && std::isspace(static_cast<unsigned char>(*text)))
        text++;

    if (!*text)
        return false;

    errno = 0;
    char* end = nullptr;
    unsigned long long v = std::strtoull(text, &end, 16);
    if (errno != 0 || end == text)
        return false;

    while (*end)
    {
        if (!std::isspace(static_cast<unsigned char>(*end)))
            return false;
        end++;
    }

    out = static_cast<uint64>(v);
    return true;
}

static bool _med_parse_hex_bytes(const char* text, std::vector<uint8>& out)
{
    out.clear();
    if (!text)
        return false;

    int nib_count = 0;
    uint8 cur = 0;

    for (const char* p = text; *p; p++)
    {
        const unsigned char ch = static_cast<unsigned char>(*p);
        if (std::isspace(ch))
            continue;

        int val = -1;
        if (ch >= '0' && ch <= '9')
            val = ch - '0';
        else if (ch >= 'a' && ch <= 'f')
            val = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            val = ch - 'A' + 10;
        else
            return false;

        if ((nib_count & 1) == 0)
            cur = static_cast<uint8>(val << 4);
        else
            out.push_back(static_cast<uint8>(cur | val));

        nib_count++;
    }

    return (nib_count > 0) && ((nib_count & 1) == 0);
}

static void _med_imgui_render_memory()
{
    static int selected_aspace = 0;
    static uint64 base_addr = 0;
    static int rows = 32;
    static char goto_buf[64] = "0";
    static char search_buf[256] = "";
    static char save_path[256] = "/tmp/mednafen_memdump.bin";
    static int save_size = 0x1000;
    static std::string status;

    if (!CurGame || !CurGame->Debugger || !CurGame->Debugger->AddressSpaces || CurGame->Debugger->AddressSpaces->empty())
    {
        ImGui::TextDisabled("No debugger memory address space available.");
        return;
    }

    auto& aspaces = *CurGame->Debugger->AddressSpaces;
    if (selected_aspace < 0)
        selected_aspace = 0;
    if (selected_aspace >= static_cast<int>(aspaces.size()))
        selected_aspace = static_cast<int>(aspaces.size()) - 1;

    const AddressSpaceType& aspace = aspaces[selected_aspace];
    const uint64 mem_size = std::max<uint64>(1, aspace.size);

    if (base_addr >= mem_size)
        base_addr %= mem_size;

    ImGui::Text("Address space size: 0x%llX", static_cast<unsigned long long>(mem_size));
    if (ImGui::BeginCombo("Address Space", aspace.long_name.c_str()))
    {
        for (int i = 0; i < static_cast<int>(aspaces.size()); i++)
        {
            const bool is_selected = (i == selected_aspace);
            if (ImGui::Selectable(aspaces[i].long_name.c_str(), is_selected))
            {
                selected_aspace = i;
                base_addr = 0;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::InputText("Go To (hex)", goto_buf, sizeof(goto_buf));
    ImGui::SameLine();
    if (ImGui::Button("Go"))
    {
        uint64 addr = 0;
        if (_med_parse_hex_address(goto_buf, addr))
        {
            base_addr = addr % mem_size;
            status = "Moved to address.";
        }
        else
            status = "Invalid Go To address.";
    }

    ImGui::InputText("Search (hex bytes)", search_buf, sizeof(search_buf));
    ImGui::SameLine();
    if (ImGui::Button("Search"))
    {
        std::vector<uint8> needle;
        if (!_med_parse_hex_bytes(search_buf, needle))
        {
            status = "Invalid search bytes. Use hex pairs, ex: DE AD BE EF";
        }
        else if (needle.size() > mem_size)
        {
            status = "Search pattern is larger than selected address space.";
        }
        else
        {
            std::vector<uint8> buf(needle.size());
            uint64 a = (base_addr + 1) % mem_size;
            const uint64 start = a;
            bool found = false;

            do
            {
                aspace.GetAddressSpaceBytes(aspace.name.c_str(), static_cast<uint32>(a), needle.size(), buf.data());
                if (std::memcmp(buf.data(), needle.data(), needle.size()) == 0)
                {
                    base_addr = a;
                    found = true;
                    break;
                }
                a = (a + 1) % mem_size;
            } while (a != start);

            status = found ? "Search match found." : "No match found.";
        }
    }

    ImGui::InputText("Save Path", save_path, sizeof(save_path));
    ImGui::InputInt("Save Size (bytes)", &save_size, 256, 4096);
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        if (save_size <= 0)
        {
            status = "Save size must be > 0.";
        }
        else
        {
            try
            {
                FileStream fp(save_path, FileStream::MODE_WRITE);
                std::vector<uint8> chunk(256);
                uint64 written = 0;

                while (written < static_cast<uint64>(save_size))
                {
                    const uint64 remaining = static_cast<uint64>(save_size) - written;
                    const size_t n = static_cast<size_t>(std::min<uint64>(remaining, chunk.size()));
                    const uint64 a = (base_addr + written) % mem_size;

                    aspace.GetAddressSpaceBytes(aspace.name.c_str(), static_cast<uint32>(a), n, chunk.data());
                    fp.write(chunk.data(), n);
                    written += n;
                }

                fp.close();
                status = "Memory dump saved.";
            }
            catch (std::exception& e)
            {
                status = std::string("Save failed: ") + e.what();
            }
        }
    }

    ImGui::Separator();
    if (!status.empty())
        ImGui::TextUnformatted(status.c_str());

    rows = std::clamp(rows, 4, 128);
    ImGui::SliderInt("Rows", &rows, 4, 128);

    ImGui::BeginChild("MemoryView");
    for (int r = 0; r < rows; r++)
    {
        const uint64 row_addr = (base_addr + static_cast<uint64>(r) * 16ULL) % mem_size;
        uint8 line[16];
        aspace.GetAddressSpaceBytes(aspace.name.c_str(), static_cast<uint32>(row_addr), 16, line);

        ImGui::Text("%08llX:", static_cast<unsigned long long>(row_addr));
        ImGui::SameLine();

        char hexbuf[16 * 3 + 1];
        char asciibuf[16 + 1];
        for (int i = 0; i < 16; i++)
        {
            std::snprintf(&hexbuf[i * 3], 4, "%02X ", line[i]);
            asciibuf[i] = (line[i] >= 0x20 && line[i] < 0x7F) ? static_cast<char>(line[i]) : '.';
        }
        hexbuf[16 * 3] = 0;
        asciibuf[16] = 0;

        ImGui::TextUnformatted(hexbuf);
        ImGui::SameLine();
        ImGui::TextUnformatted(asciibuf);
    }
    ImGui::EndChild();
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

__attribute__((optimize("O0"))) void med_imgui_render_start()
{
    if (med_init == 0)
        return;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
#ifdef IMGUI_HAS_DOCK
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
#endif

    bool show_demo_window = false;
    ImGui::ShowDemoWindow(&show_demo_window);
    /*
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("About");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
*/
    // draw game

    struct Callbacks
    {
        static void AspectRatio(ImGuiSizeCallbackData *data)
        {
            float aspect_ratio = *(float *)data->UserData;
            data->DesiredSize.y = (float)(int)(data->DesiredSize.x / aspect_ratio);
        }
    };

    const float screenRatio = (float)last_w / (float)last_h;
    ImGui::SetNextWindowSizeConstraints(ImVec2(256.f, 256.f), ImVec2(FLT_MAX, FLT_MAX), Callbacks::AspectRatio, (void *)&screenRatio);
    if (ImGui::Begin("Emulation", nullptr, 0))
    {
        // Scale target to fit window
        ImVec2 parent = ImGui::GetContentRegionAvail();

        // scale to respect ratio
        ImVec2 scaled = parent;
        scaled.y = (float)(int)(scaled.x / screenRatio);
        if (scaled.y > parent.y)
        {
            scaled.x = (float)(int)(parent.y * screenRatio);
            scaled.y = parent.y;
        }

        ImGui::Image((ImTextureID)(intptr_t)fb_tex_id, scaled);
    }
    ImGui::End();
    // draw debug
    if (ImGui::Begin("Registers"))
    {
        _med_imgui_debug_register_render();
        _med_imgui_dev_register_render();
    }
    ImGui::End();

    // draw profiler
    if (ImGui::Begin("Profiler"))
    {
        _med_imgui_render_profiler();
    }
    ImGui::End();

    if (ImGui::Begin("Memory"))
    {
        _med_imgui_render_memory();
    }
    ImGui::End();

    // draw Logs
    if (ImGui::Begin("Logs"))
    {
        _med_imgui_render_logs();
    }
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT);
}

extern void ShowExampleAppDockSpace(bool *p_open);
void med_imgui_render_end()
{
    if (med_init == 0)
        return;
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //

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
#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
#endif

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    {
        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;

        colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.70f, 0.72f, 0.75f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.08f, 0.10f, 0.98f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.08f, 0.10f, 0.98f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.10f, 0.12f, 0.98f);
    }

    {
        const float ui_scale = med_imgui_get_ui_scale();
        if (ui_scale > 1.0f)
        {
            io.FontGlobalScale = ui_scale;
            ImGui::GetStyle().ScaleAllSizes(ui_scale);
        }
    }
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