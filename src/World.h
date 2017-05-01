#pragma once
#define WORLD_SIZE 20
#include <iostream>
#include <fstream>
#include "world_generation\WorldGenerator.h"

namespace EagleBird
{
	class World
	{
	public:
		World(const uint32_t x_size, const uint32_t y_size, const uint32_t z_size);
		~World();
		void Init();
		
		uint32_t GetSizeX();
		uint32_t GetSizeY();
		uint32_t GetSizeZ();
		WorldGeneration::WorldGenerator* GetWorldGenerator();
		float GetHeight(float x, float y);
	private:
		uint32_t _size_x;
		uint32_t _size_y;
		uint32_t _size_z;
		WorldGeneration::WorldGenerator* _world_generator;
	};
}