#include "tgaimage.h"

#include "Model.h"

#define FILEPATH "obj/african_head.obj"
#define FILEPATHWIRE "african_wireframe.tga"
#define FILEPATHSHADE "african_shaded.tga"
#define FILEPATHKELLY "obj/kelly_belly.obj"
#define FILEPATHKELLYWIRE "kelly_belly_wireframe.tga"
#define FILEPATHKELLYSHADER "kelly_belly_shaded.tga"
//#define OLD

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 2000;
const int height = 2000;



void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x) {
        std::swap(p0, p1);
    }

    for (int x = p0.x; x <= p1.x; x++) {
        float t = (x - p0.x) / (float)(p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t;
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
    //for steep image we want to transpose the image
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    // if for some reason is the coordinate from right to left, make it from left to right
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

Vec3f barycentric(Vec2i* pts, Vec2i P) {
    //Using barycentric coordinates we could determine the exact location of a point in relation to a triangle it is inside of
    Vec3f u = Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]) ^ Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]);
    if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}
void triangle(Vec2i* pts, TGAImage& image, TGAColor color) {

    //Find the bounding box, 0,0 is bottom left
    Vec2i bboxmin{ image.get_width() - 1, image.get_height() - 1 };
    Vec2i bboxmax{0, 0};
    Vec2i clamp{ image.get_width() - 1,image.get_height() - 1 };
    for (int i{ 0 }; i < 3; i++) {
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.x, pts[i].x));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }
    Vec2i P{};
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            Vec3f bc_screen = barycentric(pts, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            image.set(P.x, P.y, color);
        }
    }
}

int main(int argc, char** argv) {
#ifndef OLD
    model = new Model(FILEPATH);

    TGAImage image(width, height, TGAImage::RGB);
    //Wireframe
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file(FILEPATHWIRE);
    image.clear();
    
    //Shading
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f world_coords = model->vert(face[j]);
            screen_coords[j] = Vec2i((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);
        }
        triangle(screen_coords, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    }
    image.flip_vertically();
    image.write_tga_file(FILEPATHSHADE);
    delete model;

	
#endif
    TGAImage frame(200, 200, TGAImage::RGB);
    Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
    triangle(pts, frame, TGAColor(255, 0, 0,255));
    frame.flip_vertically(); // to place the origin in the bottom left corner of the image 
    frame.write_tga_file("framebuffer.tga");
    return 0;

	return 0;
}