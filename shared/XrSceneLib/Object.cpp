// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Object.h"

using engine::Object;

void Object::SetOnlyVisibleForViewIndex(uint32_t viewIndex) {
    assert(viewIndex < m_visibleViewIndexMask.MaxViewCount);
    m_visibleViewIndexMask.m_mask = 1 << viewIndex;
}

bool Object::IsVisibleForViewIndex(uint32_t viewIndex) const {
    assert(viewIndex < m_visibleViewIndexMask.MaxViewCount);
    return (m_visibleViewIndexMask.m_mask & (1 << viewIndex)) > 0;
}

void Object::Update(engine::Context& /*context*/, const FrameTime& frameTime) {
    Motion.UpdateMotionAndPose(Pose(), frameTime.Elapsed);
}

void Object::Render(Context& context) const {
}

DirectX::XMMATRIX Object::LocalTransform() const {
    if (!m_localTransformDirty) {
        return DirectX::XMLoadFloat4x4(&m_localTransform);
    }

    const DirectX::XMMATRIX modelScale = DirectX::XMMatrixScalingFromVector(xr::math::LoadXrVector3(m_scale));
    const DirectX::XMMATRIX modelRotation = DirectX::XMMatrixRotationQuaternion(xr::math::LoadXrQuaternion(m_pose.orientation));
    const DirectX::XMMATRIX modelTranslation = DirectX::XMMatrixTranslationFromVector(xr::math::LoadXrVector3(m_pose.position));
    const DirectX::XMMATRIX localTransform = modelScale * modelRotation * modelTranslation;

    DirectX::XMStoreFloat4x4(&m_localTransform, localTransform);
    m_localTransformDirty = false;

    return localTransform;
}

DirectX::XMMATRIX Object::WorldTransform() const {
    return m_parent ? XMMatrixMultiply(LocalTransform(), m_parent->WorldTransform()) : LocalTransform();
}
