#include "WorldGenerator.h"

EagleBird::WorldGeneration::WorldGenerator::WorldGenerator(const uint32_t x_size, const uint32_t y_size, const uint32_t z_size, const int seed)
{
	_size_x = x_size;
	_size_y = y_size;
	_size_z = z_size;
	this->_seed = seed;
}

EagleBird::WorldGeneration::WorldGenerator::~WorldGenerator()
{
}

void EagleBird::WorldGeneration::WorldGenerator::GenerateWorld()
{
	
	std::cout << "World generation seed: " << _seed << std::endl;

	_height_map = new utils::NoiseMap();
	_temperature_map = new utils::NoiseMap();
	
	module::Perlin* biome_module = new module::Perlin();
	biome_module->SetSeed(_seed + 1);
	biome_module->SetFrequency(0.002);
	biome_module->SetPersistence(0.2);
	biome_module->SetOctaveCount(2);
	module::Clamp* biome_module_cl = new module::Clamp();
	biome_module_cl->SetSourceModule(0, *biome_module);
	biome_module_cl->SetBounds(-1.0, 1.0);
	_biome_module = biome_module_cl;
	_biomes_manager = new BiomeManager(biome_module_cl);


	_biomes_manager->AddBiome(new FlatBiome(_seed, LOWESET_TEMPERATURE, -0.1));
	_biomes_manager->AddBiome(new MountainBiome(_seed, -0.1, 0.3));
	_biomes_manager->AddBiome(new DesetBiome(_seed, 0.3, HIGHEST_TEMPERATURE));

	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(*_biomes_manager);
	heightMapBuilder.SetDestNoiseMap(*_height_map);
	heightMapBuilder.SetDestSize(_size_x / SIMPLENESS, _size_z / SIMPLENESS);
	heightMapBuilder.SetBounds(0.0, _size_x / SIMPLENESS, 0.0, _size_z / SIMPLENESS);
	heightMapBuilder.Build();

	/*utils::RendererImage renderer;
	utils::Image image;
	renderer.SetSourceNoiseMap(*_height_map);
	renderer.SetDestImage(image);
	renderer.ClearGradient();
	renderer.AddGradientPoint(-1.0000, utils::Color(0, 0, 128, 255)); // deeps
	renderer.AddGradientPoint(-0.2500, utils::Color(0, 0, 255, 255)); // shallow
	renderer.AddGradientPoint(0.0000, utils::Color(0, 128, 255, 255)); // shore
	renderer.AddGradientPoint(0.001, utils::Color(240, 240, 64, 255)); // sand
	renderer.AddGradientPoint(0.1250, utils::Color(32, 160, 0, 255)); // grass
	renderer.EnableLight();
	renderer.SetLightContrast(3.0);
	renderer.SetLightBrightness(2.0);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename("tutorial.bmp");
	writer.WriteDestFile();*/

	//exit(0);
}

float EagleBird::WorldGeneration::WorldGenerator::GetHeight(float x, float y)
{
	return (_biomes_manager->GetValue(x, 0.0, y) + 1.0) / 2.0 * _size_y;
	//return (_height_map->GetValue(x, y) + 1.0) / 2.0 * _size_y;
}

glm::vec3 EagleBird::WorldGeneration::WorldGenerator::GetColor(float x, float y)
{
	return _biomes_manager->GetColor(x, y);
}

module::Module * EagleBird::WorldGeneration::WorldGenerator::GenerateBaseElevation(float seed, float general_frequency)
{
	// Frequency of the planet's continents.  Higher frequency produces smaller,
	// more numerous continents.  This value is measured in radians.
	const double CONTINENT_FREQUENCY = 0.002;

	// Lacunarity of the planet's continents.  Changing this value produces
	// slightly different continents.  For the best results, this value should
	// be random, but close to 2.0.
	const double CONTINENT_LACUNARITY = 2.208984375;

	// Specifies the level on the planet in which continental shelves appear.
	// This value must be between -1.0 (minimum planet elevation) and +1.0
	// (maximum planet elevation), and must be less than SEA_LEVEL.
	const double SHELF_LEVEL = -0.375;
	const double SEA_LEVEL = 0.0;
	const double CONTINENT_HEIGHT_SCALE = (1.0 - SEA_LEVEL) / 4.0;

	// Offset to apply to the terrain type definition.  Low values (< 1.0) cause
	// the rough areas to appear only at high elevations.  High values (> 2.0)
	// cause the rough areas to appear at any elevation.  The percentage of
	// rough areas on the planet are independent of this value.
	const double TERRAIN_OFFSET = 1.0;

	// 1: [Continent module]: This Perlin-noise module generates the continents.
	//    This noise module has a high number of octaves so that detail is
	//    visible at high zoom levels.
	module::Perlin* baseContinentDef_pe0 = new module::Perlin();
	baseContinentDef_pe0->SetSeed(seed + 0);
	baseContinentDef_pe0->SetFrequency(CONTINENT_FREQUENCY);
	baseContinentDef_pe0->SetPersistence(0.5);
	baseContinentDef_pe0->SetLacunarity(CONTINENT_LACUNARITY);
	baseContinentDef_pe0->SetOctaveCount(4);
	baseContinentDef_pe0->SetNoiseQuality(QUALITY_STD);

	// 2: [Continent-with-ranges module]: Next, a curve module modifies the
	//    output value from the continent module so that very high values appear
	//    near sea level.  This defines the positions of the mountain ranges.
	module::Curve* baseContinentDef_cu = new module::Curve();
	baseContinentDef_cu->SetSourceModule(0, *baseContinentDef_pe0);
	baseContinentDef_cu->AddControlPoint(-2.0000 + SEA_LEVEL, -1.625 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(-1.0000 + SEA_LEVEL, -1.375 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(0.0000 + SEA_LEVEL, -0.375 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(0.0625 + SEA_LEVEL, 0.125 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(0.1250 + SEA_LEVEL, 0.250 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(0.2500 + SEA_LEVEL, 1.000 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(0.5000 + SEA_LEVEL, 0.250 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(0.7500 + SEA_LEVEL, 0.250 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(1.0000 + SEA_LEVEL, 0.500 + SEA_LEVEL);
	baseContinentDef_cu->AddControlPoint(2.0000 + SEA_LEVEL, 0.500 + SEA_LEVEL);

	// 3: [Carver module]: This higher-frequency Perlin-noise module will be
	//    used by subsequent noise modules to carve out chunks from the mountain
	//    ranges within the continent-with-ranges module so that the mountain
	//    ranges will not be complely impassible.
	module::Perlin* baseContinentDef_pe1 = new module::Perlin();
	baseContinentDef_pe1->SetSeed(seed + 1);
	baseContinentDef_pe1->SetFrequency(CONTINENT_FREQUENCY * 4.34375);
	baseContinentDef_pe1->SetPersistence(0.5);
	baseContinentDef_pe1->SetLacunarity(CONTINENT_LACUNARITY);
	baseContinentDef_pe1->SetOctaveCount(11);
	baseContinentDef_pe1->SetNoiseQuality(QUALITY_STD);

	// 4: [Scaled-carver module]: This scale/bias module scales the output
	//    value from the carver module such that it is usually near 1.0.  This
	//    is required for step 5.
	module::ScaleBias* baseContinentDef_sb = new module::ScaleBias();
	baseContinentDef_sb->SetSourceModule(0, *baseContinentDef_pe1);
	baseContinentDef_sb->SetScale(0.375);
	baseContinentDef_sb->SetBias(0.625);

	// 5: [Carved-continent module]: This minimum-value module carves out chunks
	//    from the continent-with-ranges module.  It does this by ensuring that
	//    only the minimum of the output values from the scaled-carver module
	//    and the continent-with-ranges module contributes to the output value
	//    of this subgroup.  Most of the time, the minimum-value module will
	//    select the output value from the continents-with-ranges module since
	//    the output value from the scaled-carver module is usually near 1.0.
	//    Occasionally, the output value from the scaled-carver module will be
	//    less than the output value from the continent-with-ranges module, so
	//    in this case, the output value from the scaled-carver module is
	//    selected.
	module::Min* baseContinentDef_mi = new module::Min();
	baseContinentDef_mi->SetSourceModule(0, *baseContinentDef_sb);
	baseContinentDef_mi->SetSourceModule(1, *baseContinentDef_cu);

	// 6: [Clamped-continent module]: Finally, a clamp module modifies the
	//    carved-continent module to ensure that the output value of this
	//    subgroup is between -1.0 and 1.0.
	module::Clamp* baseContinentDef_cl = new module::Clamp();
	baseContinentDef_cl->SetSourceModule(0, *baseContinentDef_cu);
	baseContinentDef_cl->SetBounds(-1.0, 1.0);

	////////////////////////////////////////////////////////////////////////////
	// Module group: base continent elevations (3 noise modules)
	//
	// This subgroup generates the base elevations for the continents, before
	// terrain features are added.
	//
	// The output value from this module subgroup is measured in planetary
	// elevation units (-1.0 for the lowest underwater trenches and +1.0 for the
	// highest mountain peaks.)
	//

	// 1: [Base-scaled-continent-elevations module]: This scale/bias module
	//    scales the output value from the continent-definition group so that it
	//    is measured in planetary elevation units 
	module::ScaleBias* baseContinentElev_sb = new module::ScaleBias();
	baseContinentElev_sb->SetSourceModule(0, *baseContinentDef_cl);
	baseContinentElev_sb->SetScale(CONTINENT_HEIGHT_SCALE);
	baseContinentElev_sb->SetBias(0.0);

	// 3: [Base-continent-elevation subgroup]: Caches the output value from the
	//    base-continent-with-oceans module.
	module::Cache* baseContinentElev = new module::Cache();
	baseContinentElev->SetSourceModule(0, *baseContinentElev_sb);

	return baseContinentElev;
}

module::Cache* EagleBird::WorldGeneration::WorldGenerator::GenerateBadlands(float seed, float general_frequency)
{
	const double DUNES_FREQUENCY = 4.0;
	const double DUNES_AMPLITUDE = 0.5;
	const double CLIFFS_FREQUENCY = 10.0;

	module::RidgedMulti* badlandsSand_rm = new module::RidgedMulti();
	badlandsSand_rm->SetSeed(seed + 80);
	badlandsSand_rm->SetFrequency(general_frequency * 0.18 * DUNES_FREQUENCY);
	badlandsSand_rm->SetLacunarity(2.212890625);
	badlandsSand_rm->SetNoiseQuality(QUALITY_STD);
	badlandsSand_rm->SetOctaveCount(1);

	// 2: [Scaled-sand-dunes module]: This scale/bias module shrinks the dune
	//    heights by a small amount.  This is necessary so that the subsequent
	//    noise modules in this subgroup can add some detail to the dunes.
	module::ScaleBias* badlandsSand_sb0 = new module::ScaleBias();
	badlandsSand_sb0->SetSourceModule(0, *badlandsSand_rm);
	badlandsSand_sb0->SetScale(0.875);
	badlandsSand_sb0->SetBias(0.1);

	// 3: [Dune-detail module]: This noise module uses Voronoi polygons to
	//    generate the detail to add to the dunes.  By enabling the distance
	//    algorithm, small polygonal pits are generated; the edges of the pits
	//    are joined to the edges of nearby pits.
	module::Voronoi* badlandsSand_vo = new module::Voronoi();
	badlandsSand_vo->SetSeed(seed + 81);
	badlandsSand_vo->SetFrequency(general_frequency * 1.0 * DUNES_FREQUENCY);
	badlandsSand_vo->SetDisplacement(0.0);
	badlandsSand_vo->EnableDistance();

	// 4: [Scaled-dune-detail module]: This scale/bias module shrinks the dune
	//    details by a large amount.  This is necessary so that the subsequent
	//    noise modules in this subgroup can add this detail to the sand-dunes
	//    module.
	module::ScaleBias* badlandsSand_sb1 = new module::ScaleBias();
	badlandsSand_sb1->SetSourceModule(0, *badlandsSand_vo);
	badlandsSand_sb1->SetScale(0.15);
	badlandsSand_sb1->SetBias(0.3);

	// 5: [Dunes-with-detail module]: This addition module combines the scaled-
	//    sand-dunes module with the scaled-dune-detail module.
	module::Add* badlandsSand_ad = new module::Add();
	badlandsSand_ad->SetSourceModule(0, *badlandsSand_sb0);
	badlandsSand_ad->SetSourceModule(1, *badlandsSand_sb1);

	// 6: [Badlands-sand subgroup]: Caches the output value from the dunes-with-
	//    detail module.
	module::Cache* badlandsSand = new module::Cache();
	badlandsSand->SetSourceModule(0, *badlandsSand_ad);

	////////////////////////////////////////////////////////////////////////////
	// Module subgroup: badlands cliffs (7 noise modules)
	//
	// This subgroup generates the cliffs for the badlands.
	//
	// -1.0 represents the lowest elevations and +1.0 represents the highest
	// elevations.
	//

	// 1: [Cliff-basis module]: This Perlin-noise module generates some coherent
	//    noise that will be used to generate the cliffs.
	module::Perlin* badlandsCliffs_pe = new module::Perlin();
	badlandsCliffs_pe->SetSeed(seed + 90);
	badlandsCliffs_pe->SetFrequency(839.0 / 16000.0 * general_frequency * CLIFFS_FREQUENCY);
	badlandsCliffs_pe->SetPersistence(0.5);
	badlandsCliffs_pe->SetLacunarity(2.212890625);
	badlandsCliffs_pe->SetOctaveCount(6);
	badlandsCliffs_pe->SetNoiseQuality(QUALITY_STD);

	// 2: [Cliff-shaping module]: Next, this curve module applies a curve to the
	//    output value from the cliff-basis module.  This curve is initially
	//    very shallow, but then its slope increases sharply.  At the highest
	//    elevations, the curve becomes very flat again.  This produces the
	//    stereotypical Utah-style desert cliffs.
	module::Curve* badlandsCliffs_cu = new module::Curve();
	badlandsCliffs_cu->SetSourceModule(0, *badlandsCliffs_pe);
	badlandsCliffs_cu->AddControlPoint(-2.0000, -2.0000);
	badlandsCliffs_cu->AddControlPoint(-1.0000, -1.2500);
	badlandsCliffs_cu->AddControlPoint(-0.0000, -0.7500);
	badlandsCliffs_cu->AddControlPoint(0.5000, -0.2500);
	badlandsCliffs_cu->AddControlPoint(0.6250, 0.8750);
	badlandsCliffs_cu->AddControlPoint(0.7500, 1.0000);
	badlandsCliffs_cu->AddControlPoint(2.0000, 1.2500);

	// 3: [Clamped-cliffs module]: This clamping module makes the tops of the
	//    cliffs very flat by clamping the output value from the cliff-shaping
	//    module so that the tops of the cliffs are very flat.
	module::Clamp* badlandsCliffs_cl = new module::Clamp();
	badlandsCliffs_cl->SetSourceModule(0, *badlandsCliffs_pe);
	badlandsCliffs_cl->SetBounds(0, 0.875);

	// 4: [Terraced-cliffs module]: Next, this terracing module applies some
	//    terraces to the clamped-cliffs module in the lower elevations before
	//    the sharp cliff transition.
	module::Terrace* badlandsCliffs_te = new module::Terrace();
	badlandsCliffs_te->SetSourceModule(0, *badlandsCliffs_cl);
	badlandsCliffs_te->AddControlPoint(-1.0000);
	badlandsCliffs_te->AddControlPoint(-0.8750);
	badlandsCliffs_te->AddControlPoint(-0.7500);
	badlandsCliffs_te->AddControlPoint(-0.5000);
	badlandsCliffs_te->AddControlPoint(0.0000);
	badlandsCliffs_te->AddControlPoint(1.0000);

	// 5: [Coarse-turbulence module]: This turbulence module warps the output
	//    value from the terraced-cliffs module, adding some coarse detail to
	//    it.
	/*module::Turbulence* badlandsCliffs_tu0 = new module::Turbulence();
	badlandsCliffs_tu0->SetSeed(seed + 91);
	badlandsCliffs_tu0->SetSourceModule(0, *badlandsCliffs_te);
	badlandsCliffs_tu0->SetFrequency(general_frequency * CLIFFS_FREQUENCY);
	badlandsCliffs_tu0->SetPower(1.0 / 141539.0 / 16000.0 * general_frequency * CLIFFS_FREQUENCY);
	badlandsCliffs_tu0->SetRoughness(3);

	// 6: [Warped-cliffs module]: This turbulence module warps the output value
	//    from the coarse-turbulence module.  This turbulence has a higher
	//    frequency, but lower power, than the coarse-turbulence module, adding
	//    some fine detail to it.
	module::Turbulence* badlandsCliffs_tu1 = new module::Turbulence();
	badlandsCliffs_tu1->SetSeed(seed + 92);
	badlandsCliffs_tu1->SetSourceModule(0, *badlandsCliffs_tu0);
	badlandsCliffs_tu1->SetFrequency(36107.0 / 16000.0 * general_frequency * CLIFFS_FREQUENCY);
	badlandsCliffs_tu1->SetPower(1.0 / 211543.0 / 16000.0 * general_frequency * CLIFFS_FREQUENCY);
	badlandsCliffs_tu1->SetRoughness(3);*/

	// 7: [Badlands-cliffs subgroup]: Caches the output value from the warped-
	//    cliffs module.
	module::Cache* badlandsCliffs = new module::Cache();
	badlandsCliffs->SetSourceModule(0, *badlandsCliffs_te);


	////////////////////////////////////////////////////////////////////////////
	// Module subgroup: badlands terrain (3 noise modules)
	//
	// Generates the final badlands terrain.
	//
	// Using a scale/bias module, the badlands sand is flattened considerably,
	// then the sand elevations are lowered to around -1.0.  The maximum value
	// from the flattened sand module and the cliff module contributes to the
	// final elevation.  This causes sand to appear at the low elevations since
	// the sand is slightly higher than the cliff base.
	//
	// -1.0 represents the lowest elevations and +1.0 represents the highest
	// elevations.
	//

	// 1: [Scaled-sand-dunes module]: This scale/bias module considerably
	//    flattens the output value from the badlands-sands subgroup and lowers
	//    this value to near -1.0.
	module::ScaleBias* badlandsTerrain_sb = new module::ScaleBias();
	badlandsTerrain_sb->SetSourceModule(0, *badlandsSand);
	badlandsTerrain_sb->SetScale(DUNES_AMPLITUDE);
	badlandsTerrain_sb->SetBias(0.3);

	// 2: [Dunes-and-cliffs module]: This maximum-value module causes the dunes
	//    to appear in the low areas and the cliffs to appear in the high areas.
	//    It does this by selecting the maximum of the output values from the
	//    scaled-sand-dunes module and the badlands-cliffs subgroup.
	module::Max* badlandsTerrain_ma = new module::Max();
	badlandsTerrain_ma->SetSourceModule(0, *badlandsCliffs);
	badlandsTerrain_ma->SetSourceModule(1, *badlandsTerrain_sb);

	// 3: [Badlands-terrain group]: Caches the output value from the dunes-and-
	//    cliffs module.  This is the output value for the entire badlands-
	//    terrain group.
	module::Cache* badlandsTerrain = new module::Cache();
	badlandsTerrain->SetSourceModule(0, *badlandsTerrain_ma);
	
	return badlandsTerrain;
}


module::Module * EagleBird::WorldGeneration::WorldGenerator::GenerateRivers(float seed, float general_frequency)
{
	const double CONTINENT_LACUNARITY = 2.208984375;
	const double RIVER_DEPTH = 0.6;
	
	float scale = 0.001f;
	////////////////////////////////////////////////////////////////////////////
	// Module group: river positions
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	// Module subgroup: river positions (7 noise modules)
	//
	// This subgroup generates the river positions.
	//
	// -1.0 represents the lowest elevations and +1.0 represents the highest
	// elevations.
	//

	// 1: [Large-river-basis module]: This ridged-multifractal-noise module
	//    creates the large, deep rivers.
	module::RidgedMulti* riverPositions_rm0 = new module::RidgedMulti();
	riverPositions_rm0->SetSeed(seed + 100);
	riverPositions_rm0->SetFrequency(18.75 * scale);
	riverPositions_rm0->SetLacunarity(CONTINENT_LACUNARITY);
	riverPositions_rm0->SetOctaveCount(1);
	riverPositions_rm0->SetNoiseQuality(QUALITY_BEST);

	// 2: [Large-river-curve module]: This curve module applies a curve to the
	//    output value from the large-river-basis module so that the ridges
	//    become inverted.  This creates the rivers.  This curve also compresses
	//    the edge of the rivers, producing a sharp transition from the land to
	//    the river bottom.
	module::Curve* riverPositions_cu0 = new module::Curve();
	riverPositions_cu0->SetSourceModule(0, *riverPositions_rm0);
	riverPositions_cu0->AddControlPoint(-2.000, 2.000);
	riverPositions_cu0->AddControlPoint(-1.000, 1.000);
	riverPositions_cu0->AddControlPoint(-0.125, 0.875);
	riverPositions_cu0->AddControlPoint(0.000, -1.000);
	riverPositions_cu0->AddControlPoint(1.000, -1.500);
	riverPositions_cu0->AddControlPoint(2.000, -2.000);

	/// 3: [Small-river-basis module]: This ridged-multifractal-noise module
	//     creates the small, shallow rivers.
	module::RidgedMulti* riverPositions_rm1 = new module::RidgedMulti();
	riverPositions_rm1->SetSeed(seed + 101);
	riverPositions_rm1->SetFrequency(43.25 * scale);
	riverPositions_rm1->SetLacunarity(CONTINENT_LACUNARITY);
	riverPositions_rm1->SetOctaveCount(1);
	riverPositions_rm1->SetNoiseQuality(QUALITY_BEST);

	// 4: [Small-river-curve module]: This curve module applies a curve to the
	//    output value from the small-river-basis module so that the ridges
	//    become inverted.  This creates the rivers.  This curve also compresses
	//    the edge of the rivers, producing a sharp transition from the land to
	//    the river bottom.
	module::Curve* riverPositions_cu1 = new module::Curve();
	riverPositions_cu1->SetSourceModule(0, *riverPositions_rm1);
	riverPositions_cu1->AddControlPoint(-2.000, 2.0000);
	riverPositions_cu1->AddControlPoint(-1.000, 1.5000);
	riverPositions_cu1->AddControlPoint(-0.125, 1.4375);
	riverPositions_cu1->AddControlPoint(0.000, 0.5000);
	riverPositions_cu1->AddControlPoint(1.000, 0.2500);
	riverPositions_cu1->AddControlPoint(2.000, 0.0000);

	// 5: [Combined-rivers module]: This minimum-value module causes the small
	//    rivers to cut into the large rivers.  It does this by selecting the
	//    minimum output values from the large-river-curve module and the small-
	//    river-curve module.
	module::Min* riverPositions_mi = new module::Min();
	riverPositions_mi->SetSourceModule(0, *riverPositions_cu0);
	riverPositions_mi->SetSourceModule(1, *riverPositions_cu1);

	// 6: [Warped-rivers module]: This turbulence module warps the output value
	//    from the combined-rivers module, which twists the rivers.  The high
	//    roughness produces less-smooth rivers.
	module::Turbulence* riverPositions_tu = new module::Turbulence();
	riverPositions_tu->SetSourceModule(0, *riverPositions_mi);
	riverPositions_tu->SetSeed(seed + 102);
	riverPositions_tu->SetFrequency(9.25 * scale);
	riverPositions_tu->SetPower(1.0 / 57.75 * scale);
	riverPositions_tu->SetRoughness(6);

	// 7: [River-positions group]: Caches the output value from the warped-
	//    rivers module.  This is the output value for the entire river-
	//    positions group.
	module::Cache* riverPositions = new module::Cache();
	riverPositions->SetSourceModule(0, *riverPositions_tu);

	module::ScaleBias* continentsWithRivers_sb = new module::ScaleBias();
	continentsWithRivers_sb->SetSourceModule(0, *riverPositions);
	continentsWithRivers_sb->SetScale(RIVER_DEPTH / 2.0);
	continentsWithRivers_sb->SetBias(-RIVER_DEPTH / 2.0);

	return continentsWithRivers_sb;
}



float EagleBird::WorldGeneration::WorldGenerator::normal_float(float f)
{
	return f / 2.0f + 0.5f;
}
