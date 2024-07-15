#include <cmath>
#include <cstdlib>
#include <iostream>
#include "geometry.h"
#include "tgaimage.h"
#include "Model.h"
#include "simpleGL.h"
#include "Constants.h"

//Scene
Model* model = nullptr;
float* shadowbuffer = nullptr;

const int width = 800;
const int height = 800;

Vec3f lightDir = Vec3f{ 1,1,0 }.normalize(); 
Vec3f camera{ 1,1,4 };
Vec3f center{}; //Set center to zero
Vec3f up{ 0,1,0 };



struct Shader : public IShader {

    Vec3f varying_intensiyt;
    mat<4, 4, float> ScreenToShadow;
    mat<2, 3, float> UVCoordinates;
    mat<3, 3, float> TriangleCoordinatesInWorld;
    mat<4, 4, float> ProjectionModelView;
    mat<4, 4, float> ProjectionModelViewTranspose;

    Shader(Matrix M, Matrix MIT, Matrix MS) : ProjectionModelView(M), ProjectionModelViewTranspose(MIT), ScreenToShadow(MS),UVCoordinates(),TriangleCoordinatesInWorld(){}


    virtual Vec4f vertex(int iface, int nthvert) {
        UVCoordinates.set_col(nthvert, model->uv(iface, nthvert));
        Vec4f gl_Vertex = Viewport * Projection * ModelView * embed<4>(model->vert(iface, nthvert));
        TriangleCoordinatesInWorld.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3])); //for casting shadows later
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec4f sb_p = ScreenToShadow * embed<4>(TriangleCoordinatesInWorld * bar); // corresponding point in the shadow buffer
        sb_p = sb_p / sb_p[3];
        int idx = int(sb_p[0]) + int(sb_p[1]) * width; // index in the shadowbuffer array
        float shadow = .3 + .7 * (shadowbuffer[idx] < sb_p[2]+40); // magic coeff to avoid z-fighting
        Vec2f uv = UVCoordinates * bar;                 // interpolate uv for the current pixel
        Vec3f n = proj<3>(ProjectionModelViewTranspose * embed<4>(model->norm(uv))).normalize(); // normal
        Vec3f l = proj<3>(ProjectionModelView * embed<4>(lightDir)).normalize(); // light vector
        Vec3f r = (n * (n * l * 2.f) - l).normalize();   // reflected light
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        float diff = std::max(0.f, n * l);
        TGAColor c = model->diffuse(uv);
        for (int i = 0; i < 3; i++) color[i] = std::min<float>(20 + c[i] * shadow * (1.2 * diff + .6 * spec), 255);
        return false;
    }


};

struct DepthShader : public IShader {
    mat<3, 3, float> varying_tri;

    DepthShader() : varying_tri() {}

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
        gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;          // transform it to screen coordinates
        varying_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec3f p = varying_tri * bar;
        color = TGAColor(255, 255, 255) * (p.z / depth);
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
    
    float* zBuffer = new float[width * height];
    shadowbuffer = new float[width * height];
    for (int i = width * height; --i; ) {
        zBuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();
    }

    //Shadow
    {
        TGAImage depth(width, height, TGAImage::RGB);
        lookat(lightDir, center, up);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        projection(0);

        DepthShader depthshader;
        Vec4f screen_coords[3];
        for (int i = 0; i < model->nfaces(); i++) {
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = depthshader.vertex(i, j);
            }
            triangulate(screen_coords, depthshader, depth, shadowbuffer);
        }
        depth.flip_vertically(); // to place the origin in the bottom left corner of the image
        depth.write_tga_file("depth.tga");
    }

    //FRame
    Matrix M = Viewport * Projection * ModelView;
    {
        TGAImage frame(width, height, TGAImage::RGB);
        lookat(camera, center, up);
        viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
        projection(-1.f / (camera - center).norm());

        Shader shader(ModelView, (Projection * ModelView).invert_transpose(), M * (Viewport * Projection * ModelView).invert());
        Vec4f screen_coords[3];
        for (int i = 0; i < model->nfaces(); i++) {
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangulate(screen_coords, shader, frame, zBuffer);
        }
        frame.flip_vertically(); // to place the origin in the bottom left corner of the image
        frame.write_tga_file("framebuffer.tga");
    }
   
   
    
    delete model;
    delete[] zBuffer;
    delete[] shadowbuffer;
   
	return 0;
}