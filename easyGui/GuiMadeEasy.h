#pragma once
#include "SDL.h"
#include <unordered_map>
#include <memory>
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
		int objectID;
	};
	
	SDL_Renderer* renderer = nullptr;
	
	std::unordered_map<int,IntVector> SizeList;
	std::unordered_map<int, IntVector> PositionList;
	std::unordered_map<int, SDL_Color> ColorList;
	std::unordered_map<int, SDL_Texture*> TextureList;
	std::unordered_map<int, bool> VisibleList;
	std::unordered_map<int, bool> InteractableList;

	std::vector<std::unique_ptr<genericData>> Descendants;
	std::vector<int> allocatedIDs;
	std::vector<int> freedIDs;

	int ScreenX;
	int ScreenY;

	int generateID() {
		if (!freedIDs.empty()) {
			int toReturn = freedIDs[0];
			freedIDs.erase(freedIDs.begin());
			allocatedIDs.insert(allocatedIDs.begin() + toReturn, toReturn);
			return toReturn;
		}
		allocatedIDs.push_back(allocatedIDs.size());
		return allocatedIDs.size()-1;
	}
public:
	ScreenGui(int scrX, int scrY, SDL_Renderer* Renderer) : ScreenX(scrX), ScreenY(scrY), renderer(Renderer) {

	}

	int processClick();

	int destroyObject(int objectID) {
		allocatedIDs.erase(allocatedIDs.begin()+objectID)
	}

	int createButton(int x, int y, int w, int h, bool visible, bool interactable, SDL_Color color) {
		// create a genericdata and assign variables to the correct places
		auto newButton = std::make_unique<genericData>();
		newButton->type = Frame;
		newButton->objectID = generateID();

		IntVector sizeToAssign = { w,h };
		IntVector positionToAssign = { x,y };

		VisibleList[newButton->objectID] = visible;

		SizeList[newButton->objectID] = sizeToAssign;

		PositionList[newButton->objectID] = positionToAssign;

		ColorList[newButton->objectID] = color;

		//create the texture for the frame
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);

		Uint32 nColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
		SDL_FillRect(surface, NULL, nColor);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		TextureList[newButton->objectID] = texture;

		SDL_FreeSurface(surface);

		Descendants.push_back(std::move(newButton));

		return 0;
	}

	int createFrame(int x, int y, int w, int h, bool visible, SDL_Color color) {
		// create a genericdata and assign variables to the correct places
		auto newFrame = std::make_unique<genericData>();
		newFrame->type = Frame;
		newFrame->objectID = generateID();
		
		IntVector sizeToAssign = { w,h };
		IntVector positionToAssign = { x,y };

		VisibleList[newFrame->objectID] = visible;

		SizeList[newFrame->objectID] = sizeToAssign;

		PositionList[newFrame->objectID] = positionToAssign;

		ColorList[newFrame->objectID] = color;

		//create the texture for the frame
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);

		Uint32 nColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
		SDL_FillRect(surface, NULL, nColor);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

		TextureList[newFrame->objectID] = texture;

		SDL_FreeSurface(surface);

		Descendants.push_back(std::move(newFrame));

		return 0;
	}
};