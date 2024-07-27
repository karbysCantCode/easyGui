#pragma once
#include "SDL.h"
#include <unordered_map>
#include <unordered_set>
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
		bool onScreenObject;
		uiTypes type;
		int objectID;
		int Zindex;
	};
	
	SDL_Renderer* renderer = nullptr;
	
	std::unordered_map<int,IntVector> SizeList;
	std::unordered_map<int, IntVector> PositionList;
	std::unordered_map<int, SDL_Color> ColorList;
	std::unordered_map<int, SDL_Texture*> TextureList;
	std::unordered_map<int, bool> VisibleList;
	std::unordered_map<int, bool> InteractableList;

	std::unordered_map<int, std::shared_ptr<genericData>> Descendants;
	std::vector<std::unordered_map<int, std::shared_ptr<genericData>>> VisibleDescendantsSortedByZindex2D;
	std::unordered_set<int> allocatedIDs;
	std::unordered_set<int> freedIDs;

	int ScreenX;
	int ScreenY;

	int generateID() {
		if (!freedIDs.empty()) {
			auto it = freedIDs.begin();
			int id = *it;
			freedIDs.erase(it);
			allocatedIDs.insert(id);
			return id;
		}
		int newId = static_cast<int>(allocatedIDs.size());
		allocatedIDs.insert(newId);
		return newId;

	}
	/*
	* 1 = created
	* 0 = exists w/o created
	*/
	int makeZindexIndexExist(int index) {
		if (VisibleDescendantsSortedByZindex2D.size() <= index) {
			VisibleDescendantsSortedByZindex2D.resize(index+1);
			return 1;
		}
		return 0;
	}

	bool isObjectInZindex(int index, int objectToCheckID) {
		if (index < VisibleDescendantsSortedByZindex2D.size()) {
			auto& zindexMap = VisibleDescendantsSortedByZindex2D[index];
			return zindexMap.find(objectToCheckID) != zindexMap.end();
		}
		return false;
	}
public:
	ScreenGui(int scrX, int scrY, SDL_Renderer* Renderer) : ScreenX(scrX), ScreenY(scrY), renderer(Renderer) {

	}

	int renderDescendants() {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);  // Enable blending if you use transparency

		for (const auto& currentZindex : VisibleDescendantsSortedByZindex2D) {
			for (const auto& currentObject : currentZindex) {
				int objId = currentObject.first;

				if (VisibleList[objId]) {
					SDL_Rect rect;
					rect.x = PositionList[objId].X;
					rect.y = PositionList[objId].Y;
					rect.w = SizeList[objId].X;
					rect.h = SizeList[objId].Y;

					SDL_RenderCopy(renderer, TextureList[objId], NULL, &rect);
				}
			}
		}

		return 0;
	}

	int moveZindex(std::shared_ptr<genericData> objectToMove, int ZindexToMoveTo) {
		if (isObjectInZindex(objectToMove->Zindex, objectToMove->objectID)) {
			VisibleDescendantsSortedByZindex2D[objectToMove->Zindex].erase(objectToMove->objectID);
			std::cout << "Removed object ID: " << objectToMove->objectID << " from ZINDEX: " << objectToMove->Zindex << '\n';
		}
		objectToMove->Zindex = ZindexToMoveTo;
		makeZindexIndexExist(objectToMove->Zindex);
		VisibleDescendantsSortedByZindex2D[objectToMove->Zindex].insert({ objectToMove->objectID, objectToMove });
		std::cout << "Added object ID: " << objectToMove->objectID << " to ZINDEX: " << objectToMove->Zindex << '\n';
		return 0;
	}

	int processClick();

	int objectCount() {
		std::cout << Descendants.size();
		return 0;
	}

	int zindexOut() {
		int count = 0;
		for (const auto& currentZindex : VisibleDescendantsSortedByZindex2D) {
			std::cout << "ZINDEX:" << count << '\n';
			for (const auto& currentObject : currentZindex) {
				int objId = currentObject.first;

				std::cout << "ID:" << objId << '\n';
			}
			count++;
		}
		
		return 0;
	}

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
			freedIDs.insert(objectID);

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
				Descendants[objectID].reset();
				Descendants.erase(objectID);
			}

			return 0;
		}

		return -1; // object not found
	}

	int createButton(int x, int y, int w, int h, bool visible, bool interactable, SDL_Color color) {
		// create a genericdata and assign variables to the correct places
		auto newButton = std::make_shared<genericData>();
		newButton->type = Button;
		newButton->onScreenObject = true;
		newButton->Zindex = 0;
		moveZindex(newButton, 0);
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

		Descendants[newButton->objectID] = newButton;

		return 0;
	}

	int createFrame(int x, int y, int w, int h, bool visible, SDL_Color color) {
		// create a genericdata and assign variables to the correct places
		auto newFrame = std::make_shared<genericData>();
		newFrame->type = Frame;
		newFrame->onScreenObject = true;
		newFrame->objectID = generateID();
		newFrame->Zindex = 0;
		moveZindex(newFrame, 0);

		IntVector sizeToAssign = { w,h };
		IntVector positionToAssign = { x,y };

		VisibleList.insert({ newFrame->objectID , visible});
		SizeList.insert({ newFrame->objectID , sizeToAssign });
		PositionList.insert({ newFrame->objectID , positionToAssign });
		ColorList.insert({ newFrame->objectID , color });

		// Create the texture for the frame
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
		Uint32 nColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
		SDL_FillRect(surface, NULL, nColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		TextureList.insert({ newFrame->objectID , texture });
		SDL_FreeSurface(surface);

		Descendants.insert({ newFrame->objectID , newFrame });

		// Debug print
		std::cout << positionToAssign.X << ":" << positionToAssign.Y << "\n";

		return 0;
	}

};