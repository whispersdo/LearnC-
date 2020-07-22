//#include"Raytracing.h"
#define _CRT_SECURE_NO_WARNINGS
#include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2008
#include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt
#include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2
#include <time.h>		// MILO
#define M_PI 3.141592653589793238462643	// MILO
#define USE_LCG
#ifdef USE_LCG
struct RandomLCG {
    unsigned mSeed;
    RandomLCG(unsigned seed = 0) : mSeed(seed) {}
    __forceinline double operator()() { mSeed = 214013 * mSeed + 2531011; return mSeed * (1.0 / 4294967296); }
};
#define RANDOM(Xi) Xi()
#define RANDOM_INIT(Xi) RandomLCG Xi;
#define RANDOM_PARAM(Xi) RandomLCG& Xi
#else
#include "erand48.inc"	// MILO
#define RANDOM(Xi) erand48(Xi)
#define RANDOM_INIT(Xi) unsigned short Xi[3]={0,0,y*y*y}
#define RANDOM_PARAM(Xi) unsigned short *Xi
#endif
struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
    double x, y, z;                  // position, also color (r,g,b)
    __forceinline Vec(double x_ = 0, double y_ = 0, double z_ = 0) { x = x_; y = y_; z = z_; }
    __forceinline Vec operator+(const Vec& b) const { return Vec(x + b.x, y + b.y, z + b.z); }
    __forceinline Vec operator-(const Vec& b) const { return Vec(x - b.x, y - b.y, z - b.z); }
    __forceinline Vec operator*(double b) const { return Vec(x * b, y * b, z * b); }
    __forceinline Vec mult(const Vec& b) const { return Vec(x * b.x, y * b.y, z * b.z); }
    __forceinline Vec& norm() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }
    __forceinline double dot(const Vec& b) const { return x * b.x + y * b.y + z * b.z; } // cross:
    __forceinline Vec operator%(const Vec& b) { return Vec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};
struct Ray { Vec o, d; Ray(const Vec& o_, const Vec& d_) : o(o_), d(d_) {} };
enum Refl_t { DIFF, SPEC, REFR };  // material types, used in radiance()
struct Sphere {
    double rad;       // radius
    double sqRad; // MILO
    Vec p, e, c;      // position, emission, color
    Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)
    Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_) :
        rad(rad_), p(p_), e(e_), c(c_), refl(refl_), sqRad(rad_* rad_) {} // MILO
    __forceinline double intersect(const Ray& r) const { // returns distance, 0 if nohit
        Vec op = p - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        double t, eps = 1e-4, b = op.dot(r.d), det = b * b - op.dot(op) + sqRad; // MILO
        if (det < 0) return 0; else det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }
};
Sphere spheres[] = {//Scene: radius, position, emission, color, material
  Sphere(1e5, Vec(1e5 + 1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left
  Sphere(1e5, Vec(-1e5 + 99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght
  Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back
  Sphere(1e5, Vec(50,40.8,-1e5 + 170), Vec(),Vec(),           DIFF),//Frnt
  Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm
  Sphere(1e5, Vec(50,-1e5 + 81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top
  Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1) * .999, SPEC),//Mirr
  Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1) * .999, REFR),//Glas
  Sphere(600, Vec(50,681.6 - .27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite
};
__forceinline double clamp(double x) { return x < 0 ? 0 : x>1 ? 1 : x; }
__forceinline int toInt(double x) { return int(pow(clamp(x), 1 / 2.2) * 255 + .5); }
__forceinline bool intersect(const Ray& r, double& t, int& id) {
    double n = sizeof(spheres) / sizeof(Sphere), d, inf = t = 1e20;
    for (int i = int(n); i--;) if ((d = spheres[i].intersect(r)) && d < t) { t = d; id = i; }
    return t < inf;
}
Vec radiance(const Ray& r, int depth, RANDOM_PARAM(Xi)) {
    double t;                               // distance to intersection
    int id = 0;                               // id of intersected object

    if (!intersect(r, t, id)) return Vec(); // if miss, return black

    const Sphere& obj = spheres[id];        // the hit object

    Vec x = r.o + r.d * t, n = (x - obj.p).norm(), nl = n.dot(r.d) < 0 ? n : n * -1, f = obj.c;

    double p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl

    if (++depth > 5) 
        if (RANDOM(Xi) < p) 
            f = f * (1 / p); 
        else 
            return obj.e; //R.R.

    if (depth > 100) return obj.e; // MILO

    if (obj.refl == DIFF) {                  // Ideal DIFFUSE reflection
        double r1 = 2 * M_PI * RANDOM(Xi), r2 = RANDOM(Xi), r2s = sqrt(r2);
        Vec w = nl, u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).norm(), v = w % u;
        Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
        return obj.e + f.mult(radiance(Ray(x, d), depth, Xi));
    }
    else if (obj.refl == SPEC)            // Ideal SPECULAR reflection
        return obj.e + f.mult(radiance(Ray(x, r.d - n * 2 * n.dot(r.d)), depth, Xi));
    Ray reflRay(x, r.d - n * 2 * n.dot(r.d));     // Ideal dielectric REFRACTION
    bool into = n.dot(nl) > 0;                // Ray from outside going in?
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.d.dot(nl), cos2t;
    if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0)    // Total internal reflection
        return obj.e + f.mult(radiance(reflRay, depth, Xi));
    Vec tdir = (r.d * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
    return obj.e + f.mult(depth > 2 ? (RANDOM(Xi) < P ?   // Russian roulette
        radiance(reflRay, depth, Xi) * RP : radiance(Ray(x, tdir), depth, Xi) * TP) :
        radiance(reflRay, depth, Xi) * Re + radiance(Ray(x, tdir), depth, Xi) * Tr);
}

Vec* c = nullptr;

void test_main(int argc, char* argv[]) {
    clock_t start = clock(); // MILO
    int w = 256, h = 256, samps = argc == 2 ? atoi(argv[1]) / 4 : 25; // # samples
    Ray cam(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm()); // cam pos, dir
    Vec cx = Vec(w * .5135 / h), cy = (cx % cam.d).norm() * .5135, r;
    c = new Vec[w * h];
#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP
    for (int y = 0; y < h; y++) {                       // Loop over image rows
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100. * y / (h - 1));
        RANDOM_INIT(Xi);// MILO
        for (unsigned short x = 0; x < w; x++)   // Loop cols
            for (int sy = 0, i = (h - y - 1) * w + x; sy < 2; sy++)     // 2x2 subpixel rows
                for (int sx = 0; sx < 2; sx++, r = Vec()) {        // 2x2 subpixel cols
                    for (int s = 0; s < samps; s++) {
                        double r1 = 2 * RANDOM(Xi), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                        double r2 = 2 * RANDOM(Xi), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                        Vec d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
                            cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.d;
                        r = r + radiance(Ray(cam.o + d * 140, d.norm()), 0, Xi) * (1. / samps);
                    } // Camera rays are pushed ^^^^^ forward to start in interior
                    c[i] = c[i] + Vec(clamp(r.x), clamp(r.y), clamp(r.z)) * .25;
                }
    }
    printf("\n%f sec\n", (float)(clock() - start) / CLOCKS_PER_SEC); // MILO
    FILE* f = fopen("image.ppm", "w");         // Write image to PPM file.
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for (int i = 0; i < w * h; i++)
        fprintf(f, "%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));
}