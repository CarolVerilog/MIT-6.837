#include "material.h"
#include "ray.h"
#include "hit.h"
#include "glCanvas.h"
#include <GL/gl.h>

#ifdef SPECULAR_FIX
extern int SPECULAR_FIX_WHICH_PASS;
#endif

PhongMaterial::PhongMaterial(const Vec3f &d_color, const Vec3f &s_color, float e)
    : Material(d_color), specularColor(s_color), exponent(e)
{
}

Vec3f PhongMaterial::getSpecularColor() const
{
    return specularColor;
}

Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
    Vec3f diffuse_color = fmax(0.f, dirToLight.Dot3(hit.getNormal())) * diffuseColor * lightColor;

    Vec3f half = -1.f * ray.getDirection() + dirToLight;
    half.Normalize();
    Vec3f specular_color = powf(fmax(0.f, half.Dot3(hit.getNormal())), exponent) * specularColor * lightColor;

    return diffuse_color + specular_color;
}

void PhongMaterial::glSetMaterial(void) const
{
    GLfloat one[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat zero[4] = {0.0, 0.0, 0.0, 0.0};
    GLfloat specular[4] = {
        getSpecularColor().r(),
        getSpecularColor().g(),
        getSpecularColor().b(),
        1.0};
    GLfloat diffuse[4] = {
        getDiffuseColor().r(),
        getDiffuseColor().g(),
        getDiffuseColor().b(),
        1.0};

    // NOTE: GL uses the Blinn Torrance version of Phong...
    float glexponent = exponent;
    if (glexponent < 0)
        glexponent = 0;
    if (glexponent > 128)
        glexponent = 128;

#if !SPECULAR_FIX

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

    // OPTIONAL: 3 pass rendering to fix the specular highlight
    // artifact for small specular exponents (wide specular lobe)

    if (SPECULAR_FIX_WHICH_PASS == 0)
    {
        // First pass, draw only the specular highlights
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
    }
    else if (SPECULAR_FIX_WHICH_PASS == 1)
    {
        // Second pass, compute normal dot light
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }
    else
    {
        // Third pass, add ambient & diffuse terms
        assert(SPECULAR_FIX_WHICH_PASS == 2);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }

#endif
}