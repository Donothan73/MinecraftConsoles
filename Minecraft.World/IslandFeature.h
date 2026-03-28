#pragma once
#include "Feature.h"

class Level;
class Random;

class IslandFeature : public Feature
{
private:
	int tile;

public:
	IslandFeature (int tile);
	

	virtual bool place(Level *level, Random *random, int x, int y, int z);
};