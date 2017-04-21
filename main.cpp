//
//  main.cpp
//  move_creek
//
//  Created by a1091793 on 22/01/2015.
//  Copyright (c) 2015 University of Adelaide. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <GDAL/GDAL.h>

#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/foreach.hpp>

#include "MoveCreekTypes.h"
#include "ReadInMap.h"
#include "MoveDown.h"




int main(int argc, const char * argv[])
{
    
    
    /********************************************************************
     *                  STEP 1 READ IN MAPS                             *
     ********************************************************************/

    std::string feature_file;
    std::string dem_file;
    std::string copied_feature_file;
    std::string copied_dem_file;
    std::string new_out_feature_file;
    std::string log_file;
    
    unsigned neighbour_size;
    unsigned feature_adjacents;
    unsigned stackatop;
    unsigned repeat;
    
    namespace prog_opt = boost::program_options;
    prog_opt::options_description desc("Allowed options");
    desc.add_options()
     ("help,h", "produce help message")
    ("feature-map,f", prog_opt::value<std::string>(&feature_file), "path of the gdal capatible raster feature file")
    ("dem-map,d", prog_opt::value<std::string>(&dem_file), "path of the gdal capatible elevation data file")
    ("moved-feature-map,o", prog_opt::value<std::string>(&new_out_feature_file)->default_value("out-feature.tiff"), "path of the output moved feature file")
    ("copied_dem_file,c", prog_opt::value<std::string>(&copied_dem_file)->default_value("copied-dem.tiff"), "path of the copied dem  file for debug purposes")
    ("copied_feature_file,k", prog_opt::value<std::string>(&copied_feature_file)->default_value("copied-feature.tiff"), "path of the copied dem  file for debug purposes")
    ("log_file,l", prog_opt::value<std::string>(&log_file)->default_value("moves.log"), "path of the file which logs feature cell movements")
    ("neighbourhood,n", prog_opt::value<unsigned>(&neighbour_size)->default_value(40),
     "Size of neighbourhood (maximum move per iteration)")
    ("adjacents,a", prog_opt::value<unsigned>(&feature_adjacents)->default_value(2),
     "how close adjacent feature cells need to be to be considered adjacent")
    ("stackatop,s", prog_opt::value<unsigned>(&stackatop)->default_value(1000),
     "whether different feature cells can be moved to the same moved location (i.e. staked atop each other, and how many can be stacked atop")
    ("repeat,r", prog_opt::value<unsigned>(&repeat)->default_value(1),
     "How many times to repeat the analysis, feeding the output feature back in as the input feature (iterate/recuse)");

    prog_opt::variables_map vm;
    prog_opt::store(prog_opt::parse_command_line(argc, argv, desc), vm);
    prog_opt::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    
    boost::filesystem::path feature_file_path(feature_file);
    boost::filesystem::path dem_file_path(dem_file);
    
    boost::filesystem::path new_out_feature_file_path(new_out_feature_file);
    boost::filesystem::path copied_feature_file_path(copied_feature_file);
    boost::filesystem::path copied_dem_file_path(copied_dem_file);
    
    // Check file exists
    if (!fs::exists(feature_file_path))
    {
        std::stringstream ss;
        ss << feature_file_path << " does not exist";
        throw std::runtime_error(ss.str());
        return (1);
    }
    
    if (!fs::exists(dem_file_path))
    {
        std::stringstream ss;
        ss << dem_file_path << " does not exist";
        throw std::runtime_error(ss.str());
        return (1);
    }
    
    
    // Read in maps
    
    std::tuple<boost::shared_ptr<Map_Matrix<int32_t> >, std::string, GeoTransform> gdal_feature_map = read_in_map<int32_t>(feature_file_path, GDT_Int32);
    boost::shared_ptr<Map_Matrix<int32_t> > feature_map(std::get<0>(gdal_feature_map));
    std::string & featureWKTprojection(std::get<1>(gdal_feature_map));
    GeoTransform & featureTransform(std::get<2>(gdal_feature_map));
    
    
//    std::cout << "Map 1: \n";
//    feature_map->print();

    std::tuple<boost::shared_ptr<Map_Matrix<double> >, std::string, GeoTransform> gdal_dem_map = read_in_map<double>(dem_file_path, GDT_Float64);
    boost::shared_ptr<Map_Matrix<double> > dem_map(std::get<0>(gdal_dem_map));
    std::string & demWKTprojection(std::get<1>(gdal_dem_map));
    GeoTransform & demTransform(std::get<2>(gdal_dem_map));
//    std::cout << "Map 2: \n";
//    dem_map->print();
    
    
    //Check maps for consistency (same dimensions)
    if (feature_map->NCols() != dem_map->NCols())
    {
        throw std::runtime_error("Number of columns in the two comparison maps non-identical");
    }
    
    if (feature_map->NRows() != dem_map->NRows())
    {
        throw std::runtime_error("Number of rows in the two comparison maps non-identical");
    }
    
    
    /********************************************************************
    *                  STEP 2 Do the processing                         *
    ********************************************************************/
    
    Map_Int_SPtr new_feature_map;
    Map_Int_SPtr old_feature_map(feature_map);
    Map_Int_SPtr stackontop_map;
    Move_Log move_log;
    
//    std::tie(new_feature_map, stackontop_map, log) =
//    move_down(feature_map,  dem_map, neighbour_size, feature_adjacents,                                                                                                                                                                              stackatop);
    
    for (unsigned i = 0; i < repeat; ++i)
    {
        std::tie(new_feature_map, stackontop_map, move_log) =
        move_down(old_feature_map,  dem_map, neighbour_size, feature_adjacents,                                                                                                                                                                              stackatop);
        old_feature_map = new_feature_map;
    }
    

    /********************************************************************
     *                  STEP 3 Print the new feature to raster file     *
     ********************************************************************/
    printf( "The following format drivers are configured and support output:\n" );
    printf( " CREATE:\n" );
    int	iDr;
    for( iDr = 0; iDr < GDALGetDriverCount(); iDr++ )
    {
        GDALDriverH hDriver = GDALGetDriver(iDr);
        
        
        
        if( GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATE, NULL ) != NULL
           )
        {
            printf( "  %s: %s\n",
                   GDALGetDriverShortName( hDriver  ),
                   GDALGetDriverLongName( hDriver ) );
        }
    }
    printf( "\n" );
    printf( " CREATECOPY:\n" );
    for( iDr = 0; iDr < GDALGetDriverCount(); iDr++ )
    {
        GDALDriverH hDriver = GDALGetDriver(iDr);
        
        
        
        if( GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATECOPY,
                                  NULL ) != NULL )
        {
            printf( "  %s: %s\n",
                   GDALGetDriverShortName( hDriver  ),
                   GDALGetDriverLongName( hDriver ) );
        }
    }
    printf( "\n" );
    
    std::string driverName = "GTiff";
    write_map(new_out_feature_file_path, GDT_Int32, new_feature_map, demWKTprojection, demTransform, driverName);
    write_map(copied_feature_file_path, GDT_Int32, feature_map, demWKTprojection, demTransform, driverName);
    write_map(copied_dem_file_path, GDT_Float64, dem_map, demWKTprojection, demTransform, driverName);
    
    std::ofstream fs(log_file.c_str());
    if (!fs.is_open())
    {
        std::stringstream ss;
        ss << "could not create or open " << log_file;
        throw std::runtime_error(ss.str());
        return (1);
    }
    
    
    typedef std::pair<Position, Position> LogPair;
    BOOST_FOREACH(LogPair & movement, move_log)
    {
        fs << "(" << movement.first.first << ", " << movement.first.second << ") --> (" << movement.second.first << ", " << movement.second.second << ")\n";
    }
    fs.close();
    
    return (EXIT_SUCCESS);
}



//
//    /********************************************************************
//     *                            OLD STUFF                             *
//     ********************************************************************/


//    ///???????????????? Columns then rows? this seems wrong
//    boost::shared_ptr<Map_Matrix<int32_t> > new_feature_map(new Map_Matrix<int>(feature_map->NRows(), feature_map->NCols(), feature_map->NoDataValue()));
//    new_feature_map->SetNoDataValue(feature_map->NoDataValue());





//
//    //Iterate over each feature cell in the feature raster
//    for (int i = 0; i < feature_map->NRows(); ++i)
//    {
//        for (int j = 0; j < feature_map->NCols(); ++j)
//        {
//            //Store val here for debugging purposes, rather than placing inline within the if statement following.
//            int32_t val = feature_map->Get(i, j);
//            if (val != feature_map->NoDataValue())
//            {
//                boost::shared_ptr<Set> Nh = get_neighbourhood(dem_map, i, j, neighbour_size);
//                boost::shared_ptr<Set> Af = find_adjacents(feature_map, i, j, feature_neigh);
//                unsigned long Na = Af->size();
//                Position lowest_position = std::make_pair(0,0);
//                double sum_lowest_pos = std::numeric_limits<double>::max();
//
//                BOOST_FOREACH(Position pos, *Nh)
//                {
//                    double sum = 0;
//                    unsigned long undefined_cells = 0;
//                    BOOST_FOREACH(Position offset, *Af)
//                    {
//                        Position location = apply_offset(pos, offset);
//                        if (is_in_bounds(location, dem_map))
//                        {
//                            sum += dem_map->Get(location.first, location.second);
//                        } else
//                        {
//                            ++undefined_cells;
//                        }
//                    }
//                    sum *= (double)Na / ((double)( Na - undefined_cells));
//                    if (sum < sum_lowest_pos)
//                    {
//                        sum_lowest_pos = sum;
//                        lowest_position = pos;
//                    }
//                }
//
//                if (euclidean_distance(i, j, lowest_position.first, lowest_position.second) > neighbour_size)
//                {
//                    throw std::runtime_error("Program logic error");
//                }
//                new_feature_map->Get(lowest_position.first, lowest_position.second) = val;
//
//            }
//        }
//    }

