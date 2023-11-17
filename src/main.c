/* 
 * This file is part of the gui distribution (https://github.com/lauch788/gui).
 * Copyright (c) 2023 Adrien Ricciardi.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "ease.h"

typedef struct GUIWindow_s {
  SDL_Window *win;
  int w, h;
} GUIWindow;

typedef struct GUIContext_s {
  GUIWindow win;
  SDL_Renderer *rend;

  SDL_Color bg_color;

  uint64_t tLast, tNow;
  double frameDelta;

} GUIContext;

void
GUI_CreateContext(GUIContext *ctx)
{
  SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  ctx->win.win = SDL_CreateWindow("GUIWindow",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    ctx->win.w, ctx->win.h,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  ctx->rend = SDL_CreateRenderer(ctx->win.win, -1, SDL_RENDERER_ACCELERATED);
  // ctx->running = 1;
}

void
GUI_FreeContext(GUIContext *ctx)
{
  SDL_DestroyWindow(ctx->win.win);
  SDL_DestroyRenderer(ctx->rend);
  SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_Quit();
}

void
GUI_Redraw(GUIContext *ctx)
{
  SDL_SetRenderDrawColor(ctx->rend,
    ctx->bg_color.r, ctx->bg_color.g, ctx->bg_color.b, 0xFF);
  SDL_RenderClear(ctx->rend);
}

static inline double
getFrameDelta(GUIContext *ctx)
{
  ctx->tLast = ctx->tNow;
  ctx->tNow  = SDL_GetPerformanceCounter();
  return (double)((ctx->tNow - ctx->tLast) / (double)SDL_GetPerformanceFrequency());
}

void
GUI_Main(GUIContext *ctx)
{
  _Bool running = true;
  SDL_Event ev;

  ctx->tLast = SDL_GetPerformanceCounter();

  while (running)
  {
    ctx->frameDelta = getFrameDelta(ctx);

    while (SDL_PollEvent(&ev))
    {
      if (ev.type == SDL_QUIT)
      {
        running = false;
        break;
      }

    }

    GUI_Redraw(ctx);
    SDL_RenderPresent(ctx->rend);
    SDL_Delay(10);
  }

}

int
main(int argc, char *argv[])
{
  GUIContext ctx = { .win.w = 512, .win.h = 512 };

  GUI_CreateContext(&ctx);
  GUI_Main(&ctx);
  GUI_FreeContext(&ctx);

  return 0;
}
