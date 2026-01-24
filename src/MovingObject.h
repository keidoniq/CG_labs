#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Quaternion.h"
#include "AffineTransform3D.h"

class MovingObject {
protected:  
    Quaternion orientation;
    Quaternion default_orientation;
public:
    MovingObject(Quaternion orientation): orientation(orientation) {
        default_orientation = orientation;
    }
    MovingObject(glm::vec3 O, glm::vec3 T, glm::vec3 N){
        glm::vec3 forward = glm::normalize(N - O);
        glm::vec3 right = glm::normalize(glm::cross(forward, T));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));
        glm::mat4 viewMatrix = glm::lookAt(O, N, up);
        orientation = Quaternion::fromRotationMatrix(viewMatrix);
        default_orientation = orientation;
    }
    Quaternion getOrientation() const { return orientation; }
    void resetOrientation(){
        orientation = default_orientation;
    }
    glm::vec3 getForward() const{
        glm::vec3 localForward(0.0f, 0.0f, -1.0f);
        return glm::normalize(orientation.rotateVector(localForward));
    }
    glm::vec3 getUp() const {
        glm::vec3 localUp(0.0f, 1.0f, 0.0f);
        return glm::normalize(orientation.rotateVector(localUp));
    }
    glm::vec3 getRight() const {
        glm::vec3 localRight(1.0f, 0.0f, 0.0f);
        return glm::normalize(orientation.rotateVector(localRight));
    }

    void rotateAroundAxis(float angle, const glm::vec3& axis) {
        Quaternion delta = Quaternion::fromAngleAxis(angle, axis);
        orientation = delta * orientation;
        orientation.normalize();
    }
    void yaw(float angle) { rotateAroundAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f)); }
    void pitch(float angle) { rotateAroundAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f)); }
    void roll(float angle) { rotateAroundAxis(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }
      
    void rotateAroundLocalAxis(float angle, Axis localAxis){
        glm::vec3 rotateVector;
        switch (localAxis){
            case X: rotateVector = getRight(); break;
            case Y: rotateVector = getUp(); break;
            case Z: rotateVector = getForward(); break;
        }
    }
};