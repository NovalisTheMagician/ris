#include "graphics/Transform.hpp"

#include "misc/MathHelper.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace RIS
{
    namespace Graphics
    {
        Transform Combine(const Transform &a, const Transform &b)
        {
            Transform out;

            out.scale = a.scale * b.scale;
            out.rotation = b.rotation * a.rotation;

            out.position = a.rotation * (a.scale * b.position);
            out.position = a.position + out.position;

            return out;
        }

        Transform Inverse(const Transform &t)
        {
            Transform inv;
            inv.rotation = glm::inverse(t.rotation);

            inv.scale.x = std::abs(inv.scale.x) < glm::epsilon<float>() ? 0.0f : 1.0f / t.scale.x;
            inv.scale.y = std::abs(inv.scale.y) < glm::epsilon<float>() ? 0.0f : 1.0f / t.scale.y;
            inv.scale.z = std::abs(inv.scale.z) < glm::epsilon<float>() ? 0.0f : 1.0f / t.scale.z;

            glm::vec3 invTrans = t.position * -1.0f;
            inv.position = inv.rotation * (inv.scale * invTrans);

            return inv;
        }

        Transform Mix(const Transform &a, const Transform &b, float t)
        {
            glm::quat bRot = b.rotation;
            if(glm::dot(a.rotation, bRot) < 0.0f)
                bRot = -bRot;

            return Transform(
                    Lerp(a.position, b.position, t),
                    glm::normalize(glm::lerp(a.rotation, bRot, t)),
                    Lerp(a.scale, b.scale, t));
        }

        glm::mat4 TransformToMat4(const Transform &t)
        {
            glm::vec3 x = t.rotation * glm::vec3(1, 0, 0);
            glm::vec3 y = t.rotation * glm::vec3(0, 1, 0);
            glm::vec3 z = t.rotation * glm::vec3(0, 0, 1);

            x *= t.scale.x;
            y *= t.scale.y;
            z *= t.scale.z;

            glm::vec3 p = t.position;

            return glm::mat4(
                x.x, x.y, x.z, 0.0f,
                y.x, y.y, y.z, 0.0f,
                z.x, z.y, z.z, 0.0f,
                p.x, p.y, p.z, 1.0f
            );
        }

        Transform Mat4ToTransform(const glm::mat4 &m)
        {
            Transform out;

            out.position = glm::vec3(m[3].x, m[3].y, m[3].z);
            out.rotation = glm::quat_cast(m);

            glm::mat3 rotScaleMat(m);
            glm::mat4 invRotMat(glm::mat4_cast(glm::inverse(out.rotation)));
            glm::mat4 scaleSkewMat = glm::mat4(rotScaleMat) * invRotMat;

            out.scale = glm::vec3(scaleSkewMat[0].x, scaleSkewMat[1].y, scaleSkewMat[2].z);

            return out;
        }

        glm::vec3 TransformPoint(const Transform &t, const glm::vec3 &p)
        {
            glm::vec3 out;

            out = t.rotation * (t.scale * p);
            out = t.position + out;

            return out;
        }

        glm::vec3 TransformVector(const Transform &t, const glm::vec3 &v)
        {
            glm::vec3 out;

            out = t.rotation * (t.scale * v);

            return out;
        }
    }
}
