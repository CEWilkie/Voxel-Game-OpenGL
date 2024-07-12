//
// Created by cew05 on 11/07/2024.
//

#ifndef UNTITLED7_MODELSTRUCTS_H
#define UNTITLED7_MODELSTRUCTS_H

#include <iostream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Plane {
    glm::vec3 normal {};
    float distance {};

    void SetPlane(const glm::vec3& _pos, const glm::vec3& _normal) {
        normal = glm::normalize(_normal);
        distance = glm::dot(normal, _pos);
    }
};

struct Frustrum {
    // INDEX SIDE : |0 LEFT | 1 RIGHT | 2 BOTTOM | 3 TOP | 4 NEAR | 5 FAR
    Plane planes[6] {};
};

struct Vertex {
    glm::vec3 position {};
    glm::vec2 textureIndex {};
    glm::vec2 texture {};
    glm::vec3 color {};
};

struct BoundingVolume {
    enum FRUSTRUM : int{
        OUTSIDE, INTERSECTING, IN,
    };

    [[nodiscard]] virtual int InFrustrum(const Frustrum& _frustrum, const glm::mat4& _projection) const {
        return IN;
    }
};

struct SphereBounds : public BoundingVolume {
    glm::vec3 centre {0.0f, 0.0f, 0.0f};
    float radius {0.1f};

    [[nodiscard]] int InFrustrum(const Frustrum& _camFrustrum, const glm::mat4& _projection) const override {

        // Calculate distance to each plane
        float dist;
        for (const auto& plane : _camFrustrum.planes) {
            dist = glm::dot(glm::normalize(plane.normal), centre) + plane.distance;

            // OUTSIDE FRUSTRUM
            if (dist < -radius) return OUTSIDE;

            // INTERSECTING FRUSTRUM
            if (abs(dist) < radius) return INTERSECTING;
        }

        return IN;
    }
};

struct BoxBounds : public BoundingVolume {
    glm::vec3 vertexArray[8] {};

    BoxBounds(glm::vec3 _centre, glm::vec3 _dimensions) : BoundingVolume() {
        CreateVertexArray(_centre, _dimensions);
    }

    void CreateVertexArray(glm::vec3 _centre, glm::vec3 _dimensions) {
        // Bottom of BoundingBox
        vertexArray[0] = _centre + glm::vec3(_dimensions.x/2, -_dimensions.y/2, _dimensions.z/2);
        vertexArray[1] = _centre + glm::vec3(_dimensions.x/2, -_dimensions.y/2, -_dimensions.z/2);
        vertexArray[1] = _centre + glm::vec3(-_dimensions.x/2, -_dimensions.y/2, _dimensions.z/2);
        vertexArray[1] = _centre + glm::vec3(-_dimensions.x/2, -_dimensions.y/2, -_dimensions.z/2);

        // Top of BoundingBox
        vertexArray[1] = _centre + glm::vec3(_dimensions.x/2, _dimensions.y/2, _dimensions.z/2);
        vertexArray[1] = _centre + glm::vec3(_dimensions.x/2, _dimensions.y/2, -_dimensions.z/2);
        vertexArray[1] = _centre + glm::vec3(-_dimensions.x/2, _dimensions.y/2, _dimensions.z/2);
        vertexArray[1] = _centre + glm::vec3(-_dimensions.x/2, _dimensions.y/2, -_dimensions.z/2);

//        printf("BBV: %f %f %f\n", vertexArray[0].x, vertexArray[0].y, vertexArray[0].z);
    }

    [[nodiscard]] int InFrustrum(const Frustrum& _frustrum, const glm::mat4& _projection) const override {
        int totalIn = 0;

        for (const auto& plane : _frustrum.planes) {

            int inCount = 8;
            int allIn = 1;
            for (const auto& vertex : vertexArray) {
                if (glm::dot(vertex, glm::normalize(plane.normal)) + plane.distance < 0) {
                    allIn = 0;
                    inCount--;
                }
            }

            // all points outside plane, so out
            if (inCount == 0) return OUTSIDE;

            totalIn += allIn;
        }

        // ALL VERTEXES INSIDE FRUSTRUM
        if (totalIn == 6) return IN;

        // ELSE ASSUME PARTLY INTERSECTING
        return INTERSECTING;
    }
};

#endif //UNTITLED7_MODELSTRUCTS_H
