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

bool clamp2D(IntVector value, IntVector min, IntVector max) {
	if (value.X < min.X) { return false; }
	if (value.X > max.X) { return false; }
	if (value.Y < min.Y) { return false; }
	if (value.Y > max.Y) { return false; }
	return true;
}

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

	std::vector<std::unordered_map<int, std::shared_ptr<genericData>>> interactableSortedInCells2D;

	int ScreenX;
	int ScreenY;
	int GridSize; 
	int CellX;
	int CellY;

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

	int addToGrid(std::shared_ptr<genericData> objectToAdd) {
		if (objectToAdd == nullptr) { return -1; } //object doesnt exist
		
		const int objID = objectToAdd->objectID;

		IntVector minCell = { PositionList[objID].X / CellX
									,PositionList[objID].Y / CellY };
		IntVector maxCell = { (PositionList[objID].X + SizeList[objID].X) / CellX
									,(PositionList[objID].Y + SizeList[objID].Y) / CellY };

		if (minCell.X < 0) {
			//std::cout << "less than 0 corrected X\n";
			minCell.X = 0;
		}
		if (minCell.Y < 0) {
			//std::cout << "less than 0 corrected Y\n";
			minCell.Y = 0;
		}
		if (maxCell.X >= GridSize) {
			//std::cout << "more than gridsize corrected X\n";
			maxCell.X = GridSize-1;
		}
		if (maxCell.Y >= GridSize) {
			//std::cout << "more than gridsize corrected Y\n";
			maxCell.Y = GridSize-1;
		}

		for (int y = minCell.Y; y <= maxCell.Y; ++y) {
			for (int x = minCell.X; x <= maxCell.X; ++x) {
				interactableSortedInCells2D[y * GridSize + x].insert({objID,objectToAdd});
			}
		}
		return 0;
	}

	int removeFromGrid(std::shared_ptr<genericData> objectToAdd) {
		if (objectToAdd == nullptr) { return -1; } //object doesnt exist

		const int objID = objectToAdd->objectID;

		IntVector minCell = { PositionList[objID].X / CellX
									,PositionList[objID].Y / CellY };
		IntVector maxCell = { (PositionList[objID].X + SizeList[objID].X) / CellX
									,(PositionList[objID].Y + SizeList[objID].Y) / CellY };

		if (minCell.X < 0) {
			//std::cout << "less than 0 corrected X\n";
			minCell.X = 0;
		}
		if (minCell.Y < 0) {
			//std::cout << "less than 0 corrected Y\n";
			minCell.Y = 0;
		}
		if (maxCell.X >= GridSize) {
			//std::cout << "more than gridsize corrected X\n";
			maxCell.X = GridSize;
		}
		if (maxCell.Y >= GridSize) {
			//std::cout << "more than gridsize corrected Y\n";
			maxCell.Y = GridSize;
		}

		for (int y = minCell.Y; y <= maxCell.Y; ++y) {
			for (int x = minCell.X; x <= maxCell.X; ++x) {
				//std::cout << y << ":" << x << "\n";
				interactableSortedInCells2D[y * GridSize + x].erase(objID);
			}
		}
		return 0;
	}
public:
	
	ScreenGui(int scrX, int scrY,int gridSize, SDL_Renderer* Renderer) : ScreenX(scrX), ScreenY(scrY), renderer(Renderer) {
		GridSize = gridSize;
		CellX = ScreenX / GridSize;
		CellY = ScreenY / GridSize;

		interactableSortedInCells2D.resize(GridSize*(GridSize+1)+GridSize);
	}

	/*
	0 = success
	*/
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

	/*
	0 = success
	*/
	int moveZindex(std::shared_ptr<genericData> objectToMove, int ZindexToMoveTo) {
		if (isObjectInZindex(objectToMove->Zindex, objectToMove->objectID)) {
			VisibleDescendantsSortedByZindex2D[objectToMove->Zindex].erase(objectToMove->objectID);
			//std::cout << "Removed object ID: " << objectToMove->objectID << " from ZINDEX: " << objectToMove->Zindex << '\n';
		}
		objectToMove->Zindex = ZindexToMoveTo;
		makeZindexIndexExist(objectToMove->Zindex);
		VisibleDescendantsSortedByZindex2D[objectToMove->Zindex].insert({ objectToMove->objectID, objectToMove });
		//std::cout << "added ID:" << objectToMove->objectID << '\n';
		return 0;
	}

	/*
	0 = pressed button
	1 = did not press a button
	*/
	int processClick(int x, int y, bool echo) {
		if (x > ScreenX || y > ScreenY || x < 0 || y < 0) { return 2; }
		const IntVector cellClicked = { x / CellX
									   ,y / CellY };

		std::shared_ptr<genericData> clickedButton;
		
		for (auto& currentButton : interactableSortedInCells2D[cellClicked.Y * GridSize + cellClicked.X]) {
			if (InteractableList[currentButton.first] && clamp2D({x,y},
														 PositionList[currentButton.first],
													   { PositionList[currentButton.first].X + SizeList[currentButton.first].X,
													     PositionList[currentButton.first].Y + SizeList[currentButton.first].Y })) {

				if (clickedButton == nullptr || 
					clickedButton->Zindex < currentButton.second->Zindex) {

					clickedButton = currentButton.second;
					continue; 
				}
			}
		}

		if (clickedButton == nullptr) {
			if (echo) {
				std::cout << "Did not press a button\n";
			}
			
			return 1;
		}
		if (echo) {
			std::cout << "Pressed button ID:" << clickedButton->objectID << '\n';
		}
		
		return 0;
	}
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//[[[[[[[[[[[[[[[[[[[[[DEBUG FUNCTIONS !!!!!!!! YIPEEEE ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ] ]
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	/*
	returns the size in bytes of the texture
	*/
	size_t getTextureSize(SDL_Texture* texture) {
	
		int w, h, access;
		Uint32 format;
		SDL_QueryTexture(texture, &format, &access, &w, &h);
		int bpp = SDL_BYTESPERPIXEL(format);
	
		return w * h * bpp;
	}

	/*
	returns the size in bytes of texture list
	*/
	size_t getTotalTextureSize() {
	
		size_t totalSize = 0;
		for (const auto& entry : TextureList) {
			totalSize += getTextureSize(entry.second);
		}
	
		return totalSize;
	}

	/*
	returns size of descendants
	*/
	int objectCount(bool echo) {
		
		if (echo) {
			std::cout << Descendants.size() << '\n';
		}
		
		return static_cast<int>(Descendants.size());
	}

	/*
	0 = sucess
	*/
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

	/*
	1 = sucess
	0 = object not found
	*/
	int checkIDInAllLists(int objectID, bool echo) {
		if (Descendants.find(objectID) != Descendants.end()) {
			if (echo) {
				std::cout << "Descendants:" << Descendants.find(objectID)->first << '\n';
			}
			return 1;
		}
		else
		{
			if (echo) {
				std::cout << "Descendants: NOT FOUND\n";
			}
			return 0;
		}
	}
/*
[[[[[[[[[[[[[[[[[[[[[[[[ FUNCTIONS FOR FUNCTION USE!!!!]]]]]]]]]]]]]]]]]]]]]]]
*/
	/*
	0 = sucess
	1 = object not found
	*/
	int destroyObject(int objectID) {
		auto it = allocatedIDs.find(objectID);
		if (it != allocatedIDs.end()) {
			allocatedIDs.erase(it);
			freedIDs.insert(objectID);

			// remove from grid if button
			auto ite = InteractableList.find(objectID);
			if (ite != InteractableList.end()) {
				removeFromGrid(Descendants[objectID]);
			}

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

	/*
	0 = sucess
	*/
	int createButton(int x, int y, int w, int h, bool visible, bool interactable, SDL_Color color) {
		// create a genericdata and assign variables to the correct places
		auto newButton = std::make_shared<genericData>();
		newButton->type = Frame;
		newButton->onScreenObject = true;
		newButton->objectID = generateID();
		newButton->Zindex = 0;
		moveZindex(newButton, 0);

		IntVector sizeToAssign = { w,h };
		IntVector positionToAssign = { x,y };

		VisibleList.insert({ newButton->objectID , visible });
		SizeList.insert({ newButton->objectID , sizeToAssign });
		PositionList.insert({ newButton->objectID , positionToAssign });
		ColorList.insert({ newButton->objectID , color });
		InteractableList.insert({ newButton->objectID , interactable });

		// Create the texture for the frame
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
		Uint32 nColor = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
		SDL_FillRect(surface, NULL, nColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		TextureList.insert({ newButton->objectID , texture });
		SDL_FreeSurface(surface);

		// Debug print
		//std::cout << (positionToAssign.X + sizeToAssign.X > ScreenX) << ":" << (positionToAssign.Y + sizeToAssign.Y > ScreenY) << "\n";


		Descendants.insert({ newButton->objectID , newButton });
		addToGrid(newButton);


		return 0;
	}

	/*
	0 = sucess
	*/
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
		//std::cout << positionToAssign.X << ":" << positionToAssign.Y << "\n";

		return 0;
	}

};
