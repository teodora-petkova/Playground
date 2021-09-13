// main.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <functional>

#include "SDL.h"
#include "drawalgorithms.h"
#undef main

#define degreesToRadians(x) x *(3.141592f / 180.0f)
// currying in c++ (( :

const std::function<int(SDL_Renderer *, int, int)> &drawSdlPoint =
	[](SDL_Renderer *r, int x, int y) -> int
{
	return SDL_RenderDrawPoint(r, x, y);
};

const std::function<std::function<int(int, int)>(SDL_Renderer *)> &drawPoint =
	[](SDL_Renderer *r) -> std::function<int(int, int)> {
	return [r](int x, int y) -> int
	{
		return drawSdlPoint(r, x, y);
	};
};

int updateSdlCellColor(SDL_Renderer *renderer, int cellx, int celly, int cellSize, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_Rect cell = {cellx,
					 celly,
					 cellSize,
					 cellSize};
	return SDL_RenderFillRect(renderer, &cell);
}

const std::function<std::function<int(int, int)>(SDL_Renderer *, int, SDL_Color)> &updateCellColor =
	[](SDL_Renderer *r, int gridCellSize, SDL_Color color) -> std::function<int(int, int)> {
	return [r, gridCellSize, color](int x, int y) -> int
	{
		return updateSdlCellColor(r, x, y, gridCellSize, color);
	};
};

void drawLinesInAllDirections(SDL_Renderer *renderer)
{
	// the ultimate test (( :
	auto drawPnt = drawPoint(renderer);

	int centerx = 300;
	int centery = 250;
	int radius = 200;

	for (int alpha = 0; alpha < 360; alpha += 5)
	{
		int x1 = centerx;
		int y1 = centery;

		float rad = degreesToRadians(alpha);
		int x2 = centerx + cos(rad) * radius;
		int y2 = centery + sin(rad) * radius;

		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);

		//SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

		//DrawAlgorithms::drawNaiveLine(drawPnt, x1, y1, x2, y2);
		//DrawAlgorithms::drawBresenhamLine1Octant(drawPnt, x1, y1, x2, y2);
		//DrawAlgorithms::drawBresenhamLineNegativeSlope(drawPnt, x1, y1, x2, y2);

		DrawAlgorithms::drawBresenhamLine1(drawPnt, x1, y1, x2, y2);
	}
}

void drawGrid(SDL_Renderer *renderer, int windowWidth, int windowHeight, int gridCellSize,
			  SDL_Color gridBackground, SDL_Color gridLineColor)
{
	int gridWidth = windowWidth / gridCellSize;
	int gridHeight = windowHeight / gridCellSize;

	// draw grid background.
	SDL_SetRenderDrawColor(renderer, gridBackground.r, gridBackground.g,
						   gridBackground.b, gridBackground.a);
	SDL_RenderClear(renderer);

	// draw grid lines.
	SDL_SetRenderDrawColor(renderer, gridLineColor.r, gridLineColor.g,
						   gridLineColor.b, gridLineColor.a);

	for (int x = 0; x < 1 + gridWidth * gridCellSize; x += gridCellSize)
	{
		SDL_RenderDrawLine(renderer, x, 0, x, windowHeight);
	}

	for (int y = 0; y < 1 + gridHeight * gridCellSize; y += gridCellSize)
	{
		SDL_RenderDrawLine(renderer, 0, y, windowWidth, y);
	}
}

void drawGridLine(SDL_Renderer *renderer, int cellSize, int x1, int y1, int x2, int y2, SDL_Color color)
{
	int cellx1 = x1;
	int celly1 = y1;
	int cellx2 = x2;
	int celly2 = y2;

	auto updateCell = updateCellColor(renderer, cellSize, color);

	DrawAlgorithms::drawBresenhamLine2(updateCell, cellx1, celly1, cellx2, celly2, cellSize);
}

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) == 0)
	{
		int windowWidth = 640;
		int windowHeight = 480;
		int cellSize = 15;

		SDL_Color gridBackgroundColor = {22, 22, 22, 255}; // barely black
		SDL_Color gridLineColor = {66, 66, 66, 255};	   // dark grey
		SDL_Color white = {255, 255, 255, 255};			   // white

		SDL_Window *window = NULL;
		SDL_Renderer *renderer = NULL;

		if (SDL_CreateWindowAndRenderer(windowWidth, windowHeight, 0, &window, &renderer) == 0)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);

			SDL_Rect cursorCell = SDL_Rect();

			cursorCell.x = windowWidth / (2 * cellSize) * cellSize;
			cursorCell.y = windowHeight / (2 * cellSize) * cellSize;
			SDL_Rect cursor = {cursorCell.x, cursorCell.y,
							   cellSize, cellSize};

			SDL_Rect start = cursorCell;
			SDL_Rect end = cursorCell;

			SDL_bool quit = SDL_FALSE;
			SDL_bool mouseActive = SDL_FALSE;
			SDL_bool mouseHover = SDL_FALSE;

			while (!quit)
			{
				drawGrid(renderer, windowWidth, windowHeight, cellSize,
						 gridBackgroundColor, gridLineColor);

				SDL_Event event;

				while (SDL_PollEvent(&event))
				{
					switch (event.type)
					{
					case SDL_KEYDOWN:
						switch (event.key.keysym.sym)
						{
						case SDLK_w:
						case SDLK_UP:
							cursorCell.y -= cellSize;
							break;
						case SDLK_s:
						case SDLK_DOWN:
							cursorCell.y += cellSize;
							break;
						case SDLK_a:
						case SDLK_LEFT:
							cursorCell.x -= cellSize;
							break;
						case SDLK_d:
						case SDLK_RIGHT:
							cursorCell.x += cellSize;
							break;
						}
						break;
					case SDL_MOUSEBUTTONDOWN:
						cursorCell.x = (event.motion.x / cellSize) * cellSize;
						cursorCell.y = (event.motion.y / cellSize) * cellSize;
						start = cursorCell;
						break;
					case SDL_MOUSEBUTTONUP:
						cursorCell.x = (event.motion.x / cellSize) * cellSize;
						cursorCell.y = (event.motion.y / cellSize) * cellSize;
						start = cursorCell;
						break;
					case SDL_MOUSEMOTION:
						cursor.x = (event.motion.x / cellSize) * cellSize;
						cursor.y = (event.motion.y / cellSize) * cellSize;

						if (!mouseActive)
							mouseActive = SDL_TRUE;
						break;
					case SDL_WINDOWEVENT:
						if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouseHover)
							mouseHover = SDL_TRUE;
						else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouseHover)
							mouseHover = SDL_FALSE;
						break;
					case SDL_QUIT:
						quit = SDL_TRUE;
						break;
					}
				}

				drawGridLine(renderer, cellSize,
							 start.x, start.y,
							 cursor.x, cursor.y,
							 gridLineColor);

				if (mouseActive && mouseHover)
				{
					updateSdlCellColor(renderer, cursor.x, cursor.y,
									   cellSize, gridLineColor);
				}

				SDL_RenderPresent(renderer);
			}
		}

		if (renderer)
		{
			SDL_DestroyRenderer(renderer);
		}
		if (window)
		{
			SDL_DestroyWindow(window);
		}
	}
	SDL_Quit();
	return 0;
}