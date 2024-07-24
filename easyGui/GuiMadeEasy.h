#pragma once
#include "SDL.h"
#include <vector>
#include <iostream>
struct IntVector {
	int X;
	int Y;
};

class ScreenGui {
private:
	const enum uiTypes {
		Frame,
		Button
	};

	struct genericData {
		uiTypes type;
		std::vector<int> dataPositions;

	};
	
	SDL_Renderer* renderer = nullptr;

	std::vector<IntVector> SizeList;
	std::vector<IntVector> PositionList;
	std::vector<SDL_Color> ColorList;
	std::vector<SDL_Texture*> TextureList;
	std::vector<bool> VisibleList;
	std::vector<bool> InteractableList;

	std::vector<genericData*> Descendants;

	int ScreenX;
	int ScreenY;
public:
	ScreenGui(int scrX, int scrY, SDL_Renderer* Renderer) : ScreenX(scrX), ScreenY(scrY), renderer(Renderer) {

	}

	int processClick()

	int createButton(int x, int y, int w, int h, bool visible, bool interactable, SDL_Color color) {
		// create a genericdata and assign variables to the correct places
		genericData* newButton = new genericData;
		newButton->type = Button;

		IntVector sizeToAssign = { w,h };
		IntVector positionToAssign = { x,y };

		newButton->dataPositions.resize(5);

		newButton->dataPositions[0] = VisibleList.size();
		VisibleList.push_back(visible);

		newButton->dataPositions[1] = InteractableList.size();
		InteractableList.push_back(interactable);

		newButton->dataPositions[2] = SizeList.size();
		SizeList.push_back(sizeToAssign);

		newButton->dataPositions[3] = PositionList.size();
		PositionList.push_back(positionToAssign);

		newButton->dataPositions[4] = ColorList.size();
		ColorList.push_back(color);

		//create the texture for the button
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);

		Uint32 nColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
		SDL_FillRect(surface, NULL, nColor);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		newButton->dataPositions[5] = TextureList.size();
		TextureList.push_back(texture);

		SDL_FreeSurface(surface);

		return 0;
	}

	int createFrame(int x, int y, int w, int h, bool visible, SDL_Color color) {
		// create a genericdata and assign variables to the correct places
		genericData* newFrame = new genericData;
		newFrame->type = Frame;
		
		IntVector sizeToAssign = { w,h };
		IntVector positionToAssign = { x,y };

		newFrame->dataPositions.resize(5);

		newFrame->dataPositions[0] = VisibleList.size();
		VisibleList.push_back(visible);

		newFrame->dataPositions[1] = SizeList.size();
		SizeList.push_back(sizeToAssign);

		newFrame->dataPositions[2] = PositionList.size();
		PositionList.push_back(positionToAssign);

		newFrame->dataPositions[3] = ColorList.size();
		ColorList.push_back(color);

		//create the texture for the frame
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);

		Uint32 nColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
		SDL_FillRect(surface, NULL, nColor);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		newFrame->dataPositions[4] = TextureList.size();
		TextureList.push_back(texture);

		SDL_FreeSurface(surface);

		return 0;
	}
};