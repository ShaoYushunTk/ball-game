#include "CameraController.h"
#include "XUtil.h"
#include <imgui.h>
#include "PxPhysicsAPI.h"
using namespace DirectX;

extern physx::PxRigidDynamic* ballActor;
extern XMVECTOR getBallPos();

// time
std::chrono::steady_clock::time_point currentJumpTime;
std::chrono::steady_clock::time_point lastJumpTime;
std::chrono::milliseconds jumpInterval{ 2000 };//jump触发间隔为2000ms

void FirstPersonCameraController::Update(float deltaTime)
{
    /*ImGuiIO& io = ImGui::GetIO();

    float yaw = 0.0f, pitch = 0.0f;
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {

        yaw += io.MouseDelta.x * m_MouseSensitivityX;
        pitch += io.MouseDelta.y * m_MouseSensitivityY;
    }
    
    int forward = (
        (ImGui::IsKeyDown(ImGuiKey_W) ? 1 : 0) +
        (ImGui::IsKeyDown(ImGuiKey_S) ? -1 : 0)
        );
    int strafe = (
        (ImGui::IsKeyDown(ImGuiKey_A) ? -1 : 0) +
        (ImGui::IsKeyDown(ImGuiKey_D) ? 1 : 0)
        );
    
    
    if (forward || strafe)
    {
        XMVECTOR dir = m_pCamera->GetLookAxisXM() * (float)forward + m_pCamera->GetRightAxisXM() * (float)strafe;
        XMStoreFloat3(&m_MoveDir, dir);
        m_MoveVelocity = m_MoveSpeed;
        m_DragTimer = m_TotalDragTimeToZero;
        m_VelocityDrag = m_MoveSpeed / m_DragTimer;
    }
    else
    {
        if (m_DragTimer > 0.0f)
        {
            m_DragTimer -= deltaTime;
            m_MoveVelocity -= m_VelocityDrag * deltaTime;
        }
        else
        {
            m_MoveVelocity = 0.0f;
        }
    }

    m_pCamera->RotateY(yaw);
    m_pCamera->Pitch(pitch);

    m_pCamera->Translate(m_MoveDir, m_MoveVelocity * deltaTime);*/
    ImGuiIO& io = ImGui::GetIO();
    float yaw = 0.0f, pitch = 0.0f;
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        yaw += io.MouseDelta.x * m_MouseSensitivityX;
        pitch += io.MouseDelta.y * m_MouseSensitivityY;
    }
    HandleBallKey();
    m_pCamera->RotateY(yaw);
    m_pCamera->Pitch(pitch);
    XMVECTOR ballPos = getBallPos();
    XMVECTOR lookat = m_pCamera->GetLookAxisXM();
    XMVECTOR cameraPos = ballPos - 15.0f * lookat;
    XMFLOAT3 pos;
    XMStoreFloat3(&pos, cameraPos);
    m_pCamera->SetPosition(pos);
}

void FirstPersonCameraController::InitCamera(FirstPersonCamera* pCamera)
{
    m_pCamera = pCamera;
}

void FirstPersonCameraController::SetMouseSensitivity(float x, float y)
{
    m_MouseSensitivityX = x;
    m_MouseSensitivityY = y;
}

void FirstPersonCameraController::SetMoveSpeed(float speed)
{
    m_MoveSpeed = speed;
}

void FirstPersonCameraController::HandleBallKey() {
    ImGuiIO& io = ImGui::GetIO();

    //XMFLOAT3X3 rotMatrix = getCameraRot();
    //XMFLOAT3 forwardVector = XMFLOAT3(rotMatrix._31, rotMatrix._32, rotMatrix._33);
    //XMFLOAT3 upVector = XMFLOAT3(rotMatrix._21, rotMatrix._22, rotMatrix._23);
    //XMFLOAT3 leftVector = XMFLOAT3(rotMatrix._11, rotMatrix._12, rotMatrix._13);
    XMFLOAT3 forwardVector = m_pCamera->GetLookAxis();
    XMFLOAT3 upVector = m_pCamera->GetUpAxis();
    XMFLOAT3 leftVector = m_pCamera->GetRightAxis();
    physx::PxVec3 forward(forwardVector.x, forwardVector.y, forwardVector.z);
    physx::PxVec3 up(upVector.x, upVector.y, upVector.z);
    physx::PxVec3 left(leftVector.x, leftVector.y, leftVector.z);


    if (ImGui::IsKeyDown(ImGuiKey_W))
    {
        ballActor->addForce(5.0f * forward, physx::PxForceMode::eACCELERATION);
    }
    if (ImGui::IsKeyDown(ImGuiKey_S))
    {
        ballActor->addForce(-5.0f * forward, physx::PxForceMode::eACCELERATION);
    }
    if (ImGui::IsKeyDown(ImGuiKey_A))
    {
        ballActor->addForce(-5.0f * left, physx::PxForceMode::eACCELERATION);
    }
    if (ImGui::IsKeyDown(ImGuiKey_D))
    {
        ballActor->addForce(5.0f * left, physx::PxForceMode::eACCELERATION);
    }

    

    if (ImGui::IsKeyReleased(ImGuiKey_Space))
    {
        currentJumpTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentJumpTime - lastJumpTime);
        if (elapsed >= jumpInterval)
        {
            ballActor->addForce(12.0f * up, physx::PxForceMode::eVELOCITY_CHANGE);
            lastJumpTime = currentJumpTime;
        }
    }


    if (ImGui::IsKeyDown(ImGuiKey_F))
    {
        ballActor->addForce(30.0f * up, physx::PxForceMode::eIMPULSE);
    }
 
}
XMFLOAT3X3 FirstPersonCameraController::getCameraRot() {
    XMVECTOR forward = XMVector3Normalize((getBallPos() - m_pCamera->GetLookAxisXM()));
    XMFLOAT3 upfloat(0, 1, 0);
    XMVECTOR up = XMLoadFloat3(&upfloat);
    XMVECTOR left = XMVector3Normalize(XMVector3Cross(up, forward));
    XMVECTOR newUp = XMVector3Normalize(XMVector3Cross(forward, left));
    XMMATRIX rot;
    rot.r[0] = XMVectorSet(left.m128_f32[0], left.m128_f32[1], left.m128_f32[2], 0.0f);
    rot.r[1] = XMVectorSet(newUp.m128_f32[0], newUp.m128_f32[1], newUp.m128_f32[2], 0.0f);
    rot.r[2] = XMVectorSet(-forward.m128_f32[0], -forward.m128_f32[1], -forward.m128_f32[2], 0.0f);
    rot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMFLOAT3X3 rotMatrix;
    XMStoreFloat3x3(&rotMatrix, rot);
    return rotMatrix;
}
