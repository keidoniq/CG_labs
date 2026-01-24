#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

class Quaternion {
private:
    float w, x, y, z;  // w + xi + yj + zk

public:
    Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
    
    static Quaternion fromAngleAxis(float angle, const glm::vec3& axis) {
        float halfAngle = angle * 0.5f;
        float sinHalf = sin(halfAngle);
        glm::vec3 normalizedAxis = glm::normalize(axis);
        
        return Quaternion(
            cos(halfAngle),
            normalizedAxis.x * sinHalf,
            normalizedAxis.y * sinHalf,
            normalizedAxis.z * sinHalf
        );
    }
    
    static Quaternion fromEulerAngles(float pitch, float yaw, float roll) {
        // pitch (X), yaw (Y), roll (Z)
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cp = cos(pitch * 0.5f);
        float sp = sin(pitch * 0.5f);
        float cr = cos(roll * 0.5f);
        float sr = sin(roll * 0.5f);
        
        return Quaternion(
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy
        );
    }
    
    Quaternion normalized() const {
        float length = sqrt(w*w + x*x + y*y + z*z);
        if (length < 1e-6f) return Quaternion(1, 0, 0, 0);
        return Quaternion(w/length, x/length, y/length, z/length);
    }
    
    void normalize() {
        float length = sqrt(w*w + x*x + y*y + z*z);
        if (length < 1e-6f) {
            w = 1; x = 0; y = 0; z = 0;
        } else {
            w /= length;
            x /= length;
            y /= length;
            z /= length;
        }
    }
    
    Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }
    
    Quaternion inverse() const {
        float norm = w*w + x*x + y*y + z*z;
        if (norm < 1e-6f) return Quaternion(1, 0, 0, 0);
        return Quaternion(w/norm, -x/norm, -y/norm, -z/norm);
    }
    
    Quaternion operator*(const Quaternion& q) const {
        return Quaternion(
            w*q.w - x*q.x - y*q.y - z*q.z,
            w*q.x + x*q.w + y*q.z - z*q.y,
            w*q.y - x*q.z + y*q.w + z*q.x,
            w*q.z + x*q.y - y*q.x + z*q.w
        );
    }
    
    Quaternion operator*(float s) const {
        return Quaternion(w*s, x*s, y*s, z*s);
    }
    
    Quaternion operator+(const Quaternion& q) const {
        return Quaternion(w+q.w, x+q.x, y+q.y, z+q.z);
    }
    
    glm::vec3 rotateVector(const glm::vec3& v) const {
        // q * v * q^-1
        Quaternion p(0, v.x, v.y, v.z);
        Quaternion result = (*this) * p * this->conjugate();
        return glm::vec3(result.x, result.y, result.z);
    }
    
    static Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t) {
        return Quaternion(
            q1.w + (q2.w - q1.w) * t,
            q1.x + (q2.x - q1.x) * t,
            q1.y + (q2.y - q1.y) * t,
            q1.z + (q2.z - q1.z) * t
        ).normalized();
    }
    
    static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
        float dot = q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;
        
        // Если dot отрицательный, инвертируем один из кватернионов
        Quaternion q2_adj = q2;
        if (dot < 0) {
            dot = -dot;
            q2_adj = q2 * -1.0f;
        }
        
        // Если кватернионы очень близки => LERP
        if (dot > 0.9995f) {
            return lerp(q1, q2_adj, t).normalized();
        }
        
        float theta_0 = acos(dot);
        float theta = theta_0 * t;
        float sin_theta = sin(theta);
        float sin_theta_0 = sin(theta_0);
        
        float s1 = cos(theta) - dot * sin_theta / sin_theta_0;
        float s2 = sin_theta / sin_theta_0;
        
        return (q1 * s1 + q2_adj * s2).normalized();
    }
    
    glm::mat4 toMatrix() const {
        Quaternion q = normalized();
        
        float xx = q.x * q.x;
        float xy = q.x * q.y;
        float xz = q.x * q.z;
        float xw = q.x * q.w;
        float yy = q.y * q.y;
        float yz = q.y * q.z;
        float yw = q.y * q.w;
        float zz = q.z * q.z;
        float zw = q.z * q.w;
        
        return glm::mat4(
            glm::vec4(1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw), 0),
            glm::vec4(2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw), 0),
            glm::vec4(2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy), 0),
            glm::vec4(0, 0, 0, 1)
        );
    }
    
    // Получение углов Эйлера
    glm::vec3 toEulerAngles() const {
        Quaternion q = this->normalized();
        
        // pitch (x-axis rotation)
        float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
        float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
        float pitch = atan2(sinr_cosp, cosr_cosp);
        
        // yaw (y-axis rotation)
        float sinp = 2.0f * (q.w * q.y - q.z * q.x);
        if (fabs(sinp) >= 1.0f)
            sinp = copysign(1.0f, sinp); 
        else
            sinp = asin(sinp);
        
        // roll (z-axis rotation)
        float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
        float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
        float roll = atan2(siny_cosp, cosy_cosp);
        
        return glm::vec3(pitch, sinp, roll);
    }

    static Quaternion fromRotationMatrix(const glm::mat4& m) {
        glm::mat3 rotMatrix = glm::mat3(m);
        
        float trace = rotMatrix[0][0] + rotMatrix[1][1] + rotMatrix[2][2];
        if (trace > 0) {
            float s = 0.5f / sqrtf(trace + 1.0f);
            return Quaternion(
                0.25f / s,
                (rotMatrix[2][1] - rotMatrix[1][2]) * s,
                (rotMatrix[0][2] - rotMatrix[2][0]) * s,
                (rotMatrix[1][0] - rotMatrix[0][1]) * s
            );
        } else {
            if (rotMatrix[0][0] > rotMatrix[1][1] && rotMatrix[0][0] > rotMatrix[2][2]) {
                float s = 2.0f * sqrtf(1.0f + rotMatrix[0][0] - rotMatrix[1][1] - rotMatrix[2][2]);
                return Quaternion(
                    (rotMatrix[2][1] - rotMatrix[1][2]) / s,
                    0.25f * s,
                    (rotMatrix[0][1] + rotMatrix[1][0]) / s,
                    (rotMatrix[0][2] + rotMatrix[2][0]) / s
                );
            } else if (rotMatrix[1][1] > rotMatrix[2][2]) {
                float s = 2.0f * sqrtf(1.0f + rotMatrix[1][1] - rotMatrix[0][0] - rotMatrix[2][2]);
                return Quaternion(
                    (rotMatrix[0][2] - rotMatrix[2][0]) / s,
                    (rotMatrix[0][1] + rotMatrix[1][0]) / s,
                    0.25f * s,
                    (rotMatrix[1][2] + rotMatrix[2][1]) / s
                );
            } else {
                float s = 2.0f * sqrtf(1.0f + rotMatrix[2][2] - rotMatrix[0][0] - rotMatrix[1][1]);
                return Quaternion(
                    (rotMatrix[1][0] - rotMatrix[0][1]) / s,
                    (rotMatrix[0][2] + rotMatrix[2][0]) / s,
                    (rotMatrix[1][2] + rotMatrix[2][1]) / s,
                    0.25f * s
                );
            }
        }
        return Quaternion::fromEulerAngles(0.f, 0.f, 0.f);
    }
    
    float getW() const { return w; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    
    void toAngleAxis(float& angle, glm::vec3& axis) const {
        Quaternion q = this->normalized();
        angle = 2.0f * acos(q.w);
        float s = sqrt(1.0f - q.w * q.w);
        
        if (s < 0.001f) {
            axis = glm::vec3(1.0f, 0.0f, 0.0f);
        } else {
            axis = glm::vec3(q.x / s, q.y / s, q.z / s);
        }
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
        os << "(" << q.w << ", " << q.x << "i, " << q.y << "j, " << q.z << "k)";
        return os;
    }
};