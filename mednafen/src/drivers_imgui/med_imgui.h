#pragma once

void med_imgui_init(SDL_Window* window, SDL_GLContext glcontext);
void med_imgui_render_frame(const MDFN_Surface *src_surface, const MDFN_Rect *src_rect, const MDFN_Rect *dest_rect, const MDFN_Rect *original_src_rect, int InterlaceField, int UsingIP, int rotated);
void med_imgui_kill();
void med_imgui_render_start();
void med_imgui_render_end();
void med_imgui_process_event(SDL_Event *event);