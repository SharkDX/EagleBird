#pragma once
#include <vector>
#include <algorithm>
#include <glm\glm.hpp>
#include <noise\noise.h>
#include <noise\interp.h>
#include <exception>
#include "Biomes.h"

namespace EagleBird
{
	namespace WorldGeneration
	{
		class BiomeManager : public noise::module::Module
		{
		public:
			BiomeManager(module::Module* biome_map)
				: _biome_map(biome_map), noise::module::Module(3)
			{

			}

			virtual int GetSourceModuleCount() const
			{
				return 3;
			}

			void AddBiome(Biome* biome)
			{
				_biomes.push_back(biome);
				CalculateBiomes();
			}
			Biome* GetBiome(const double value){
				for (Biome* biome : _biomes)
				{
					if (value >= biome->_min_temperature && value < biome->_max_temperature)
						return biome;
				}
				return nullptr;
			};

			virtual double GetValue(double x, double y, double z) const
			{
				double biome_value = _biome_map->GetValue(x, 0.0, z);
				int biome_index = 0;
				// Find the index of the current biome.
				for (int i = 0; i < _biomes.size(); i++)
				{
					if (biome_value >= _biomes[i]->_min_temperature && biome_value <= _biomes[i]->_max_temperature)
					{
						biome_index = i;
					}
				}

				// Check if we are close to the end of the biome, and interpulate it to the next biome.
				if (biome_index > 0 && biome_value < _biomes[biome_index]->_min_temperature + BIOME_EDGE_FALLOFF)
				{
					double upper_limit = _biomes[biome_index]->_min_temperature + BIOME_EDGE_FALLOFF;
					double lower_limit = _biomes[biome_index]->_min_temperature - BIOME_EDGE_FALLOFF;
					double alpha = SCurve3((biome_value - lower_limit) / (upper_limit - lower_limit));
					return LinearInterp(_biomes[biome_index - 1]->_noise_module->GetValue(x, 0.0, z), _biomes[biome_index]->_noise_module->GetValue(x, 0.0, z), alpha);
				}
				// Check if we are close to the end of the biome, and interpulate it to the next biome.
				else if (biome_index < _biomes.size() - 1 && biome_value > _biomes[biome_index]->_max_temperature - BIOME_EDGE_FALLOFF)
				{
					double upper_limit = _biomes[biome_index]->_max_temperature + BIOME_EDGE_FALLOFF;
					double lower_limit = _biomes[biome_index]->_max_temperature - BIOME_EDGE_FALLOFF;
					double alpha = SCurve3((biome_value - lower_limit) / (upper_limit - lower_limit));
					return LinearInterp(_biomes[biome_index]->_noise_module->GetValue(x, 0.0, z), _biomes[biome_index + 1]->_noise_module->GetValue(x, 0.0, z), alpha);
				}
				else
				{
					return _biomes[biome_index]->_noise_module->GetValue(x, 0.0, z);
				}

				return 0.0;
			};

			glm::vec3 GetColor(double x, double z)
			{
				double biome_value = _biome_map->GetValue(x, 0.0, z);
				int biome_index = 0;
				// Find the index of the current biome.
				for (int i = 0; i < _biomes.size(); i++)
				{
					if (biome_value >= _biomes[i]->_min_temperature && biome_value <= _biomes[i]->_max_temperature)
					{
						biome_index = i;
					}
				}

				if (biome_index > 0 && biome_value < _biomes[biome_index]->_min_temperature + COLOR_EDGE_FALLOFF)
				{
					double upper_limit = _biomes[biome_index]->_min_temperature + COLOR_EDGE_FALLOFF;
					double lower_limit = _biomes[biome_index]->_min_temperature - COLOR_EDGE_FALLOFF;
					double alpha = SCurve3((biome_value - lower_limit) / (upper_limit - lower_limit));

					glm::vec3 color1 = _biomes[biome_index - 1]->GetColor(x, z, glm::vec3());
					glm::vec3 color2 = _biomes[biome_index]->GetColor(x, z, glm::vec3());

					return glm::vec3(LinearInterp(color1.r, color2.r, alpha),
						LinearInterp(color1.g, color2.g, alpha),
						LinearInterp(color1.b, color2.b, alpha));
				}
				// Check if we are close to the end of the biome, and interpulate it to the next biome.
				else if (biome_index < _biomes.size() - 1 && biome_value > _biomes[biome_index]->_max_temperature - COLOR_EDGE_FALLOFF)
				{
					double upper_limit = _biomes[biome_index]->_max_temperature + COLOR_EDGE_FALLOFF;
					double lower_limit = _biomes[biome_index]->_max_temperature - COLOR_EDGE_FALLOFF;
					double alpha = SCurve3((biome_value - lower_limit) / (upper_limit - lower_limit));

					glm::vec3 color1 = _biomes[biome_index]->GetColor(x, z, glm::vec3());
					glm::vec3 color2 = _biomes[biome_index + 1]->GetColor(x, z, glm::vec3());

					return glm::vec3(LinearInterp(color1.r, color2.r, alpha),
						LinearInterp(color1.g, color2.g, alpha),
						LinearInterp(color1.b, color2.b, alpha));
				}
				else
				{
					return _biomes[biome_index]->GetColor(x, z, glm::vec3());
				}
			}

		private:
			std::vector<Biome*> _biomes;
			module::Module* _biome_map;

			void CalculateBiomes()
			{
				std::sort(_biomes.begin(), _biomes.end(), [](Biome* first, Biome* second) {
					return first->_min_temperature < second->_min_temperature;
				});
				//std::reverse(_biomes.begin(), _biomes.end());
			}
		};
	}
}