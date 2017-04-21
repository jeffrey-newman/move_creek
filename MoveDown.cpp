//
//  MoveDown.cpp
//  move_creek
//
//  Created by Jeffrey Newman on 28/01/2015.
//  Copyright (c) 2015 University of Adelaide. All rights reserved.
//

#include "MoveDown.h"



#include "MoveCreekTypes.h"
#include <boost/foreach.hpp>
#include "Neighbourhood.h"

template <typename DataType>
bool is_in_bounds(Position pos, boost::shared_ptr<Map_Matrix<DataType> > map)
{
    if (pos.first >= map->NRows() || pos.first < 0) return (false);
    if (pos.second >= map->NCols() || pos.second < 0) return (false);
    if (map->Get(pos.first, pos.second) == map->NoDataValue()) return  (false);
    return (true);
}

inline
Position apply_offset(Position pos, Position offset)
{
    return (Position(pos.first + offset.first, pos.second + offset.second));
}

template <typename DataFormat1, typename DataFormat2>
std::tuple  <   Map_Matrix_SPtr<DataFormat1>,
                Map_Matrix_SPtr<DataFormat1>,
                Move_Log     >
move_down(  Map_Matrix_SPtr<DataFormat1> feature_map,
            Map_Matrix_SPtr<DataFormat2> dem_map,
            unsigned neighbour_size,
            unsigned adjacent_size,
            unsigned stackatop
         )
{
    //assert(false);
    std::cout << "In move down\n";
    ///???????????????? Columns then rows? this seems wrong
    boost::shared_ptr<Map_Matrix<DataFormat1> > new_feature_map(new Map_Matrix<DataFormat1>(feature_map->NRows(), feature_map->NCols(), feature_map->NoDataValue()));
    new_feature_map->SetNoDataValue(feature_map->NoDataValue());
    
    ///???????????????? Columns then rows? this seems wrong
    boost::shared_ptr<Map_Matrix<DataFormat1> > stackatop_count_map (new Map_Matrix<DataFormat1>(feature_map->NRows(), feature_map->NCols(), 0));
    new_feature_map->SetNoDataValue(feature_map->NoDataValue());
    
    Move_Log log;
    
    
    //Iterate over each feature cell in the feature raster
    for (int i = 0; i < feature_map->NRows(); ++i)
    {
        for (int j = 0; j < feature_map->NCols(); ++j)
        {
            //Store val here for debugging purposes, rather than placing inline within the if statement following.
            int32_t val = feature_map->Get(i, j);
            if (val != feature_map->NoDataValue())
            {
                boost::shared_ptr<Set> Nh = get_neighbourhood(dem_map, i, j, neighbour_size);
                boost::shared_ptr<Set> Af = find_adjacents(feature_map, i, j, adjacent_size);
                unsigned long Na = Af->size();
                
                Position lowest_position = std::make_pair(i,j);
                double sum = 0;
                unsigned undefined_cells = 0;
                BOOST_FOREACH(Position offset, *Af)
                {
                    Position location(i,j);
                    if (is_in_bounds(location, dem_map))
                    {
                        sum += dem_map->Get(location.first, location.second);
                    } else
                    {
                        ++undefined_cells;
                    }
                }
                sum *= (double)Na / ((double)( Na - undefined_cells));
                double sum_lowest_pos = sum;
                
                BOOST_FOREACH(Position pos, *Nh)
                {
                    BOOST_FOREACH(Position offset, *Af)
                    {
                        Position location = apply_offset(pos, offset);
                        if (is_in_bounds(location, dem_map))
                        {
                            sum += dem_map->Get(location.first, location.second);
                        } else
                        {
                            ++undefined_cells;
                        }
                    }
                    sum *= (double)Na / ((double)( Na - undefined_cells));
                    if (sum < sum_lowest_pos && stackatop_count_map->Get(pos.first, pos.second) < stackatop)
                    {
                        sum_lowest_pos = sum;
                        lowest_position = pos;
                    }
                }
                
                if (euclidean_distance(i, j, lowest_position.first, lowest_position.second) > neighbour_size)
                {
                    throw std::runtime_error("Program logic error");
                }
                new_feature_map->Get(lowest_position.first, lowest_position.second) = val;
                ++(stackatop_count_map->Get(lowest_position.first, lowest_position.second));
                if (i != lowest_position.first && j != lowest_position.second)
                    log.push_back(std::make_pair(Position(i,j), Position(lowest_position.first, lowest_position.second)));
                
            }
        }
    }
    
    return (std::make_tuple(new_feature_map, stackatop_count_map, log));
}