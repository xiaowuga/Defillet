//
// Created by 13900K on 2024/3/19.
//

#include <defillet.h>


#include <CLI/CLI.hpp>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/core/point_cloud.h>
#include <easy3d/fileio/surface_mesh_io.h>
#include <easy3d/fileio/point_cloud_io.h>
#include <easy3d/util/file_system.h>
#include <igl/jet.h>

#include <nlohmann/json.hpp>


//#dkm <easy3d/viewer/viewer.h"
//#dkm "easy3d/renderer/renderer.h"
//#dkm "easy3d/renderer/drawable_points.h"
//#dkm "easy3d/renderer/drawable_triangles.h"
//#dkm "easy3d/renderer/drawable_lines.h"
//#dkm "easy3d/renderer/texture_manager.h"

using json = nlohmann::json;

int main(int argc, char **argv) {
    struct {
        std::string input_mesh;
        std::string output_dir;
        double angle;
    } args;
    CLI::App app{"DEFILLET Command Line"};

    app.add_option("-i,--input_mesh", args.input_mesh, "Input Mesh")->required();
    app.add_option("-o,--output_dir", args.output_dir, "Output Directory")->required();
    app.add_option("--angle", args.angle, "Angle")->default_val(45);
    CLI11_PARSE(app, argc, argv);
    easy3d::SurfaceMesh* mesh = easy3d::SurfaceMeshIO::load(args.input_mesh);
    bool gcp_labels = false;
    for(auto p : mesh->face_properties()) {
        if(p == "f:gcp_labels") {
            gcp_labels = true;
        }
    }
    if(!gcp_labels) {
        std::cout << "Please gcp_label first." << std::endl;
        return 0;
    }
    DeFillet defillet;
    defillet.set_mesh(mesh);
    defillet.set_angle(args.angle);
    defillet.run_geodesic();
    easy3d::SurfaceMesh* fillet_mesh = defillet.get_fillet_mesh();
    std::string out_fillet_geo_path = args.output_dir +  "fillet_geo.ply";
    easy3d::io::save_ply(out_fillet_geo_path, fillet_mesh, true);


    std::string info_path = args.output_dir +  "geo_info.json";
    json info;
    info["input_mesh"] = args.input_mesh;
    info["output_dir"] = args.output_dir;
    info["angle"] = args.angle;
    info["geo_time"] = defillet.get_geodesic_time();
    info["target_refine_time"] = defillet.get_target_normals_refine_time();
    std::ofstream file(info_path.c_str());
    file << std::setw(4) << info << std::endl;


    return 0;
}