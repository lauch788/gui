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
  SDL_DestroyRenderer(ctx->rend);
  SDL_DestroyWindow(ctx->win.win);
  SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_Quit();
}

void
GUI_Redraw(GUIContext *ctx)
{
  SDL_SetRenderDrawBlendMode(ctx->rend, SDL_BLENDMODE_NONE);
  SDL_SetRenderDrawColor(ctx->rend,
    ctx->bg_color.r, ctx->bg_color.g, ctx->bg_color.b, 0xFF);
  SDL_RenderClear(ctx->rend);
}

static void
window_event(GUIWindow *win, const SDL_Event *ev)
{
  if (ev->type == SDL_WINDOWEVENT
   && ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
  {
    win->w = ev->window.data1;
    win->h = ev->window.data2;
  }
}

static void
sqrt_draw(GUIContext *ctx)
{
  SDL_SetRenderDrawColor(ctx->rend, 0xFF, 0x00, 0x00, 0xFF);
  for (int screenX = 0; screenX < ctx->win.w; screenX++)
  {
    double realX = (double)screenX/ctx->win.w;
    double realY = sqrt(realX);
    int screenY = ctx->win.h - (realY * ctx->win.h);
    SDL_RenderDrawPoint(ctx->rend, screenX, screenY);
  }
}

static void
cubic_draw(GUIContext *ctx)
{
  SDL_SetRenderDrawColor(ctx->rend, 0x00, 0xFF, 0x00, 0xFF);
  for (int screenX = 0; screenX < ctx->win.w; screenX++)
  {
    double realX = (double)screenX/ctx->win.w;
    double realY = easeOutCubic(realX);
    int screenY = ctx->win.h - (realY * ctx->win.h);
    SDL_RenderDrawPoint(ctx->rend, screenX, screenY);
  }
}

static void
square_draw(GUIContext *ctx)
{
  SDL_SetRenderDrawColor(ctx->rend, 0x00, 0x00, 0xFF, 0xFF);
  for (int screenX = 0; screenX < ctx->win.w; screenX++)
  {
    double realX = (double)screenX/ctx->win.w;
    double realY = 2.0*realX*realX;
    int screenY = ctx->win.h - (realY * ctx->win.h);
    SDL_RenderDrawPoint(ctx->rend, screenX, screenY);
  }
}

static void
noisy_draw(GUIContext *ctx)
{
 SDL_SetRenderDrawBlendMode(ctx->rend, SDL_BLENDMODE_NONE);
 SDL_SetRenderDrawColor(ctx->rend, 0xFF, 0xC3, 0x00, 0xFF);
  for (int screenY = 0; screenY < ctx->win.h; screenY++) {
    for (int screenX = 0; screenX < ctx->win.w; screenX++) {
      double realX = (double)((10 * screenX) + 0) / ctx->win.w;
      double realY = (double)((10 * screenY) + 0) / ctx->win.h;
      if (fabs(fabs(sin(realX*realX - realY*realY)) - sin(realX + realY) + cos(realX*realY)) <= 0.15)
        SDL_RenderDrawPoint(ctx->rend, screenX, ctx->win.h - screenY);
    }
  }

 SDL_SetRenderDrawColor(ctx->rend, 0xAF, 0x82, 0xC9, 0xFF);
  for (int screenY = 0; screenY < ctx->win.h; screenY++) {
    for (int screenX = 0; screenX < ctx->win.w; screenX++) {
      double realX = (double)((10 * screenX) + 0) / ctx->win.w;
      double realY = (double)((10 * screenY) + 0) / ctx->win.h;
      {
        double s = sin(realX*realX + realY*realY); s *= s;
        double t = tan(realX*realX + realY*realY); t *= t;
        if (fabs(s + t - 1.0) <= 0.0)
          SDL_RenderDrawPoint(ctx->rend, screenX, ctx->win.h - screenY);
      }
    }
  }

 SDL_SetRenderDrawBlendMode(ctx->rend, SDL_BLENDMODE_MOD);
  SDL_SetRenderDrawColor(ctx->rend, 0xDB, 0xEF, 0x76, 0xFF);
  for (int screenY = 0; screenY < ctx->win.h; screenY++) {
    for (int screenX = 0; screenX < ctx->win.w; screenX++) {
      double realX = (double)((10 * screenX) + 0) / ctx->win.w;
      double realY = (double)((10 * screenY) + 0) / ctx->win.h;
      if (fabs(sin(sin(realX)+cos(realY)) - cos(sin(realX*realY) + cos(realX))) <= 0.4)
        SDL_RenderDrawPoint(ctx->rend, screenX, ctx->win.h - screenY);
    }
  }
}

static inline void
getFrameDelta(GUIContext *ctx)
{
  ctx->tLast = ctx->tNow;
  ctx->tNow  = SDL_GetPerformanceCounter();
  ctx->frameDelta = (double)((ctx->tNow - ctx->tLast) / (double)SDL_GetPerformanceFrequency());
}

void
GUI_Main(GUIContext *ctx)
{
  bool running = true;
  SDL_Event ev;

  ctx->tLast = SDL_GetPerformanceCounter();

  while (running)
  {
    getFrameDelta(ctx);

    while (SDL_PollEvent(&ev))
    {
      if (ev.type == SDL_QUIT)
      {
        running = false;
        break;
      }
      window_event(&ctx->win, &ev);
    }

    GUI_Redraw(ctx);
    noisy_draw(ctx);
    sqrt_draw(ctx);
    cubic_draw(ctx);
    square_draw(ctx);
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
