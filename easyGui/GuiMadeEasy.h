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

	std::unordered_map<int, std::unique_ptr<genericData>> Descendants;
	std::unordered_map<int, bool> allocatedIDs;
	std::unordered_map<int, bool> freedIDs;

	int ScreenX;
	int ScreenY;

	int generateID() {
		if (!freedIDs.empty()) {
			auto it = freedIDs.begin();
			int id = it->first;
			freedIDs.erase(it);
			allocatedIDs[id] = true;
			return id;
		}
		int newId = allocatedIDs.size();
		allocatedIDs[newId] = true;
		return newId;
	}
public:
	ScreenGui(int scrX, int scrY, SDL_Renderer* Renderer) : ScreenX(scrX), ScreenY(scrY), renderer(Renderer) {

	}

	int processClick();

	int checkIDInAllLists(int objectID) {
		if (SizeList.find(objectID) != SizeList.end()) {
			std::cout << "SizeList:" << SizeList.find(objectID)->first << '\n';
		}
		else
		{
			std::cout << "SizeList: NOT FOUND\n";
		}

		return 0;
	}

	int destroyObject(int objectID) {
		auto it = allocatedIDs.find(objectID);
		if (it != allocatedIDs.end()) {
			allocatedIDs.erase(it);
			freedIDs[objectID] = true;

			SizeList.erase(objectID);
			PositionList.erase(objectID);
			ColorList.erase(objectID);
			VisibleList.erase(objectID);
			InteractableList.erase(objectID);
			if (TextureList.find(objectID) != TextureList.end()) {
				SDL_DestroyTexture(TextureList[objectID]);
				TextureList.erase(objectID);
			}

			if (Descendants.find(objectID) != Descendants.end()) {
				Descendants[objectID].release();
				Descendants.erase(objectID);
			}

			return 0;
		}

		return -1; // object not found
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

		Descendants[newButton->objectID] = std::move(newButton);

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

		Descendants[newFrame->objectID] = std::move(newFrame);

		return 0;
	}
};