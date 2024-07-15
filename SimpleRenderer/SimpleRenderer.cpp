#include <cmath>
#include <cstdlib>
#include <iostream>

#include "geometry.h"
#include "tgaimage.h"
#include "Model.h"
#include "simpleGL.h"

#define FILEPATH "obj/african_head.obj"
#define FILEPATHWIRE "african_wireframe.tga"
#define FILEPATHSHADE "african_shaded.tga"
#define FILEPATHSHADELIGHT "african_shaded_light.tga"
#define FILEPATHKELLY "obj/kelly_belly.obj"
#define FILEPATHKELLYWIRE "kelly_belly_wireframe.tga"
#define FILEPATHKELLYSHADER "kelly_belly_shaded.tga"

//Colors
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

//Scene
Model* model = nullptr;
const int width = 800;
const int height = 800;

Vec3f lightDir = Vec3f{ 1,-1,1 }.normalize(); 
Vec3f camera{ 1,1,3 };
Vec3f center{}; //Set center to zero
Vec3f up{ 0,1,0 };



struct Shader : public IShader {
    Vec3f varying_intensiyt;
    mat<2, 3, float> varying_uv;
    mat<4, 4, float> ProjectionModelView;
    mat<4, 4, float> ProjectionModelViewTranspose;


    virtual Vec4f vertex(int iface, int nthvert) {
        varying_intensiyt[nthvert] = std::max(.0f, model->norm(iface, nthvert) * lightDir);
        varying_uv.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
        return Viewport * Projection * ModelView * gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        float intensity = varying_intensiyt * bar;
        Vec2f uv = varying_uv * bar;   
        Vec3f n = proj<3>(ProjectionModelView * embed<4>(model->norm(uv))).normalize();
        Vec3f l = proj<3>(ProjectionModelView * embed<4>(lightDir)).normalize();
        Vec3f r = (n * (n * l * 2.f) - l).normalize(); //Phong shading reflection light
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        float diff = std::max(0.f, n * l);
        TGAColor c = model->diffuse(uv);
        color = c;
        for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * (diff + .6 * spec), 255);
        for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * (diff + 1 * spec), 255);
        return false;
    }


};

struct GouraudShader : public IShader {
    Vec3f varying_intensity; 

    //We have two shader per usual. Vertex and Color
    virtual Vec4f vertex(int iface, int nthvert) {
        //get a lightning value for eac vertex as well
        varying_intensity[nthvert] = std::max(0.f, model->norm(iface, nthvert) * lightDir);
        //we need 4*4 matrice for mvp computation, so we draw the vertice from model
        //add the last one as 1
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); 
        //after that we project it to screen
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;          
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        float intensity = varying_intensity * bar;
        if (intensity > .75f) intensity = .9f;
        else if (intensity > .50f) intensity = .8f;
        else if (intensity > .25f) intensity = .3f;
        color = TGAColor(100, 0, 255) * intensity; 
        return false; 
    }
};


int main(int argc, char** argv) {
    model = new Model(FILEPATH);
    
    lookat(camera, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (camera - center).norm());
    lightDir.normalize();

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    Shader shader;
    shader.ProjectionModelView = Projection * ModelView;
    shader.ProjectionModelViewTranspose = shader.ProjectionModelView.invert_transpose();
    for (int i = 0; i < model->nfaces(); i++) {
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangulate(screen_coords, shader, image, zbuffer);
    }

    image.flip_vertically(); // to place the origin in the bottom left corner of the image
    zbuffer.flip_vertically();
    image.write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");
   
    
    delete model;
   
	return 0;
}