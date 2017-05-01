#include "World.h"

EagleBird::World::World(const uint32_t x_size, const uint32_t y_size, const uint32_t z_size)
{
	_size_x = x_size;
	_size_y = y_size;
	_size_z = z_size;
	int seed = rand();

	_world_generator = new WorldGeneration::WorldGenerator(_size_x, _size_y, _size_z, seed);
	_world_generator->GenerateWorld();
}

void EagleBird::World::Init()
{
}

uint32_t EagleBird::World::GetSizeX()
{
	return _size_x;
}

uint32_t EagleBird::World::GetSizeY()
{
	return _size_y;
}

uint32_t EagleBird::World::GetSizeZ()
{
	return _size_z;
}

EagleBird::WorldGeneration::WorldGenerator * EagleBird::World::GetWorldGenerator()
{
	return _world_generator;
}



float EagleBird::World::GetHeight(float x, float y)
{
	return _world_generator->GetHeight(x, y);
}
