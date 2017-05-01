#pragma once
#include <vector>
#include <algorithm>
#include <noise\noise.h>
#include <noise\interp.h>
#include <exception>

namespace EagleBird
{
	namespace WorldGeneration
	{

		class Biome
		{
		public:
			Biome(const double min_temperature, const double max_temperature, noise::module::Module* noise_module)
				: _min_temperature(min_temperature), _max_temperature(max_temperature), _noise_module(noise_module)
			{
				if (max_temperature - min_temperature < EDGE_FALLOFF + EDGE_FALLOFF)
				{
					throw std::exception("Bad argument: the temp's range of the biome must be greater than the edgefalloff * 2");
				}
			}

			double _min_temperature;
			double _max_temperature;
			noise::module::Module* _noise_module;
		};

		class BiomeManager : public noise::module::Module
		{
		public:
			BiomeManager(module::Module* biome_map)
				: _biome_map(biome_map), _edge_falloff(0.125), noise::module::Module(3)
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
				if (biome_index > 0 && biome_value < _biomes[biome_index]->_min_temperature + _edge_falloff)
				{
					double upper_limit = _biomes[biome_index]->_min_temperature + _edge_falloff;
					double lower_limit = _biomes[biome_index]->_min_temperature - _edge_falloff;
					double alpha = SCurve3((biome_value - lower_limit) / (upper_limit - lower_limit));
					return LinearInterp(_biomes[biome_index - 1]->_noise_module->GetValue(x, 0.0, z), _biomes[biome_index]->_noise_module->GetValue(x, 0.0, z), alpha);
				}
				// Check if we are close to the end of the biome, and interpulate it to the next biome.
				else if (biome_index < _biomes.size() - 1 && biome_value > _biomes[biome_index]->_max_temperature - _edge_falloff)
				{
					double upper_limit = _biomes[biome_index]->_max_temperature + _edge_falloff;
					double lower_limit = _biomes[biome_index]->_max_temperature - _edge_falloff;
					double alpha = SCurve3((biome_value - lower_limit) / (upper_limit - lower_limit));
					return LinearInterp(_biomes[biome_index]->_noise_module->GetValue(x, 0.0, z), _biomes[biome_index + 1]->_noise_module->GetValue(x, 0.0, z), alpha);
				}
				else
				{
					return _biomes[biome_index]->_noise_module->GetValue(x, 0.0, z);
				}

				return 0.0;
			};
		private:
			std::vector<Biome*> _biomes;
			module::Module* _biome_map;
			double _edge_falloff;

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