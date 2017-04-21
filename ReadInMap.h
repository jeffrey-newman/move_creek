#ifndef READ_IN_MAP
#define READ_IN_MAP

#include <boost/filesystem.hpp>
#include <exception>
#include "MoveCreekTypes.h"

namespace fs = boost::filesystem;

/*! Pixel data types */
typedef enum {
    /*! Unknown or unspecified type */ 		    GDT_Unknown_ = 0,
    /*! Eight bit unsigned integer */           GDT_Byte_ = 1,
    /*! Sixteen bit unsigned integer */         GDT_UInt16_ = 2,
    /*! Sixteen bit signed integer */           GDT_Int16_ = 3,
    /*! Thirty two bit unsigned integer */      GDT_UInt32_ = 4,
    /*! Thirty two bit signed integer */        GDT_Int32_ = 5,
    /*! Thirty two bit floating point */        GDT_Float32_ = 6,
    /*! Sixty four bit floating point */        GDT_Float64_ = 7,
    /*! Complex Int16 */                        GDT_CInt16_ = 8,
    /*! Complex Int32 */                        GDT_CInt32_ = 9,
    /*! Complex Float32 */                      GDT_CFloat32_ = 10,
    /*! Complex Float64 */                      GDT_CFloat64_ = 11,
    GDT_TypeCount_ = 12          /* maximum type # + 1 */
} GDALDataType_;

struct GeoTransform {
    double x_origin;
    double pixel_width;
    double x_line_space;
    double y_origin;
    double pixel_height;
    double y_line_space;
};

template <typename DataFormat>
std::tuple<boost::shared_ptr<Map_Matrix<DataFormat> >, std::string, GeoTransform>  read_in_map (fs::path file_path, GDALDataType data_type) throw(std::runtime_error);

template <typename DataFormat>
void write_map(fs::path file_path, GDALDataType data_type, boost::shared_ptr<Map_Matrix<DataFormat> > data, std::string WKTprojection, GeoTransform transform, std::string driverName) throw(std::runtime_error);

#endif // !READ_IN_MAP