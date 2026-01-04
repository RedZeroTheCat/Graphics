#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iostream>
#include <vector>


// Константы для сцены
const int WIDTH = 1800; // Ширина изображения
const int HEIGHT = 1200; // Высота изображения
const int MAX_DEPTH = 10; // Глубина трассировки
const float EPSILON = 0.001f; // Маленькое значение для избежания самопересечений

using namespace glm;

// Структуры данных
struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 color;
    float reflectivity; // Коэффициент отражения
    float refractivity; // Коэффициент преломления (0 = нет преломления)
    float ior;          // Относительный показатель преломления
};

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

struct Plane {
    vec3 point;   // Точка на плоскости
    vec3 normal;  // Нормаль к плоскости
    Material material;
};

// Функции пересечения
bool intersectSphere(const Ray& ray, const Sphere& sphere, float& t, vec3& hitNormal) {
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
        float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
        t = (t0 > EPSILON) ? t0 : t1;
        if (t > EPSILON) {
            hitNormal = normalize(ray.origin + t * ray.direction - sphere.center);
            return true;
        }
    }
    return false;
}

bool intersectPlane(const Ray& ray, const Plane& plane, float& t, vec3& hitNormal) {
    float denom = dot(ray.direction, plane.normal);
    if (abs(denom) > EPSILON) {
        t = dot(plane.point - ray.origin, plane.normal) / denom;
        if (t > EPSILON) {
            hitNormal = plane.normal;
            return true;
        }
    }
    return false;
}

// Трассировка луча
vec3 traceRay(const Ray& ray, const std::vector<Sphere>& spheres, const Plane& plane, int depth) {
    if (depth <= 0) {
        return vec3(0.0f); // Базовый случай: черный цвет
    }

    vec3 color(0.0f);
    float nearestT = INFINITY;
    vec3 hitNormal;
    Material hitMaterial;

    // Проверка пересечения со сферами
    for (const auto& sphere : spheres) {
        float t;
        vec3 normal;
        if (intersectSphere(ray, sphere, t, normal) && t < nearestT) {
            nearestT = t;
            hitNormal = normal;
            hitMaterial = sphere.material;
        }
    }

    // Проверка пересечения с плоскостью
    float t;
    vec3 normal;
    if (intersectPlane(ray, plane, t, normal) && t < nearestT) {
        nearestT = t;
        hitNormal = normal;
        hitMaterial = plane.material;
    }

    if (nearestT < INFINITY) {
        // Точка пересечения
        vec3 hitPoint = ray.origin + nearestT * ray.direction;

        // Отражение
        vec3 reflectionColor(0.0f);
        if (hitMaterial.reflectivity > 0.0f) {
            vec3 reflectedDir = reflect(ray.direction, hitNormal);
            Ray reflectedRay = { hitPoint + EPSILON * reflectedDir, reflectedDir };
            reflectionColor = traceRay(reflectedRay, spheres, plane, depth - 1);
        }

        // Преломление
        vec3 refractionColor(0.0f);
        if (hitMaterial.refractivity > 0.0f) {
            vec3 refractedDir = refract(ray.direction, hitNormal, hitMaterial.ior);
            Ray refractedRay = { hitPoint + EPSILON * refractedDir, refractedDir };
            refractionColor = traceRay(refractedRay, spheres, plane, depth - 1);
        }

        // Итоговый цвет
        color = hitMaterial.color * (1.0f - hitMaterial.reflectivity - hitMaterial.refractivity)
                + reflectionColor * hitMaterial.reflectivity
                + refractionColor * hitMaterial.refractivity;
    }

    return color;
}

// Основная функция
int main() {
    std::vector<Sphere> spheres = {
            { vec3(-3.5f, 0.5f, -7.0f), 0.8f, { vec3(1.0f, 0.0f, 0.0f), 0.5f, 0.0f, 1.0f } },
            { vec3(0.0f, 0.5f, -5.0f), 0.9f, { vec3(0.0f, 1.0f, 0.0f), 0.5f, 0.0f, 1.0f } },
            { vec3(2.5f, 0.0f, -6.0f), 1.0f, { vec3(0.0f, 0.0f, 1.0f), 0.5f, 0.0f, 0.5f } }
    };

    Plane plane = { vec3(0.0f, -2.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), { vec3(0.7f), 1.0f, 0.5f, 0.9f } };

    std::vector<unsigned char> image(WIDTH * HEIGHT * 3);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            float u = (x + 0.5f) / WIDTH;
            float v = (y + 0.5f) / HEIGHT;
            vec3 rayDir = normalize(vec3(u * 2.0f - 1.0f, v * 2.0f - 1.0f, -1.0f));
            Ray ray = { vec3(0.0f), rayDir };

            vec3 color = traceRay(ray, spheres, plane, MAX_DEPTH);
            image[(y * WIDTH + x) * 3 + 0] = (unsigned char)(glm::clamp(color.r, 0.0f, 1.0f) * 255);
            image[(y * WIDTH + x) * 3 + 1] = (unsigned char)(glm::clamp(color.g, 0.0f, 1.0f) * 255);
            image[(y * WIDTH + x) * 3 + 2] = (unsigned char)(glm::clamp(color.b, 0.0f, 1.0f) * 255);
        }
    }

    stbi_write_png("../output.png", WIDTH, HEIGHT, 3, image.data(), WIDTH * 3);

    std::cout << "Image saved as output.png" << std::endl;
    return 0;
}
