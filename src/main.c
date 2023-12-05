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
  double epsilon;
  double off_x;
  double off_y;
  double mag;
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
    return;
  }

  if (ev->type == SDL_KEYDOWN)
  {
    if (ev->key.keysym.scancode == SDL_SCANCODE_DOWN)
    {
      win->off_y -= 0.1*win->mag;
      return;
    }
    if (ev->key.keysym.scancode == SDL_SCANCODE_UP)
    {
      win->off_y += 0.1*win->mag;
      return;
    }
    if (ev->key.keysym.scancode == SDL_SCANCODE_LEFT)
    {
      win->off_x -= 0.1*win->mag;
      return;
    }
    if (ev->key.keysym.scancode == SDL_SCANCODE_RIGHT)
    {
      win->off_x += 0.1*win->mag;
      return;
    }

    if (ev->key.keysym.scancode == SDL_SCANCODE_D)
    {
      win->epsilon += 0.01;
      return;
    }
    if (ev->key.keysym.scancode == SDL_SCANCODE_A)
    {
      win->epsilon -= 0.01;
      return;
    }

    if (ev->key.keysym.scancode == SDL_SCANCODE_E)
    {
      win->mag += 0.1;
      return;
    }
    if (ev->key.keysym.scancode == SDL_SCANCODE_Q)
    {
      win->mag -= 0.1;
      return;
    }
    
    return;
  }

}

static void
curve_draw(GUIContext *ctx)
{
  SDL_SetRenderDrawColor(ctx->rend, 0xFF, 0xC3, 0x00, 0xFF);
  for (int sy = 0; sy < ctx->win.h; sy++)
  for (int sx = 0; sx < ctx->win.w; sx++)
  {
    double x = ctx->win.mag*((double)sx / ctx->win.w) + ctx->win.off_x;
    double y = ctx->win.mag*((double)sy / ctx->win.h) + ctx->win.off_y;
    const double ε = ctx->win.epsilon;

    if (fabs(sin(x*x - y*y) - sin(x*y) + cos(x*y)) < ε)
      SDL_RenderDrawPoint(ctx->rend, sx, ctx->win.h - sy);
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
    curve_draw(ctx);
    SDL_RenderPresent(ctx->rend);
    SDL_Delay(30);
  }

}

int
main(int argc, char *argv[])
{
  GUIContext ctx = {
    .win.w = 512,
    .win.h = 512,
    .win.mag = 10.0,
    .win.off_x = 0.0,
    .win.off_y = 0.0,
    // .win.offddd
    .win.epsilon = 0.14
  };

  GUI_CreateContext(&ctx);
  GUI_Main(&ctx);
  GUI_FreeContext(&ctx);

  return 0;
}
