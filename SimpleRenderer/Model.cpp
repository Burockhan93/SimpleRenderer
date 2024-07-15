#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Model.h"

Model::Model(const char* filename) : verts_{}, faces_{}, norms_{}, uv_{}, diffusemap_{}, normalmap_{}, specularmap_{} {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;

    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        //compare first two characters of line with "v ", if matches this returns 0-meaining if(!statement) evaluates to 1= if this statment is true
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;//get rid of 'v '
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f n;
            for (int i = 0; i < 3; i++) iss >> n[i];
            norms_.push_back(n);
        }
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            uv_.push_back(uv);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;//trash the f
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) //put slashes into trash as well
            {
                for (int i = 0; i < 3; i++) tmp[i]--; //in obj files indices start at 1, we use 0 based thats why we subtract it by one
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap_);
   // load_texture(filename, "_nm.tga", normalmap_);
   // load_texture(filename, "_spec.tga", specularmap_);

}
Model::~Model() {
    //intentionally left blank.
}

int Model::nverts() {
    return static_cast<int>(verts_.size());
}

int Model::nfaces() {
    return static_cast<int>(faces_.size());
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i = 0; i < static_cast<int>(faces_[idx].size()); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::vert(int iface, int nthvertex) {
    return verts_[faces_[iface][nthvertex][0]];
}

void Model::load_texture(std::string filename, const char* suffix, TGAImage& image) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot != std::string::npos) {
        texfile = texfile.substr(0, dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (image.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        image.flip_vertically();
    }
}

Vec3f Model::norm(Vec2f uvf) {
    Vec2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
    TGAColor c = normalmap_.get(uv[0], uv[1]);
    Vec3f res;
    for (int i = 0; i < 3; i++)
        res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
    return res;
}

TGAColor Model::diffuse(Vec2f uvf) {
    Vec2i uv(uvf[0] * diffusemap_.get_width(), uvf[1] * diffusemap_.get_height());
    return diffusemap_.get(uv[0], uv[1]);
}

Vec2f Model::uv(int iface, int nthvertex) {
    return uv_[faces_[iface][nthvertex][1]];
}

float Model::specular(Vec2f uvf) {
    Vec2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
    return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}
Vec3f Model::norm(int iface, int nvert) {
    int idx = faces_[iface][nvert][2];
    return norms_[idx].normalize();
}