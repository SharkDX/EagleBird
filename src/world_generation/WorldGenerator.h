#pragma once
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <noise\noise.h>
#include "..\Config.h"
#include "noiseutils.h"
#include "open-simplex-noise.h"
#include "BiomeManager.h"

namespace EagleBird
{
	namespace WorldGeneration
	{
		class WorldGenerator
		{
		public:
			WorldGenerator(const uint32_t x_size, const uint32_t y_size, const uint32_t z_size, const int seed);
			virtual ~WorldGenerator();
			WorldGenerator(const WorldGenerator&) = delete;
			WorldGenerator& operator=(const WorldGenerator&) = delete;
			void GenerateWorld();

			module::Module* _terrain_type;
			
			float GetHeight(float x, float y);
			glm::vec3 GetColor(float x, float y);
		private:
			module::Module* GenerateBaseElevation(float seed, float general_frequency);
			module::Cache* GenerateBadlands(float seed, float general_frequency);
			module::Module* GenerateRivers(float seed, float general_frequency);
			float normal_float(float f);

			int _seed;
			uint32_t _size_x;
			uint32_t _size_y;
			uint32_t _size_z;
			noise::utils::NoiseMap* _height_map;
			noise::utils::NoiseMap* _temperature_map;
			module::Module* _biome_module;
			BiomeManager* _biomes_manager;
			struct osn_context* _ctx;
		};
	}
}