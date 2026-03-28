#include "stdafx.h"
#include "IslandFeature.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.level.biome.h"
#include "TreeFeature.h"

IslandFeature::IslandFeature(int tile)
{
	this->tile = tile;
}

bool IslandFeature::place(Level *level, Random *random, int x, int y, int z)
{
	x -= 8;
	z -= 8;
	while (y > 5 && level->isEmptyTile(x, y, z))
		y--;
	if (y <= 4)
	{
		return false;
	}

	y -= 4;

	bool grid[16*16*8] = {0};

	LevelGenerationOptions *levelGenOptions = nullptr;
	if( app.getLevelGenerationOptions() != nullptr )
	{
		levelGenOptions = app.getLevelGenerationOptions();

		int minX = x;
		int minY = y;
		int minZ = z;

		int maxX = x + 24;
		int maxY = y + 8;
		int maxZ = z + 24;

		bool intersects = levelGenOptions->checkIntersects(minX, minY, minZ, maxX, maxY, maxZ);
		if(intersects)
		{
			//app.DebugPrintf("Skipping reeds feature generation as it overlaps a game rule structure\n");
			return false;
		}
	}

	int spots = random->nextInt(4) + 4;
	for (int i = 0; i < spots; i++)
	{
		double xr = random->nextDouble() * 8 + 8;
		double yr = random->nextDouble() * 4 + 2;
		double zr = random->nextDouble() * 8 + 8;

		double xp = random->nextDouble() * (16 - xr - 2) + 1 + xr / 2;
		double yp = random->nextDouble() * (8 - yr - 4) + 2 + yr / 2;
		double zp = random->nextDouble() * (16 - zr - 2) + 1 + zr / 2;

		for (int xx = 1; xx < 15; xx++)
		{
			for (int zz = 1; zz < 15; zz++)
			{
				for (int yy = 1; yy < 7; yy++)
				{
					double xd = ((xx - xp) / (xr / 2));
					double yd = ((yy - yp) / (yr / 2));
					double zd = ((zz - zp) / (zr / 2));
					double d = xd * xd + yd * yd + zd * zd;
					if (d < 1) grid[((xx) * 16 + (zz)) * 8 + (yy)] = true;
				}
			}
		}
	}

	for (int xx = 0; xx < 16; xx++)
	{
		for (int zz = 0; zz < 16; zz++)
		{
			for (int yy = 0; yy < 8; yy++)
			{
				bool check = !grid[((xx) * 16 + (zz)) * 8 + (yy)] && ((xx < 15 && grid[((xx + 1) * 16 + (zz)) * 8 + (yy)])//
					|| (xx > 0 && grid[((xx - 1) * 16 + (zz)) * 8 + (yy)])
					|| (zz < 15 && grid[((xx) * 16 + (zz + 1)) * 8 + (yy)])
					|| (zz > 0 && grid[((xx) * 16 + (zz - 1)) * 8 + (yy)])
					|| (yy < 7 && grid[((xx) * 16 + (zz)) * 8 + (yy + 1)])
					|| (yy > 0 && grid[((xx) * 16 + (zz)) * 8 + (yy - 1)]));

				if (check)
				{
					Material *m = level->getMaterial(x + xx, y + yy, z + zz);
					if (yy >= 4 && m->isLiquid()) return false;
					if (yy < 4 && (!m->isSolid() && level->getTile(x + xx, y + yy, z + zz) != tile)) return false;

				}
			}
		}
	}

	int height = random->nextInt(32);

	for (int xx = 0; xx < 16; xx++)
	{
		for (int zz = 0; zz < 16; zz++)
		{
			for (int yy = 0; yy < 8; yy++)
			{
				if (grid[((xx) * 16 + (zz)) * 8 + (yy)])
				{
					level->setTileAndData(x + xx, y + yy+100+height, z + zz, tile, 0, Tile::UPDATE_CLIENTS);
					level->setTileAndData(x + xx, y + yy+99+height, z + zz, Tile::dirt_Id, 0, Tile::UPDATE_CLIENTS);
					level->setTileAndData(x + xx, y + yy+98+height, z + zz, Tile::stone_Id, 0, Tile::UPDATE_CLIENTS);
				}
			}
		}
	}

	for (int xx = 0; xx < 4; xx++)
	{
		for (int zz = 0; zz < 4; zz++)
		{
			for (int yy = 0; yy < 6; yy++)
			{
				if (grid[((xx) * 16 + (zz)) * 8 + (yy)])
				{
						level->setTileAndData(x + xx, y + yy+93+height, z + zz, Tile::stone_Id, 0, Tile::UPDATE_CLIENTS);
				}
			}
		}
	}

	for (int xx = 0; xx < 12; xx++)
	{
		for (int zz = 0; zz < 12; zz++)
		{
			for (int yy = 0; yy < 3; yy++)
			{
				if (grid[((xx) * 16 + (zz)) * 8 + (yy)])
				{
					if (!level->isEmptyTile(x + xx+1, y + yy+105+height, z + zz+1))
					{
						level->setTileAndData(x + xx, y + yy+105+height, z + zz, 0, 0, Tile::UPDATE_CLIENTS);
						level->setTileAndData(x + xx, y + yy+104+height, z + zz, Tile::water_Id, 0, Tile::UPDATE_CLIENTS);
					}
				}
			}
		}
	}

	return true;
}