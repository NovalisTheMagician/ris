#include "graphics/Animation.hpp"

namespace RIS
{
    namespace Graphics
    {
        namespace Animation
        {
            template TTransformTrack<VectorTrack, QuaternionTrack>;
            template TTransformTrack<FastVectorTrack, FastQuaternionTrack>;

            template<typename VTRACK, typename QTRACK>
            TTransformTrack<VTRACK, QTRACK>::TTransformTrack()
                : id(0)
            {}

            template<typename VTRACK, typename QTRACK>
            unsigned int TTransformTrack<VTRACK, QTRACK>::GetId() const
            {
                return id;
            }

            template<typename VTRACK, typename QTRACK>
            void TTransformTrack<VTRACK, QTRACK>::SetId(unsigned int id)
            {
                this->id = id;
            }

            template<typename VTRACK, typename QTRACK>
            VTRACK& TTransformTrack<VTRACK, QTRACK>::GetPositionTrack()
            {
                return positionTrack;
            }

            template<typename VTRACK, typename QTRACK>
            QTRACK& TTransformTrack<VTRACK, QTRACK>::GetRotationTrack()
            {
                return rotationTrack;
            }

            template<typename VTRACK, typename QTRACK>
            VTRACK& TTransformTrack<VTRACK, QTRACK>::GetScaleTrack()
            {
                return scaleTrack;
            }

            template<typename VTRACK, typename QTRACK>
            const VTRACK& TTransformTrack<VTRACK, QTRACK>::GetPositionTrack() const
            {
                return positionTrack;
            }

            template<typename VTRACK, typename QTRACK>
            const QTRACK& TTransformTrack<VTRACK, QTRACK>::GetRotationTrack() const
            {
                return rotationTrack;
            }

            template<typename VTRACK, typename QTRACK>
            const VTRACK& TTransformTrack<VTRACK, QTRACK>::GetScaleTrack() const
            {
                return scaleTrack;
            }

            template<typename VTRACK, typename QTRACK>
            bool TTransformTrack<VTRACK, QTRACK>::IsValid() const
            {
                return positionTrack.Size() > 1 || rotationTrack.Size() > 1 || scaleTrack.Size() > 1;
            }

            template<typename VTRACK, typename QTRACK>
            float TTransformTrack<VTRACK, QTRACK>::GetStartTime() const
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

            template<typename VTRACK, typename QTRACK>
            float TTransformTrack<VTRACK, QTRACK>::GetEndTime() const
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

            template<typename VTRACK, typename QTRACK>
            Transform TTransformTrack<VTRACK, QTRACK>::Sample(const Transform &ref, float time, bool looping) const
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

            FastTransformTrack OptimizeTransformTrack(const TransformTrack &input)
            {
                FastTransformTrack result;
                result.SetId(input.GetId());
                result.GetPositionTrack() = OptimizeTrack(input.GetPositionTrack());
                result.GetRotationTrack() = OptimizeTrack(input.GetRotationTrack());
                result.GetScaleTrack() = OptimizeTrack(input.GetScaleTrack());
                return result;
            }
        }
    }
}
