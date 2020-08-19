#include "graphics/Animation.hpp"

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            TransformTrack::TransformTrack()
                : id(0)
            {}

            unsigned int TransformTrack::GetId() const
            {
                return id;
            }

            void TransformTrack::SetId(unsigned int id)
            {
                this->id = id;
            }

            VectorTrack& TransformTrack::GetPositionTrack()
            {
                return positionTrack;
            }

            QuaternionTrack& TransformTrack::GetRotationTrack()
            {
                return rotationTrack;
            }

            VectorTrack& TransformTrack::GetScaleTrack()
            {
                return scaleTrack;
            }

            const VectorTrack& TransformTrack::GetPositionTrack() const
            {
                return positionTrack;
            }

            const QuaternionTrack& TransformTrack::GetRotationTrack() const
            {
                return rotationTrack;
            }

            const VectorTrack& TransformTrack::GetScaleTrack() const
            {
                return scaleTrack;
            }

            bool TransformTrack::IsValid() const
            {
                return positionTrack.Size() > 1 || rotationTrack.Size() > 1 || scaleTrack.Size() > 1;
            }

            float TransformTrack::GetStartTime() const
            {
                float result = 0.0f;
                bool isSet = false;

                if(positionTrack.Size() > 1)
                {
                    result = positionTrack.GetStartTime();
                    isSet = true;
                }
                if(rotationTrack.Size() > 1)
                {
                    float rotationStart = rotationTrack.GetStartTime();
                    if(rotationStart < result || !isSet)
                    {
                        result = rotationStart;
                        isSet = true;
                    }
                }
                if(scaleTrack.Size() > 1)
                {
                    float scaleStart = scaleTrack.GetStartTime();
                    if(scaleStart < result || !isSet)
                    {
                        result = scaleStart;
                        isSet = true;
                    }
                }
                return result;
            }

            float TransformTrack::GetEndTime() const
            {
                float result = 0.0f;
                bool isSet = false;

                if(positionTrack.Size() > 1)
                {
                    result = positionTrack.GetEndTime();
                    isSet = true;
                }
                if(rotationTrack.Size() > 1)
                {
                    float rotationStart = rotationTrack.GetEndTime();
                    if(rotationStart > result || !isSet)
                    {
                        result = rotationStart;
                        isSet = true;
                    }
                }
                if(scaleTrack.Size() > 1)
                {
                    float scaleStart = scaleTrack.GetEndTime();
                    if(scaleStart > result || !isSet)
                    {
                        result = scaleStart;
                        isSet = true;
                    }
                }
                return result;
            }

            Transform TransformTrack::Sample(const Transform &ref, float time, bool looping) const
            {
                Transform result = ref;
                if(positionTrack.Size() > 1)
                    result.position = positionTrack.Sample(time, looping);
                if(rotationTrack.Size() > 1)
                    result.rotation = rotationTrack.Sample(time, looping);
                if(scaleTrack.Size() > 1)
                    result.scale = scaleTrack.Sample(time, looping);
                return result;
            }
        }
    }
}
