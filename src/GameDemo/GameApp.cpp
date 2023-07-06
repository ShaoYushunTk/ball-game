#include "GameApp.h"
#include <XUtil.h>
#include <DXTrace.h>
#include<DemoProject.h>
#include<PxPhysicsAPI.h>
#include<fstream>
#include "../Common/Print.h"
#include "../Common/PVD.h"
#include "../Utils/Utils.h"
#include "../../PhysX_3.4/Snippets/SnippetCommon/SnippetPrint.h"
#include "../../PhysX_3.4/Snippets/SnippetCommon/SnippetPVD.h"
#include "../../PhysX_3.4/Snippets/SnippetUtils/SnippetUtils.h"
#include<vector>
#include "foundation/PxPreprocessor.h"
#include<GameObject.h>
#include<string>
#include<imgui.h>
#include <queue>
#define MAX_NUM_ACTOR_SHAPES 500
using namespace DirectX;
using namespace physx;
#include <iostream>
PxU32 lastframeObeject = 0;
PxU32 ThisframeObject = 0;



PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* gMaterial = NULL;
PxMaterial* ballMaterial = NULL;
PxPvd* gPvd = NULL;

PxReal stackX = 10.0f;
PxReal stackY = 0.0f;
PxReal stackZ = 10.0f;

PxRigidStatic** bodies = nullptr;
PxRigidDynamic** bodies_dynamic = nullptr;

//设置平台上下移动相关参数
float maxHeight = 159.0f;  // 平台的最大高度
float minHeight = 130.0f;  // 平台的最小高度
float speed = 0.1f;  // 上升下降速度
PxRigidDynamic* planeActor = NULL;

float minX = -76.0f;
float maxX = -30.0f;
PxRigidDynamic* planeActor2 = NULL;

PxCooking* gCooking = NULL;
PxTriangleMesh* platformMesh = NULL;
PxRigidStatic* platformActor = NULL;

static const PxU32 gGridSize = 10;
static const PxReal gGridStep = 64.0f / PxReal(gGridSize - 1);
static float gTime = 0.0f;


PxReal density_PushStack = 0.01f;
PxReal density_CrashStack = 1.0f;

PxController* gController = NULL;
PxControllerManager* gControllerManager = NULL;
PxVec3 gControllerInitialPosition;
PxRigidDynamic* ballActor = NULL;
PxArticulation* gArticulation = NULL;

// trigger
PxVec3 triggerPos1 = PxVec3(0, 70, 0);
PxRigidStatic* triggerActor1 = NULL;
PxVec3 triggerPos2 = PxVec3(174, 141, 230);
PxRigidStatic* triggerActor2 = NULL;
std::string gameState = "normal";
PxVec3 ballInitPos = PxVec3(22, 110, 22);
bool ballOnTrigger = false;
bool isCreate = false;


std::queue<PxRigidStatic*> bodiesQueue;
std::queue<PxRigidDynamic**> bodies_dynamicQueue;
std::queue<PxArticulation*> bodies_ArticulationQueue;
std::queue<PxRigidDynamic*> bodies_KinematicPlatformQueue;
std::queue<PxRigidStatic*> bodies_RigidStaticQueue;
std::queue<PxJoint**> bodies_ChainJointQueue;
std::queue<PxRigidDynamic*> bodies_ChainDynamicQueue;

// time
std::chrono::steady_clock::time_point currentTime;
std::chrono::steady_clock::time_point lastTriggerTime;
std::chrono::steady_clock::time_point lastLaunchTime;
std::chrono::milliseconds triggerInterval{ 100 };//trigger触发间隔为100ms
std::chrono::milliseconds launchInterval{ 200 };//launch触发间隔为100ms


void changeBallPos(PxVec3 pos);
XMVECTOR getBallPos();
PxVec3 getLatestPlanePos();
extern unsigned int loadTexture2D(char const* path);

// ImGui Colors
const ImVec4 button_color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
const ImVec4 button_color_hovered = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
const ImVec4 button_color_active = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
const ImVec4 letters_color = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

// ImGui windows
bool game_loading = true;
bool show_menu = false;
bool show_ingame_menu = false;
bool ingame = false;
bool game_over = false;
ImFont* Font;
ImFont* Font2;


// Background image
ImTextureID loading_background_image = 0;
ImTextureID main_menu_background_image = 0;


int numofall = -1;
int numofbox = -1;
int numofsphere = -1;
int numofplane = -1;
int numofTmesh = -1;
int numofcy = -1;
static float addTime = 0.0f;
//int textOfTerrain[] = { 0,1,2,3,4,5,6,7,8,3, 5, 2, 6, 7, 6, 2, 6, 1, 8, 1, 4, 0, 5, 3, 7, 1, 4, 2, 8, 3, 6, 4, 3, 2, 1, 4, 6, 2, 6, 7, 8, 1, 1, 1, 6, 3, 0, 1, 1, 3, 1, 4, 4, 7, 6, 8, 6, 1, 2, 4, 0, 1, 3, 0, 0, 8, 2, 4, 4, 7, 4, 0, 6, 1, 6, 8, 4, 0, 1, 3, 6, 4, 1, 1, 5, 2, 3, 3, 4, 0, 1, 0, 8, 1, 4, 4, 4, 2, 5, 1, 7, 4, 0, 0, 0, 6, 4, 1, 4, 3, 2, 1, 1, 4, 0, 4, 0, 6, 1, 2, 5, 4 };
int textOfTerrain[] = { 0,1,2,3,4,5,6,7,8,  0,1,2,3,4,5,6,7,8,  0,1,2,3,4,5,6,7,8,  0,1,2,3,4,5,6,7,8,  0,1,2,3,4,5,6,7,8,  0,1,2,3,4,5,6,7,8,  0,1,2,3,4,5,6,7,8 };
int CreateTextOfTerrain[] = { 0 ,5 ,6 ,1 ,0 ,3 ,0 ,2 ,4 ,2 ,4 ,2 ,5 ,4 ,0 ,5 ,6 ,8 ,0 ,4 ,0 ,5 ,6 ,1 ,0 ,4 ,0 ,3 ,0 ,2 ,1 ,2 ,5 ,0 ,5 ,4 ,1 ,2 ,3 ,2 ,3 ,2 ,4 ,3 ,4 ,0 ,5 ,2 ,5 ,0 ,5 ,6 ,5 ,6 };
int DeleteTextOfTerrain[] = { 0,1,2,3,4,5,6,7,8, 0 ,5 ,6 ,1 ,0 ,3 ,0 ,2 ,4 ,2 ,4 ,2 ,5 ,4 ,0 ,5 ,6 ,8 ,0 ,4 ,0 ,5 ,6 ,1 ,0 ,4 ,0 ,3 ,0 ,2 ,1 ,2 ,5 ,0 ,5 ,4 ,1 ,2 ,3 ,2 ,3 ,2 ,4 ,3 ,4 ,0 ,5 ,2 ,5 ,0 ,5 ,6 ,5 ,6 };
int iText = 0;
int iText2 = 0;
static float delTime = 0.0f;

bool isJump = false;


PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);
typedef PxJoint* (*JointCreateFunction)(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);
PxJoint** createChain(const PxTransform& t, PxU32 length, const PxGeometry& g1, const PxGeometry& g2, PxReal separation, JointCreateFunction createJoint);
void deleteTerrain_Chain(std::queue<PxJoint**>& n_bodies_JointQueue, std::queue<PxRigidDynamic*>& n_bodies_ChainDynamicQueue);
void setTerrainSeed();

GameApp::GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
    : D3DApp(hInstance, windowName, initWidth, initHeight)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
    if (!D3DApp::Init())
        return false;

    m_TextureManager.Init(m_pd3dDevice.Get());
    m_ModelManager.Init(m_pd3dDevice.Get());

    // 务必先初始化所有渲染状态，以供下面的特效使用
    RenderStates::InitAll(m_pd3dDevice.Get());

    if (!m_BasicEffect.InitAll(m_pd3dDevice.Get()))
        return false;

    if (!m_SkyboxEffect.InitAll(m_pd3dDevice.Get()))
        return false;

    if (!m_ShadowEffect.InitAll(m_pd3dDevice.Get()))
        return false;

    if (!initPhysics())
        return false;

    if (!InitResource())
        return false;

    return true;
}

//ContactReportCallback gContactReportCallback;

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    PX_UNUSED(attributes0);
    PX_UNUSED(attributes1);
    PX_UNUSED(filterData0);
    PX_UNUSED(filterData1);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(constantBlock);

    //
    // Enable CCD for the pair, request contact reports for initial and CCD contacts.
    // Additionally, provide information per contact point and provide the actor
    // pose at the time of contact.
    //

    pairFlags = PxPairFlag::eCONTACT_DEFAULT
        | PxPairFlag::eDETECT_CCD_CONTACT
        | PxPairFlag::eNOTIFY_TOUCH_CCD
        | PxPairFlag::eNOTIFY_TOUCH_FOUND
        | PxPairFlag::eNOTIFY_CONTACT_POINTS
        | PxPairFlag::eCONTACT_EVENT_POSE;
    return PxFilterFlag::eDEFAULT;
}

struct FilterGroup
{
    enum Enum {
        eBall = (1 << 0),
        eLanuchBall = (1 << 1),
        eBox = (1 << 2),
    };
};

PxFilterFlags ballFilterShader(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // let triggers through
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }
    // check for intersection of filter data
    if ((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1))
    {
        // if the two objects' filter data intersect, don't generate any contacts
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eSUPPRESS;
    }

    // otherwise, generate contacts
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;
    return PxFilterFlag::eDEFAULT;

}

class MySimulationFilterCallback : public PxSimulationFilterCallback
{
public:
    virtual PxFilterFlags pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1, PxPairFlags& pairFlags) override
    {
        // let triggers through
        if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
        {
            pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
            return PxFilterFlag::eDEFAULT;
        }
        // 定义需要触发接触回调的碰撞对
        if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
        {
            pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
        }
        return PxFilterFlag::eDEFAULT;
    }
    virtual void pairLost(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, bool objectRemoved) override
    {
        // 处理碰撞对结束的情况
    }
    virtual bool statusChange(PxU32& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags) override
    {
        // 处理碰撞对状态变化的情况
        return true;
    }
};

// trigger
void GameApp::createTrigger() {
    // trigger1用于触发重新开始
    triggerActor1 = gPhysics->createRigidStatic(PxTransform(triggerPos1));
    PxShape* triggerShape1 = gPhysics->createShape(PxBoxGeometry(1000, 1, 1000), *gMaterial);
    triggerShape1->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    triggerShape1->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
    triggerShape1->userData = (void*)"trigger1";
    triggerActor1->attachShape(*triggerShape1);
    gScene->addActor(*triggerActor1);

    // trigger2用于触发地形生成
    triggerActor2 = gPhysics->createRigidStatic(PxTransform(triggerPos2));
    PxShape* triggerShape2 = gPhysics->createShape(PxBoxGeometry(5, 5, 5), *gMaterial);
    triggerShape2->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    triggerShape2->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
    triggerShape2->userData = (void*)"trigger2";
    triggerActor2->attachShape(*triggerShape2);
    gScene->addActor(*triggerActor2);

}

class MySimulationEventCallback : public PxSimulationEventCallback {
    void onTrigger(PxTriggerPair* pairs, PxU32 count) {
        currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTriggerTime);
        if (elapsed >= triggerInterval)
        {
            for (PxU32 i = 0; i < count; i++)
            {
                PxTriggerPair& pair = pairs[i];
                if (pair.otherActor == ballActor) {
                    PxShape* triggerShape = pair.triggerShape;

                    if (triggerShape->userData == (void*)"trigger1") {
                        // 落地重新开始
                        gameState = "restart";
                        ballOnTrigger = true;
                        lastTriggerTime = currentTime;
                    }

                    if (triggerShape->userData == (void*)"trigger2") {
                        // 触发后开始创建地形
                        isCreate = true;
                        ballOnTrigger = true;
                        lastTriggerTime = currentTime;
                    }

                }
            }
        }

    }
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {};
    void onWake(PxActor** actors, PxU32 count) {};
    void onSleep(PxActor** actors, PxU32 count) {};
    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {};
    void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {};
};

MySimulationEventCallback* mySimulationEventCallback = new MySimulationEventCallback();

void GameApp::OnResize()
{
    D3DApp::OnResize();

    m_pDepthTexture = std::make_unique<Depth2D>(m_pd3dDevice.Get(), m_ClientWidth, m_ClientHeight);
    m_pLitTexture = std::make_unique<Texture2D>(m_pd3dDevice.Get(), m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

    m_pDepthTexture->SetDebugObjectName("DepthTexture");
    m_pLitTexture->SetDebugObjectName("LitTexture");

    // 摄像机变更显示
    if (m_pCamera != nullptr)
    {
        m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
        m_pCamera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
        m_BasicEffect.SetProjMatrix(m_pCamera->GetProjMatrixXM());
        m_SkyboxEffect.SetProjMatrix(m_pCamera->GetProjMatrixXM());
    }
}


bool GameApp::initPhysics()
{  //PxFoundation(版本号,内存回调,错误回调)
    gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);
    //PVD
    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    //创建顶级PxPhysics对象
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

    PxInitExtensions(*gPhysics, gPvd);
    //?缩放
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    //重力
    sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
    PxU32 numCores = SnippetUtils::getNbPhysicalCores();
    gDispatcher = PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
    sceneDesc.cpuDispatcher = gDispatcher;
    sceneDesc.filterShader = ballFilterShader;
    sceneDesc.simulationEventCallback = mySimulationEventCallback;
    gScene = gPhysics->createScene(sceneDesc);



    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    //静摩擦，动摩擦，restitution恢复原状(弹性)

    //弹性平面
    PxCookingParams cookingParams(gPhysics->getTolerancesScale());

    // Deformable meshes are only supported with PxMeshMidPhase::eBVH33.
    cookingParams.midphaseDesc.setToDefault(PxMeshMidPhase::eBVH33);
    // We need to disable the mesh cleaning part so that the vertex mapping remains untouched.
    cookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, cookingParams);

    gMaterial = gPhysics->createMaterial(0.3f, 0.3f, 0.6f);

    initGame();
    return true;


}

void GameApp::cleanupPhysics()
{
    //release()销毁对象以及包含的所有对象
    //PX_UNUSED(interactive);
    gScene->release();
    gDispatcher->release();
    gPhysics->release();
    PxPvdTransport* transport = gPvd->getTransport();
    gPvd->release();
    transport->release();
    PxCloseExtensions();
    gFoundation->release();

    //printf("HelloWorld done.\n");
}

void GameApp::UpdateScene(float dt)
{

    ThisframeObject = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
    // ImGui Rendering
    if (game_loading == true || show_menu == true || show_ingame_menu == true || game_over == true)
    {
        // 加载页面
        if (game_loading == true)
        {
            // 全屏
            ImGui::SetNextWindowPos(ImVec2((float)0, (float)0));
            ImGui::SetNextWindowSize(ImVec2((float)m_ClientWidth, (float)m_ClientHeight));
            ImGui::Begin("0", 0,
                ImGuiWindowFlags_NoTitleBar   //没有标题栏
                | ImGuiWindowFlags_NoScrollbar  //没有右下角缩放键
                | ImGuiWindowFlags_NoResize     //不能调整窗口大小
                | ImGuiWindowFlags_NoMove       //不能移动窗口
                | ImGuiWindowFlags_NoCollapse); //不能折叠

            /*int width, height, channels;
            unsigned char* image_data = stbi_load("../../src/Resource/image/container.jpg", &width, &height, &channels, 0);

            GLuint texture_id;
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(image_data);

            ImGui::GetWindowDrawList()->AddImage((ImTextureID)texture_id,
                ImVec2(0, 0),
                ImVec2((float)m_ClientWidth, (float)m_ClientHeight),
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f),
                ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));*/

            ImGui::GetWindowDrawList()->AddImage((ImTextureID)loading_background_image,
                ImVec2(0, 0),
                ImVec2((float)m_ClientWidth, (float)m_ClientHeight),
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f));
            //ImGui::Image(loading_background_image, ImVec2((float)m_ClientWidth, (float)m_ClientHeight));

            ImGui::PushFont(Font);



            // 模拟加载过程
            static int progress = 0; // 初始化进度条变量
            static float timer = 0.0f;
            static float delay = 0.5f; // 设置延时时间，单位为秒

            if (progress >= 0 && progress <= 30)
            {
                ImVec4 progress_bar_color_red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 红色
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, progress_bar_color_red);
            }
            else if (progress > 30 && progress <= 60)
            {
                ImVec4 progress_bar_color_yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // 黄色
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, progress_bar_color_yellow);
            }
            else if (progress > 60 && progress < 100)
            {
                ImVec4 progress_bar_color_green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // 绿色
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, progress_bar_color_green);
            }

            // 更新进度条
            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 300));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2 + 200));
            ImGui::ProgressBar(progress / 100.0f, ImVec2((float)600.0f, (float)30.0f));
            ImGui::PopStyleColor();
            //ImGui::Text("Loading... %d%%", progress);
            timer += m_Timer.DeltaTime();
            if (timer >= delay)
            {
                progress += 10;
                timer = 0.0f;
            }

            if (progress >= 100)
            {
                game_loading = false;
                show_menu = true;
            }

            ImGui::PopFont();
            ImGui::End();
        }
        // 主页面
        if (show_menu == true)
        {
            // 全屏
            ImGui::SetNextWindowPos(ImVec2((float)0, (float)0));
            ImGui::SetNextWindowSize(ImVec2((float)m_ClientWidth, (float)m_ClientHeight));


            ImGui::Begin("1", 0,
                ImGuiWindowFlags_NoBackground //背景透明
                | ImGuiWindowFlags_NoTitleBar   //没有标题栏
                | ImGuiWindowFlags_NoScrollbar  //没有右下角缩放键
                | ImGuiWindowFlags_NoResize     //不能调整窗口大小
                | ImGuiWindowFlags_NoMove       //不能移动窗口
                | ImGuiWindowFlags_NoCollapse); //不能折叠

            ImGui::GetWindowDrawList()->AddImage((ImTextureID)main_menu_background_image,
                ImVec2(0, 0),
                ImVec2((float)m_ClientWidth, (float)m_ClientHeight),
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f));

            ImGui::PushFont(Font2);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::SetCursorPos(ImVec2((float)(m_ClientWidth / 2 - 350), (float)(m_ClientHeight / 2 - 350)));
            ImGui::Text("Ball Game");
            ImGui::PopStyleColor();
            ImGui::PopFont();


            ImGui::PushFont(Font);
            //ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255, 0, 0, 255));
            ImGui::PushStyleColor(ImGuiCol_Button, button_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_color_hovered);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_color_active);

            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 100));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2 - 100));
            if (ImGui::Button("START", ImVec2((float)200, (float)100)))
            {
                m_Timer.Reset();
                m_Timer.Start();
                show_menu = false;
                ingame = true;
            }

            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 100));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2 + 100));
            if (ImGui::Button("QUIT", ImVec2((float)200, (float)100)))
            {
                //关闭游戏
                mciSendString(L"stop BGM", nullptr, 0, nullptr);
                DestroyWindow(m_hMainWnd);
            }

            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            ImGui::End();

        }
        // 结算页面
        else if (game_over == true)
        {
            mciSendString(L"stop BGM", nullptr, 0, nullptr);

            ImGui::SetNextWindowPos(ImVec2((float)0, (float)0));
            ImGui::SetNextWindowSize(ImVec2((float)m_ClientWidth, (float)m_ClientHeight));

            ImGui::Begin("2", 0,
                ImGuiWindowFlags_NoTitleBar   //没有标题栏
                | ImGuiWindowFlags_NoScrollbar  //没有右下角缩放键
                | ImGuiWindowFlags_NoResize     //不能调整窗口大小
                | ImGuiWindowFlags_NoMove       //不能移动窗口
                | ImGuiWindowFlags_NoCollapse); //不能折叠


            ImGui::PushStyleColor(ImGuiCol_Button, button_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_color_hovered);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_color_active);

            ImGui::PushFont(Font);
            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 100));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2 - 130));
            ImGui::Text("GAME OVER!");
            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 200));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2 - 100));
            ImGui::Text("Keep-alive time: %.2f s", getGameTotalTime());
            ImGui::PopFont();

            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 100));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2));
            if (ImGui::Button("QUIT", ImVec2(200, 100)))
            {
                //关闭游戏

                DestroyWindow(m_hMainWnd);
            }

            ImGui::PopStyleColor(3);
            ImGui::End();

        }
        // 游戏内菜单
        else if (show_ingame_menu == true)
        {
            m_Timer.Stop();
            mciSendString(L"pause BGM", nullptr, 0, nullptr);

            ImGui::SetNextWindowPos(ImVec2((float)0, (float)0));
            ImGui::SetNextWindowSize(ImVec2((float)m_ClientWidth, (float)m_ClientHeight));


            ImGui::Begin("3", 0,
                ImGuiWindowFlags_NoTitleBar   //没有标题栏
                | ImGuiWindowFlags_NoScrollbar  //没有右下角缩放键
                | ImGuiWindowFlags_NoResize     //不能调整窗口大小
                | ImGuiWindowFlags_NoMove       //不能移动窗口
                | ImGuiWindowFlags_NoCollapse); //不能折叠

            ImGui::PushFont(Font);
            //ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255, 0, 0, 255));
            ImGui::PushStyleColor(ImGuiCol_Button, button_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_color_hovered);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_color_active);

            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 100));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2 - 150));
            if (ImGui::Button("RESUME", ImVec2((float)200, (float)100)))
            {
                m_Timer.Start();
                mciSendString(L"resume BGM", nullptr, 0, nullptr);
                show_ingame_menu = false;
            }

            ImGui::SetCursorPosX((float)(m_ClientWidth / 2 - 100));
            ImGui::SetCursorPosY((float)(m_ClientHeight / 2 + 50));
            if (ImGui::Button("QUIT", ImVec2((float)200, (float)100)))
            {
                //关闭游戏
                mciSendString(L"stop BGM", nullptr, 0, nullptr);
                DestroyWindow(m_hMainWnd);
            }

            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            ImGui::End();
        }
    }
    // 游戏画面ui
    else
    {

        ImGui::SetNextWindowPos(ImVec2((float)0, (float)0));
        ImGui::SetNextWindowSize(ImVec2((float)m_ClientWidth, (float)m_ClientHeight));

        ImGui::Begin("4", 0,
            ImGuiWindowFlags_NoBackground //背景透明
            | ImGuiWindowFlags_NoTitleBar   //没有标题栏
            | ImGuiWindowFlags_NoScrollbar  //没有右下角缩放键
            | ImGuiWindowFlags_NoResize     //不能调整窗口大小
            | ImGuiWindowFlags_NoMove       //不能移动窗口
            | ImGuiWindowFlags_NoCollapse); //不能折叠

        ImGui::PushFont(Font);
        ImGui::SetCursorPosX(10);
        ImGui::SetCursorPosY(0);
        ImGui::Text("HP: %d", getHP());
        ImGui::SetCursorPosX(10);
        ImGui::SetCursorPosY(40);
        ImGui::Text("Keep-alive time: %.2f s", m_Timer.TotalTime());
        ImGui::PopFont();

        ImGui::End();

        static const DirectX::XMFLOAT3 lightDirs[] = {
            XMFLOAT3(1.0f / sqrtf(2.0f), -1.0f / sqrtf(2.0f), 0.0f),
            XMFLOAT3(3.0f / sqrtf(13.0f), -2.0f / sqrtf(13.0f), 0.0f),
            XMFLOAT3(2.0f / sqrtf(5.0f), -1.0f / sqrtf(5.0f), 0.0f),
            XMFLOAT3(3.0f / sqrtf(10.0f), -1.0f / sqrtf(10.0f), 0.0f),
            XMFLOAT3(4.0f / sqrtf(17.0f), -1.0f / sqrtf(17.0f), 0.0f)
        };


        m_CameraController.Update(dt);
        //m_CameraController.HandleBallKey();


        if (ImGui::IsKeyDown(ImGuiKey_Escape))
        {
            if (show_menu == false && show_ingame_menu == false)
            {
                show_ingame_menu = true;
            }
        }
        if (ImGui::IsKeyDown(ImGuiKey_Q) || getHP() == 0)
        {
            game_over = true;
            setGameTotalTime();
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            XMFLOAT3 cameraPos = m_pCamera->GetTransform().m_Position;
            XMFLOAT4 cameraRot = m_pCamera->GetTransform().m_Rotation;
            PxTransform transform;
            XMFLOAT3 forwardVector = m_pCamera->GetLookAxis();
            XMFLOAT3 upVector = m_pCamera->GetUpAxis();
            physx::PxVec3 forward(forwardVector.x, forwardVector.y, forwardVector.z);
            physx::PxVec3 up(upVector.x, upVector.y, upVector.z);
            transform.p = PxVec3(cameraPos.x, cameraPos.y, cameraPos.z) + forward.getNormalized() * 20.0f;
            transform.q = PxQuat(cameraRot.x, cameraRot.y, cameraRot.z, cameraRot.w);

            createDynamic(transform, PxSphereGeometry(1.0f), (forward) * 200);
        }
        if (!ballOnTrigger) {
            gameState = "normal";
        }
        if (gameState == "restart") {
            gameState == "normal";
            if (m_HP > 0) {
                m_HP--;
                changeBallPos(getLatestPlanePos());
            }
        }
        ballOnTrigger = false;


        m_SkyboxEffect.SetViewMatrix(m_pCamera->GetViewMatrixXM());
        m_BasicEffect.SetViewMatrix(m_pCamera->GetViewMatrixXM());
        m_BasicEffect.SetEyePos(m_pCamera->GetPosition());

        // 更新光照
        static float theta = 0;
        if (m_UpdateLight)
        {
            theta += XM_2PI * dt / 40.0f;
        }

        for (int i = 0; i < 3; ++i)
        {
            XMVECTOR dirVec = XMLoadFloat3(&m_OriginalLightDirs[i]);
            dirVec = XMVector3Transform(dirVec, XMMatrixRotationY(theta));
            XMStoreFloat3(&m_DirLights[i].direction, dirVec);
            m_BasicEffect.SetDirLight(i, m_DirLights[i]);
        }

        //
        // 投影区域为正方体，以原点为中心，以方向光为+Z朝向
        //
        XMVECTOR dirVec = XMLoadFloat3(&m_DirLights[0].direction);
        XMMATRIX LightView = XMMatrixLookAtLH(dirVec * 30.0f * (-1.0f), g_XMZero, g_XMIdentityR1);
        m_ShadowEffect.SetViewMatrix(LightView);

        // 将NDC空间 [-1, +1]^2 变换到纹理坐标空间 [0, 1]^2
        static XMMATRIX T(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f);
        // S = V * P * T
        m_BasicEffect.SetShadowTransformMatrix(LightView * XMMatrixOrthographicLH(800.0f, 800.0f, 20.0f, 60.0f) * T);


        stepPhysics(dt);
        if (m_Timer.TotalTime() >= 160.0f && !m_ChangePlane.empty())
        {
            m_ChangePlane.erase(m_ChangePlane.begin());
        }
        lastframeObeject = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
        if (lastframeObeject > ThisframeObject)
        { //渲染动态静态刚体
            PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
            std::vector<PxRigidActor*> actors(nbActors);
            gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
            RenderActors(&actors[ThisframeObject], lastframeObeject - ThisframeObject);
        }
        //else  if (lastframeObeject < ThisframeObject)
        //{   

        //    auto iter = m_Box.begin();
        //    std::advance(iter, 101);//从102号box开始删除
        //    int count = 0;
        //    while (iter != m_Box.end())
        //    {
        //        iter->GetModel()->GetPhysicsData()->release();
        //        iter->SetModel(nullptr);
        //        iter = m_Box.erase(iter);

        //        numofbox--;
        //        count++;
        //        
        //        if (count >=ThisframeObject - lastframeObeject)
        //        {
        //            break;
        //        }

        //    }
        //}
        else if (lastframeObeject < ThisframeObject)
        {
            auto iter = m_Box.begin();
            std::advance(iter, 0); // 从102号box开始删除
            int count = 0;
            int numToDelete = ThisframeObject - lastframeObeject;
            while (iter != m_Box.end() && count < numToDelete)
            {
                //iter->GetModel()->GetPhysicsData()->release();
                //iter->SetModel(nullptr);
                iter = m_Box.erase(iter);
                numofbox--;
                count++;
            }
        }
        else
        {
            for (auto& box : m_Box)
                UpdatePhysics(box);
            for (auto& sphere : m_Sphere)
                UpdatePhysics(sphere);
        }



    }

}

void GameApp::stepPhysics(float dt)
{

    {
        PxVec3* verts = platformMesh->getVerticesForModification();
        // 更新计时器
        gTime += 0.05f;
        delTime += dt;
        if (isCreate)
        {
            addTime += dt;
        }
        updateVertices(verts, sinf(gTime) * 5.0f);
        PxBounds3 newBounds = platformMesh->refitBVH();
        PX_UNUSED(newBounds);

        // Reset filtering to tell the broadphase about the new mesh bounds.
        gScene->resetFiltering(*platformActor);


        //移动平面
        if (planeActor != nullptr)
        {
            PxVec3 planePosition = planeActor->getGlobalPose().p;
            updateKinematicPlatform(&planePosition, planeActor);
        }
        if (bodies_KinematicPlatformQueue.empty() != true)
        {
            PxVec3 planePosition_Seed = bodies_KinematicPlatformQueue.back()->getGlobalPose().p;
            updateKinematicPlatform(&planePosition_Seed, bodies_KinematicPlatformQueue.back());
        }
        //PxVec3 planePosition2 = planeActor2->getGlobalPose().p;
        //updateKinematicPlatform2(&planePosition2);
    }
    gScene->simulate(dt);
    gScene->fetchResults(true);
    createTerrain_Seed();

}

void GameApp::UpdatePhysics(GameObject& ob)
{
    if (ob.GetModel()->GetPhysicsData() != nullptr)
    {
        PxTransform transform = ob.GetModel()->GetPhysicsData()->getGlobalPose();//更新位置信息
        XMFLOAT3 position(transform.p.x, transform.p.y, transform.p.z);
        ob.GetTransform().SetPosition(position);
        PxQuat rotationMatrix(transform.q);
        PxMat33 rotation(rotationMatrix);
        XMFLOAT4X4 dxrotationMatrix;
        dxrotationMatrix._11 = rotation.column0.x;
        dxrotationMatrix._12 = rotation.column0.y;
        dxrotationMatrix._13 = rotation.column0.z;
        dxrotationMatrix._14 = 0.0f;
        dxrotationMatrix._21 = rotation.column1.x;
        dxrotationMatrix._22 = rotation.column1.y;
        dxrotationMatrix._23 = rotation.column1.z;
        dxrotationMatrix._24 = 0.0f;
        dxrotationMatrix._31 = rotation.column2.x;
        dxrotationMatrix._32 = rotation.column2.y;
        dxrotationMatrix._33 = rotation.column2.z;
        dxrotationMatrix._34 = 0.0f;
        dxrotationMatrix._41 = 0.0f;
        dxrotationMatrix._42 = 0.0f;
        dxrotationMatrix._43 = 0.0f;
        dxrotationMatrix._44 = 1.0f;
        XMFLOAT3 eulerangle = Transform::GetEulerAnglesFromRotationMatrix(dxrotationMatrix);
        ob.GetTransform().SetRotation(eulerangle);
    }
}

void GameApp::DrawScene()
{
    // 创建后备缓冲区的渲染目标视图
    if (m_FrameCount < m_BackBufferCount)
    {
        ComPtr<ID3D11Texture2D> pBackBuffer;
        m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
        CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
        m_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), &rtvDesc, m_pRenderTargetViews[m_FrameCount].ReleaseAndGetAddressOf());
    }




    RenderShadow();
    RenderForward();
    RenderSkybox();
    if (m_EnableDebug)
    {
        if (ImGui::Begin("Depth Buffer", &m_EnableDebug))
        {
            CD3D11_VIEWPORT vp(0.0f, 0.0f, (float)m_pDebugShadowTexture->GetWidth(), (float)m_pDebugShadowTexture->GetHeight());
            m_ShadowEffect.RenderDepthToTexture(
                m_pd3dImmediateContext.Get(),
                m_pShadowMapTexture->GetShaderResource(),
                m_pDebugShadowTexture->GetRenderTarget(),
                vp);

            ImVec2 winSize = ImGui::GetWindowSize();
            float smaller = (std::min)(winSize.x - 20, winSize.y - 36);
            ImGui::Image(m_pDebugShadowTexture->GetShaderResource(), ImVec2(smaller, smaller));
        }
        ImGui::End();
    }
    ImGui::Render();

    ID3D11RenderTargetView* pRTVs[]{ GetBackBufferRTV() };
    m_pd3dImmediateContext->OMSetRenderTargets(1, pRTVs, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    HR(m_pSwapChain->Present(0, m_IsDxgiFlipModel ? DXGI_PRESENT_ALLOW_TEARING : 0));
}

void GameApp::RenderShadow()
{
    CD3D11_VIEWPORT shadowViewport(0.0f, 0.0f, (float)m_pShadowMapTexture->GetWidth(), (float)m_pShadowMapTexture->GetHeight());
    m_pd3dImmediateContext->ClearDepthStencilView(m_pShadowMapTexture->GetDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pd3dImmediateContext->OMSetRenderTargets(0, nullptr, m_pShadowMapTexture->GetDepthStencil());
    m_pd3dImmediateContext->RSSetViewports(1, &shadowViewport);


    m_ShadowEffect.SetRenderDepthOnly();
    DrawScene<ShadowEffect>(m_ShadowEffect);
}

void GameApp::RenderForward()
{
    float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    ID3D11RenderTargetView* pRTVs[]{ m_pLitTexture->GetRenderTarget() };
    m_pd3dImmediateContext->ClearRenderTargetView(pRTVs[0], black);
    m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthTexture->GetDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pd3dImmediateContext->OMSetRenderTargets(1, pRTVs, m_pDepthTexture->GetDepthStencil());
    D3D11_VIEWPORT vp = m_pCamera->GetViewPort();
    m_pd3dImmediateContext->RSSetViewports(1, &vp);

    m_BasicEffect.SetTextureShadowMap(m_pShadowMapTexture->GetShaderResource());

    if (m_EnableNormalMap)
        m_BasicEffect.SetRenderWithNormalMap();
    else
        m_BasicEffect.SetRenderDefault();


    DrawScene<BasicEffect>(m_BasicEffect, [](BasicEffect& effect, ID3D11DeviceContext* deviceContext) {
        effect.SetRenderDefault();
        });


    m_BasicEffect.SetTextureShadowMap(nullptr);
    m_BasicEffect.Apply(m_pd3dImmediateContext.Get());
}

void GameApp::RenderSkybox()
{
    D3D11_VIEWPORT skyboxViewport = m_pCamera->GetViewPort();
    skyboxViewport.MinDepth = 1.0f;
    skyboxViewport.MaxDepth = 1.0f;
    m_pd3dImmediateContext->RSSetViewports(1, &skyboxViewport);

    m_SkyboxEffect.SetRenderDefault();
    m_SkyboxEffect.SetDepthTexture(m_pDepthTexture->GetShaderResource());
    m_SkyboxEffect.SetLitTexture(m_pLitTexture->GetShaderResource());

    // 由于全屏绘制，不需要用到深度缓冲区，也就不需要清空后备缓冲区了
    ID3D11RenderTargetView* pRTVs[] = { GetBackBufferRTV() };
    m_pd3dImmediateContext->OMSetRenderTargets(1, pRTVs, nullptr);
    m_Skybox.Draw(m_pd3dImmediateContext.Get(), m_SkyboxEffect, m_Skybox.m_pModel);

    // 解除绑定
    m_pd3dImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
    m_SkyboxEffect.SetDepthTexture(nullptr);
    m_SkyboxEffect.SetLitTexture(nullptr);
    m_SkyboxEffect.Apply(m_pd3dImmediateContext.Get());
}

void GameApp::RenderActors(PxRigidActor** actors, const PxU32 numActors)
{

    Model** pModels = new Model * [numActors];
    for (PxU32 i = 0; i < numActors; ++i) {
        pModels[i] = nullptr;
    }//初始化model指针数组
    PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
    for (PxU32 i = 0; i < numActors; i++)
    {
        const PxU32 nbShapes = actors[i]->getNbShapes();
        PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
        actors[i]->getShapes(shapes, nbShapes);


        for (PxU32 j = 0; j < nbShapes; ++j)
        {
            const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
            PxGeometryHolder h = shapes[j]->getGeometry();
            //shapes[j]->getFlags()& PxShapeFlag::eTRIGGER_SHAPE;
            switch (h.getType())
            {
            case PxGeometryType::eBOX:

            {
                if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)    break;
                numofbox++;
                numofall++;
                if (actors[i]->is<PxRigidDynamic>())
                {
                    float x1 = 2 * h.box().halfExtents.x;
                    float x2 = 2 * h.box().halfExtents.y;
                    float x3 = 2 * h.box().halfExtents.z;
                    pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateBox(x1, x2, x3));
                    //pModels[i]->SetDebugObjectName("Box"+ numofbox);
                    m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\ice.dds", false, true);
                    pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\ice.dds");
                    pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
                    pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
                    pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
                    pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
                    pModels[i]->SetPhysicsData(actors[i]);
                    //PxTransform transform = pModels[i]->GetPhysicsData()->getGlobalPose();//更新位置信息
                    //XMFLOAT3 position(transform.p.x, transform.p.y, transform.p.z); 
                    GameObject box;
                    box.SetModel(pModels[i]);
                    m_Box.push_back(box);
                    UpdatePhysics(m_Box[numofbox]);
                    break;
                }
                else
                {
                    float x1 = 2 * h.box().halfExtents.x;
                    float x2 = 2 * h.box().halfExtents.y;
                    float x3 = 2 * h.box().halfExtents.z;
                    pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateBox(x1, x2, x3));
                    //pModels[i]->SetDebugObjectName("Box"+ numofbox);
                    m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\bricks.dds", false, true);
                    pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\bricks.dds");
                    m_TextureManager.CreateFromFile("..\\Texture\\bricks_nmap.dds");
                    pModels[i]->materials[0].Set<std::string>("$Normal", "..\\Texture\\bricks_nmap.dds");
                    pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
                    pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
                    pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
                    pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
                    pModels[i]->SetPhysicsData(actors[i]);
                    //PxTransform transform = pModels[i]->GetPhysicsData()->getGlobalPose();//更新位置信息
                    //XMFLOAT3 position(transform.p.x, transform.p.y, transform.p.z); 
                    GameObject box;
                    box.SetModel(pModels[i]);
                    m_Box.push_back(box);
                    UpdatePhysics(m_Box[numofbox]);
                }

            }
            case PxGeometryType::ePLANE:
            {   numofplane++;
            numofall++;
            pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreatePlane(XMFLOAT2(800.0f, 800.0f), XMFLOAT2(6.0f, 9.0f)));
            //pModels[i]->SetDebugObjectName("Ground"+numofplane);
            m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\floor.dds", false, true);
            pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\floor.dds");
            m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\floor_nmap.dds");
            pModels[i]->materials[0].Set<std::string>("$Normal", "..\\..\\src\\Texture\\floor_nmap.dds");
            pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
            pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
            pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
            pModels[i]->SetPhysicsData(actors[i]);
            GameObject plane;
            m_Plane.push_back(plane);
            m_Plane[numofplane].SetModel(pModels[i]);
            m_Plane[numofplane].GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
            break;
            }


            case PxGeometryType::eSPHERE:
            {   numofsphere++;
            numofall++;
            float radius1 = h.sphere().radius;
            pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateSphere(radius1));
            //pModels[i]->SetDebugObjectName("Sphere");
            m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\flare.dds", false, true);
            pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\flare.dds");
            pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
            pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
            pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
            pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
            GameObject sphere;
            m_Sphere.push_back(sphere);
            m_Sphere[numofsphere].SetModel(pModels[i]);
            pModels[i]->SetPhysicsData(actors[i]);
            UpdatePhysics(m_Sphere[numofsphere]);
            break;
            }
            case PxGeometryType::eTRIANGLEMESH:
            {   numofall++;
            numofTmesh++;
            const PxTriangleMeshGeometry& triGeom = h.triangleMesh();
            const PxTriangleMesh& mesh = *triGeom.triangleMesh;
            const PxVec3 scale = triGeom.scale.scale;
            //获取三角形的几何体相关属性，包括三角形网格的指针，缩放比例
            const PxU32 triangleCount = mesh.getNbTriangles();
            const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
            const void* indexBuffer = mesh.getTriangles();
            //获取三角形的数量和索引类型 16 或 32
            std::function<float(float, float)> heightFunc = [](float x, float z) -> float {

                return sinf(x) * cosf(z);
            };
            std::function<DirectX::XMFLOAT3(float, float)> normalFunc = [](float x, float z) -> DirectX::XMFLOAT3 {
                return DirectX::XMFLOAT3(-10 * sinf(x), 10.0f, -cosf(z));

            };
            std::function<DirectX::XMFLOAT4(float, float)> colorFunc = [](float x, float z) -> DirectX::XMFLOAT4 {
                return DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
            };

            pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateGrid(XMFLOAT2(76.0f, 76.0f), XMUINT2(10, 10), XMFLOAT2(1.0f, 1.0f), heightFunc, normalFunc, colorFunc));
            m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\water1.dds", false, true);
            pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\water1.dds");
            pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
            pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
            pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
            GameObject cplane;
            m_ChangePlane.push_back(cplane);
            pModels[i]->SetPhysicsData(actors[i]);
            m_ChangePlane[numofTmesh].SetModel(pModels[i]);
            PxTransform transform = m_ChangePlane[numofTmesh].GetModel()->GetPhysicsData()->getGlobalPose();//更新位置信息
            XMFLOAT3 position(transform.p.x + 30, transform.p.y + 20, transform.p.z + 37);
            m_ChangePlane[numofTmesh].GetTransform().SetPosition(position);
            break;
            }
            case PxGeometryType::eCAPSULE:
            {   numofcy++;
            numofall++;
            float radius2 = h.capsule().radius;
            float height = 2 * h.capsule().halfHeight;
            pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateCylinder(radius2, height));
            m_TextureManager.CreateFromFile("..\\Texture\\bricks.dds", false, true);
            pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\Texture\\bricks.dds");
            m_TextureManager.CreateFromFile("..\\Texture\\bricks_nmap.dds");
            pModels[i]->materials[0].Set<std::string>("$Normal", "..\\Texture\\bricks_nmap.dds");
            pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
            pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
            pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
            pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
            GameObject cylinder;
            m_Cylinder.push_back(cylinder);
            m_Cylinder[numofcy].SetModel(pModels[i]);
            pModels[i]->SetPhysicsData(actors[i]);
            UpdatePhysics(m_Cylinder[numofcy]);
            }
            default:
                break;


            }
        }


    }
    delete[] pModels;


}


//void GameApp::RenderArticulations(PxRigidActor** actors, const PxU32 numActors)
//{
//    int numofall = -1;
//    int numofbox = -1;
//    int numofsphere = -1;
//    int numofcy = -1;
//    Model** pModels = new Model * [numActors];
//    for (PxU32 i = 0; i < numActors; ++i) {
//        pModels[i] = nullptr;
//    }//初始化model指针数组
//    PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
//    for (int i = 0; i < numActors; i++)
//    {
//        const PxU32 nbShapes = actors[i]->getNbShapes();
//        PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
//        actors[i]->getShapes(shapes, nbShapes);
//
//
//        for (PxU32 j = 0; j < nbShapes; ++j)
//        {
//            //const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
//            PxGeometryHolder h = shapes[j]->getGeometry();
//            switch (h.getType())
//            {
//            case PxGeometryType::eSPHERE:
//            {   numofsphere++;
//                numofall++;
//            float radius1 = h.sphere().radius;
//            pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateSphere(radius1));
//            //pModels[i]->SetDebugObjectName("Sphere");
//            m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\flare.dds", false, true);
//            pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\flare.dds");
//            pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
//            pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
//            pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
//            pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
//            GameObject sphere;
//            m_SphereA.push_back(sphere);
//            m_SphereA[numofsphere].SetModel(pModels[i]);
//            pModels[i]->SetPhysicsData(actors[i]);
//            UpdatePhysics(m_SphereA[numofsphere]);
//            break;
//            }
//            case PxGeometryType::eCAPSULE:
//            {   numofcy++;
//                numofall++;
//                float radius2 =  h.capsule().radius;
//                float height = 2* h.capsule().halfHeight;
//                pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateCylinder(radius2, height));
//                m_TextureManager.CreateFromFile("..\\Texture\\bricks.dds", false, true);
//                pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\Texture\\bricks.dds");
//                m_TextureManager.CreateFromFile("..\\Texture\\bricks_nmap.dds");
//                pModels[i]->materials[0].Set<std::string>("$Normal", "..\\Texture\\bricks_nmap.dds");
//                pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
//                pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
//                GameObject cylinder;
//                m_Cylinder.push_back(cylinder);
//                m_Cylinder[numofcy].SetModel(pModels[i]);
//                //PxArticulationLink* link = dynamic_cast<PxArticulationLink*>(actors[i]);
//                //if (link != nullptr)
//                //{
//                 //pModels[i]->SetPhysicsData(actors[i]);
//                 //UpdatePhysics(m_Cylinder[numofcy]);
//                //}
//            }
//            }
//        }
//    }
//}


void createPhysics()
{

}

bool GameApp::InitResource()
{
    // ******************
    // 初始化摄像机
    //


    auto camera = std::make_shared<FirstPersonCamera>();
    m_pCamera = camera;

    camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
    camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
    camera->LookTo(XMFLOAT3(0.0f, 110.0f, 22.0f), XMFLOAT3(1.0f, -0.1f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
    m_CameraController.InitCamera(m_pCamera.get());

    // ******************
    // 初始化阴影贴图和特效
    m_pShadowMapTexture = std::make_unique<Depth2D>(m_pd3dDevice.Get(), 2048, 2048);
    m_pDebugShadowTexture = std::make_unique<Texture2D>(m_pd3dDevice.Get(), 2048, 2048, DXGI_FORMAT_R8G8B8A8_UNORM);

    m_pShadowMapTexture->SetDebugObjectName("ShadowMapTexture");
    m_pDebugShadowTexture->SetDebugObjectName("DebugShadowTexture");

    m_BasicEffect.SetDepthBias(0.005f);
    m_BasicEffect.SetViewMatrix(camera->GetViewMatrixXM());
    m_BasicEffect.SetProjMatrix(camera->GetProjMatrixXM());

    m_ShadowEffect.SetProjMatrix(XMMatrixOrthographicLH(150.0f, 150.0f, 20.0f, 60.0f));

    m_SkyboxEffect.SetViewMatrix(camera->GetViewMatrixXM());
    m_SkyboxEffect.SetProjMatrix(camera->GetProjMatrixXM());

    // ImGui 初始化
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    Font = io.Fonts->AddFontFromFileTTF("../../src/Resource/font/font.ttf", 30);
    Font2 = io.Fonts->AddFontFromFileTTF("../../src/Resource/font/font2.ttf", 200);

    // 背景图片初始化
    //loading_background_image = loadTexture2D("../../src/Resource/image/container.jpg");
    //main_menu_background_image = loadTexture2D("../../src/Resource/image/back.jpg");
    loading_background_image = (ImTextureID)loadImage("../../src/Resource/image/loading.jpg");
    main_menu_background_image = (ImTextureID)loadImage("../../src/Resource/image/background.jpg");

    // 背景音乐
    MCIERROR err = mciSendString(L"open ../../src/Resource/music/1.mp3 alias BGM", nullptr, 0, nullptr);
    if (err != 0) {
        std::cout << "Failed to open audio file." << std::endl;
    }
    err = mciSendString(L"play BGM", nullptr, 0, nullptr);


    // HP 初始化
    m_HP = 3;


    //渲染动态静态刚体
    PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
    std::vector<PxRigidActor*> actors(nbActors);
    gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
    RenderActors(&actors[0], static_cast<PxU32>(actors.size()));

    ////渲染连接体
    //PxU32 nbArticulations = gScene->getNbArticulations();
    //for (PxU32 i = 0; i < nbArticulations; i++)
    //{
    //    PxArticulation* articulation;
    //    gScene->getArticulations(&articulation, 1, i);
    //    const PxU32 nbLinks = articulation->getNbLinks();
    //    std::vector<PxArticulationLink*> links(nbLinks);
    //    articulation->getLinks(&links[0], nbLinks);
    //    //RenderArticulations(reinterpret_cast<PxRigidActor**>(&links[0]), static_cast<PxU32>(links.size()));     
    //}




    // 天空盒
    {
        Model* pModel = m_ModelManager.CreateFromGeometry("Skybox", Geometry::CreateBox(), false);
        pModel->SetDebugObjectName("Skybox");
        m_Skybox.SetModel(pModel);
        m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\Clouds.dds", false, true);
        pModel->materials[0].Set<std::string>("$Skybox", "..\\..\\src\\Texture\\Clouds.dds");
    }





    //******************
     //初始化光照

    m_DirLights[0].ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_DirLights[0].diffuse = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
    m_DirLights[0].specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    m_DirLights[0].direction = XMFLOAT3(5.0f / sqrtf(50.0f), -5.0f / sqrtf(50.0f), 0.0f);

    m_DirLights[1].ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_DirLights[1].diffuse = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
    m_DirLights[1].specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_DirLights[1].direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

    m_DirLights[2].ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_DirLights[2].diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_DirLights[2].specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_DirLights[2].direction = XMFLOAT3(0.0f, 0.0, -1.0f);

    for (int i = 0; i < 3; ++i)
    {
        m_OriginalLightDirs[i] = m_DirLights[i].direction;
        m_BasicEffect.SetDirLight(i, m_DirLights[i]);
    }
    return true;

}

void GameApp::createTerrainWeek3()
{

    //for (int i = 0; i <= 9; i++)
    //{
    //	createTerrain_Week3_StageOf(i);
    //}

    createTerrain_Week3_StageOf(0);
    createTerrain_Week3_StageOf(1);
    createTerrain_Week3_StageOf(2);
    createTerrain_Week3_StageOf(3);
    createTerrain_Week3_StageOf(4);
    createTerrain_Week3_StageOf(5);
    createTerrain_Week3_StageOf(6);
    createTerrain_Week3_StageOf(7);
    createTerrain_Week3_StageOf(8);
    //createTerrain_Week3_StageOf(9);


}

void GameApp::createTerrain_Week3_StageOf(const int i)
{
    switch (i)
    {
    case 0:createTerrain_Stage0_Week3(); break;
    case 1:createTerrain_Stage1_Week3(); break;
    case 2:createTerrain_Stage2_Week3(); break;
    case 3:createTerrain_Stage3_Week3(); break;
    case 4:createTerrain_Stage4_Week3(); break;
    case 5:createTerrain_Stage5_Week3(); break;
    case 6:createTerrain_Stage6_Week3(); break;
    case 7:createTerrain_Stage7_Week3(); break;
    case 8:createTerrain_Stage8_Week3(); break;
    case 9:createTerrain_Stage9_Week5(); break;

    }
}

void GameApp::createTerrain_Stage0_Week3()
{
    //创建初始平台,以及角色球
    stackY = 100.0f;
    ballActor = createBall(1.0, ballInitPos);
    //ballActor = createBall(1.0, PxVec3(0,1,0));
    bodiesQueue.push(createPlane(PxVec3(22, 100, 22), 7, 7, 1, 2.0f, PxVec3(1, 0, 0)));


    //第零关，没有障碍的路径
    stackX += 28.0f;
    stackZ += 4.0f;
    //createPlane(PxVec3(46, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0));
    bodiesQueue.push(createPlane(PxVec3(46, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
    //createPlane(PxVec3(66, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0));
    bodiesQueue.push(createPlane(PxVec3(66, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
}

PxRigidStatic* GameApp::createPlane(PxVec3 pos, PxU32 x, PxU32 y, PxU32 z, PxF32 size, PxVec3 rotation) {
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(x * size, y * size, z * size), *gMaterial);
    PxRigidStatic* body = gPhysics->createRigidStatic(PxTransform(pos, PxQuat(PxHalfPi, -rotation)));
    body->attachShape(*shape);
    body->userData = body;
    gScene->addActor(*body);
    return body;
}

void GameApp::createTerrain_Stage1_Week3()
{
    //第一关，有一个高度为2的凸起
    //第一关，有一个高度为2的凸起
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(86, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0));
    //createPlane(PxVec3(78, 104, 22), 1, 1, 5, 2.0f, PxVec3(0, 0, 1));
    bodiesQueue.push(createPlane(PxVec3(86, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    bodiesQueue.push(createPlane(PxVec3(78, 104, 22), 1, 1, 5, 2.0f, PxVec3(0, 0, 1)));
    stackY += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 8.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 24.0f;
    stackY -= 4.0f;
    stackZ -= 12.0f;

}

void GameApp::createTerrain_Stage2_Week3()
{
    //第二关，发射小球砸垮方块堆以继续前进
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(110, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0));
    bodiesQueue.push(createPlane(PxVec3(110, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(120, 105, 5), 3, 3, 1, 2.0f, PxVec3(1, 0, 0));
    //createPlane(PxVec3(130, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0));
    //bodiesQueue.push(createPlaneNew(PxVec3(120, 105, 5), 3, 3, 1, 2.0f, PxVec3(1, 0, 0)));
    bodiesQueue.push(createPlane(PxVec3(130, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackY += 4.0f;
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f, density_CrashStack));
    stackY += 20.0f;
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f, density_CrashStack));

    stackX += 20.0f;
    stackY -= 24.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(150, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0));
    bodiesQueue.push(createPlane(PxVec3(150, 100, 22), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
    stackZ -= 12.0f;
}

void GameApp::createTerrain_Stage3_Week3()
{
    //第三关：缝隙间有方块，玩家需要推开方块才能继续前进
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 11, 2.0f);
    //createPlane(PxVec3(182, 100, 22), 11, 1, 11, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(182, 100, 22), 11, 1, 11, 2.0f, PxVec3(0, 1, 0)));
    stackY += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(170, 108, 10), 5, 3, 5, 2.0f, PxVec3(0, 1, 0));
    //bodiesQueue.push(createPlane(PxVec3(170, 108, 10), 5, 3, 5, 2.0f, PxVec3(0, 1, 0)));
    stackX += 20.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY -= 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY -= 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(190, 108, 10), 5, 3, 5, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(180, 108, 10), 5, 3, 10, 2.0f, PxVec3(0, 1, 0)));
    stackZ += 24.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY += 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(190, 108, 34), 5, 3, 5, 2.0f, PxVec3(0, 1, 0));
    //bodiesQueue.push(createPlane(PxVec3(190, 108, 34), 5, 3, 5, 2.0f, PxVec3(0, 1, 0)));
    stackX -= 20.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY -= 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackY -= 4.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(170, 108, 34), 5, 3, 5, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(180, 108, 34), 5, 3, 10, 2.0f, PxVec3(0, 1, 0)));

    stackZ -= 4.0f;
    stackX += 4.0f;
    stackY -= 0.0f;

    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(166, 104, 22)), 1, 2.0f, density_PushStack));
    stackX += 40.0f;
    stackZ -= 0.0f;
    stackY -= 4.0f;
}

void GameApp::createTerrain_Stage4_Week3()
{
    //第四关，台阶
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(214, 100, 30), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(214, 100, 30), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackZ += 18.0f;
    //createStairsNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    for (int i = 0; i < 10; i++)
    {
        bodiesQueue.push(createPlane(PxVec3(214.0f, 104.0f + i * 4.0f, 40.0f + i * 10.0f), (PxU32)5.0, (PxU32)1.0, (PxU32)5.0, 2.0f, PxVec3(0.0f, 1.0f, 0.0f)));

    }

    stackZ += 50.0f;
    stackY += 20.0f;
    //createStairsNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    stackZ += 32.0f;
    stackX -= 20.0f;
    stackY += 20.0f;

}

void GameApp::createTerrain_Stage5_Week3()
{
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(194, 140, 130), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(194, 140, 130), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackZ += 20.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 3, 2.0f);
    //createPlane(PxVec3(194, 140, 146), 3, 1, 3, 2.0f, PxVec3(0, 1, 0));
    //createPlane(PxVec3(194, 140, 166), 3, 1, 3, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(194, 140, 146), 3, 1, 3, 2.0f, PxVec3(0, 1, 0)));
    bodiesQueue.push(createPlane(PxVec3(194, 140, 166), 3, 1, 3, 2.0f, PxVec3(0, 1, 0)));
    //第五关，摇摆的障碍物
    /*for (int i = 0; i < 4; i++)
    {
        createArticulation(PxVec3(184, 153, 145 + i * 10));
    }*/

    bodies_ChainJointQueue.push(createChain(PxTransform(PxVec3(188.0f, 175.0f, 155.0f)), 4, PxBoxGeometry(4.0f, 0.5f, 0.5f), PxBoxGeometry(4.0f, 4.0f, 4.0f), 8.0f, &createLimitedSpherical));
    bodies_ChainJointQueue.push(createChain(PxTransform(PxVec3(188.0f, 175.0f, 175.0f)), 4, PxBoxGeometry(4.0f, 0.5f, 0.5f), PxBoxGeometry(4.0f, 4.0f, 4.0f), 8.0f, &createLimitedSpherical));
    bodies_ChainJointQueue.push(createChain(PxTransform(PxVec3(198.0f, 175.0f, 195.0f)), 4, PxBoxGeometry(4.0f, 0.5f, 0.5f), PxBoxGeometry(4.0f, 4.0f, 4.0f), 8.0f, &createLimitedSpherical));

    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 3, 2.0f);
    stackZ += 20.0f;
    stackZ += 20.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(194, 140, 190), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(194, 140, 190), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackX -= 20.0f;

}

void GameApp::createTerrain_Stage6_Week3()
{
    //第六关，十字路口，障碍方块阻拦
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(174, 140, 190), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    //bodiesQueue.push(createPlane(PxVec3(174, 140, 190), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    bodiesQueue.push(createPlane(PxVec3(174, 140, 200), 10, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    bodiesQueue.push(createPlane(PxVec3(194, 138, 190), 1, 1, 1, 2.0f, PxVec3(0, 1, 0)));
   
    stackY += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 8.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX -= 8.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 8.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 8.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackY += 8.0f;
    stackZ -= 4.0f;
    //createStackNew(PxTransform(PxVec3(174, 145, 190)), 1, 2.0f, density_PushStack);
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(174, 145, 190)), 1, 2.0f, density_PushStack));

    stackX += -4.0f;
    stackZ += 4.0f;
    //createStackNew(PxTransform(PxVec3(170, 145, 194)), 1, 2.0f, density_PushStack);
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(170, 145, 194)), 1, 2.0f, density_PushStack));

    stackX += -4.0f;
    stackZ += 8.0f;
    stackY += -12.0f;

    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(174, 140, 210), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    //bodiesQueue.push(createPlane(PxVec3(174, 140, 210), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));

    stackY += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 8.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackX += 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    stackZ -= 4.0f;
    //createCuboidNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f);
    //createPlane(PxVec3(174, 146, 184), 5, 2, 2, 2.0f, PxVec3(1, 0, 0));
    //createPlane(PxVec3(166, 146, 206), 1, 7, 2, 2.0f, PxVec3(1, 0, 0));
    //createPlane(PxVec3(178, 146, 206), 3, 7, 2, 2.0f, PxVec3(1, 0, 0));
    bodiesQueue.push(createPlane(PxVec3(174, 146, 184), 5, 2, 2, 2.0f, PxVec3(1, 0, 0)));
    bodiesQueue.push(createPlane(PxVec3(166, 146, 206), 1, 7, 2, 2.0f, PxVec3(1, 0, 0)));
    bodiesQueue.push(createPlane(PxVec3(178, 146, 206), 3, 7, 2, 2.0f, PxVec3(1, 0, 0)));
    stackZ += 20.0f;
    stackY += -4.0f;
    stackX += -16.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(174, 140, 230), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));

    stackZ += 20.0f;
}

void GameApp::createTerrain_Stage7_Week3()
{
    //第7关，创建弹簧平面
    bodiesQueue.push(createPlane(PxVec3(174, 140, 230), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    bodies_KinematicPlatformQueue.push(createKinematicPlatform3(PxVec3(174, 138, 230), 0.1f, 0.1f, 0.1f));
    bodies_RigidStaticQueue.push(createDeformablePlatform(PxVec3(stackX - 24.0f, stackY - 20.0f, 220.0f)));


    stackZ += 24.0f;
    stackX -= 42.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f);
    //createPlane(PxVec3(132, 140, 274), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    stackX += -20.0f;
}

void GameApp::createTerrain_Stage8_Week3()
{
    //第8关，上下移动的平面
    bodies_KinematicPlatformQueue.push(createKinematicPlatform(PxVec3(stackX + 15.0f, stackY, stackZ - 10.0f)));
    //planeActor = createKinematicPlatform(PxVec3(stackX, stackY, stackZ - 10.0f));
    stackX += -20.0f;
    stackZ += -12.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY + 18.0f, stackZ)), 11, 2.0f);
    //createPlane(PxVec3(80, 158, 274), 11, 1, 11, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlane(PxVec3(80, 158, 274), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackX += -20.0f;
    stackZ += 12.0f;

}

void GameApp::createTerrain_Stage9_Week5() {
    //createRoadNew(PxTransform(PxVec3(-100.0f, 130.0f, 22.0f)), 5, 2.0f);
    createPlane(PxVec3(-92, 130, 22), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    planeActor2 = createKinematicPlatform2(PxVec3(-50.0f, 132.0f, 22.0f));
    //createRoadNew(PxTransform(PxVec3(-20.0f, 130.0f, 22.0f)), 5, 2.0f);
    createPlane(PxVec3(-12, 130, 22), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
}

PxRigidDynamic* GameApp::createKinematicPlatform2(PxVec3 location)
{

    PxShape* shape = gPhysics->createShape(PxBoxGeometry(5.0f, 1.0f, 5.0f), *gMaterial);
    PxRigidDynamic* actor = PxCreateKinematic(*gPhysics, PxTransform(location), *shape, 2.0f);

    //PxTransform planeTarget(location, PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));

    gScene->addActor(*actor);
    //actor->setKinematicTarget(PxTransform(location));

    shape->release();
    return actor;
}

PxRigidDynamic* GameApp::createKinematicPlatform(PxVec3 location)
{

    PxShape* shape = gPhysics->createShape(PxBoxGeometry(10.0f, 1.0f, 10.0f), *gMaterial);
    PxRigidDynamic* actor = PxCreateKinematic(*gPhysics, PxTransform(PxVec3(location.x + 8.0f, location.y, location.z + 8.0f)), *shape, 2.0f);

    PxTransform planeTarget(PxVec3(stackX + 20.0f, location.y, stackZ), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));

    gScene->addActor(*actor);
    actor->setKinematicTarget(planeTarget);

    shape->release();
    return actor;
}
PxRigidDynamic* GameApp::createKinematicPlatform3(PxVec3 location, PxReal x, PxReal y, PxReal z)
{

    PxShape* shape = gPhysics->createShape(PxBoxGeometry(x, y, z), *gMaterial);
    PxRigidDynamic* actor = PxCreateKinematic(*gPhysics, PxTransform(PxVec3(location.x + 8.0f, location.y, location.z + 8.0f)), *shape, 2.0f);

    PxTransform planeTarget(PxVec3(stackX + 20.0f, location.y, stackZ), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));

    gScene->addActor(*actor);
    actor->setKinematicTarget(planeTarget);

    shape->release();
    return actor;
}

void GameApp::updateKinematicPlatform2(PxVec3* verts)
{
    PxReal height = sinf(gTime * speed) * (maxX - minX) * 0.5f + (maxX + minX) * 0.5f;
    PxTransform planeTarget(PxVec3(height, verts->y, verts->z), PxQuat(PxHalfPi, PxVec3(0, 1.0f, 0)));
    planeActor2->setKinematicTarget(planeTarget);
}

void GameApp::updateKinematicPlatform(PxVec3* verts, PxRigidDynamic* n_planeActor)
{
    PxReal height = cosf(gTime * speed) * (maxHeight - minHeight) * 0.5f + (maxHeight + minHeight) * 0.5f;
    PxTransform planeTarget(PxVec3(verts->x, height, verts->z), PxQuat(PxHalfPi, PxVec3(0, 1.0f, 0)));
    n_planeActor->setKinematicTarget(planeTarget);
}

PxRigidStatic* GameApp::createPlaneNew(PxVec3 pos, PxU32 x, PxU32 y, PxU32 h, PxF32 size, PxVec3 rotation)
{
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(x * size, y * size, h * size), *gMaterial);
    PxRigidStatic* body = gPhysics->createRigidStatic(PxTransform(PxVec3(pos.x + size * (x - 1), pos.y, pos.z + size * (y - 1)), PxQuat(PxHalfPi, rotation)));

    std::cout << "X: " << pos.x + size * (x - 1) << " Y: " << pos.y << " Z: " << pos.z + size * (y - 1) << std::endl;

    body->attachShape(*shape);
    body->userData = body;
    gScene->addActor(*body);
    return body;
}

bool GameApp::initGame() {

    /*setTerrainSeed();
    for (int i = 0; i < 9; i++)
    {
        DeleteTextOfTerrain[i] = i;
    }
    for (int i = 9; i < 59; i++)
    {
        DeleteTextOfTerrain[i] = CreateTextOfTerrain[i - 9];
    }*/

    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    gScene->addActor(*groundPlane);
    createTrigger();
    createTerrainWeek3();
    return true;

}

void changeBallPos(PxVec3 pos) {
    ballActor->clearForce(PxForceMode::eVELOCITY_CHANGE);
    ballActor->setAngularVelocity(PxVec3(0, 0, 0));
    ballActor->setLinearVelocity(PxVec3(0, 0, 0));
    ballActor->setGlobalPose(PxTransform(pos));
}

PxRigidDynamic* GameApp::createBall(PxReal r, PxVec3 location) {
    //ballMaterial = gPhysics->createMaterial(100.0f, 100.0f, 0.6f);
    PxShape* shape = gPhysics->createShape(PxSphereGeometry(r), *gMaterial);
    PxRigidDynamic* dynamic = gPhysics->createRigidDynamic(PxTransform(location));
    setupFiltering(dynamic, FilterGroup::eBall, FilterGroup::eLanuchBall);
    dynamic->attachShape(*shape);
    dynamic->setAngularDamping(10.0f);
    dynamic->setLinearVelocity(PxVec3(0, 0, 0));
    dynamic->setMass(30.0f);
    gScene->addActor(*dynamic);
    return dynamic;
}

PxRigidDynamic* GameApp::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
    currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastLaunchTime);
    PxRigidDynamic* dynamic = NULL;
    if (elapsed >= launchInterval) {
        dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
        dynamic->setAngularDamping(0.5f);
        dynamic->setLinearVelocity(velocity);
        setupFiltering(dynamic, FilterGroup::eLanuchBall, FilterGroup::eBall | FilterGroup::eLanuchBall);
        gScene->addActor(*dynamic);
        lastLaunchTime = currentTime;
    }

    return dynamic;
}

XMVECTOR getBallPos() {
    PxVec3 ballPos = ballActor->getGlobalPose().p;
    XMFLOAT3 pos(ballPos.x, ballPos.y, ballPos.z);
    XMVECTOR vec = XMLoadFloat3(&pos);
    return vec;
}
PxVec3 getLatestPlanePos() {
    PxRigidStatic* plane = bodiesQueue.front();
    PxVec3 planePos = plane->getGlobalPose().p;
    PxVec3 pos = PxVec3(planePos.x, planePos.y + 3.0f, planePos.z);
    return pos;
}

void GameApp::setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
    PxFilterData filterData;
    filterData.word0 = filterGroup; // word0 = own ID
    filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
    const PxU32 numShapes = actor->getNbShapes();
    PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*) * numShapes);
    actor->getShapes(shapes, numShapes);
    for (PxU32 i = 0; i < numShapes; i++)
    {
        PxShape* shape = shapes[i];
        shape->setQueryFilterData(filterData);
        shape->setSimulationFilterData(filterData);
    }
    free(shapes);

}

PxRigidDynamic** GameApp::createStackNew(const PxTransform& t, PxU32 size, PxReal halfExtent, PxReal Density)
{
    //createShape构建形状;(halfExtent x,y,z)
    PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);


    PxRigidDynamic** bodies_dynamic = new PxRigidDynamic * [size * size];

    for (PxU32 i = 0; i < size; i++)
    {
        for (PxU32 j = 0; j < size; j++)
        {
            //指定位置
            PxTransform localTm(PxVec3(0, PxReal(j * 2), PxReal(i * 2)) * halfExtent);
            //createRigidStatic构建刚体
            PxRigidDynamic* body_dynamic = gPhysics->createRigidDynamic(t.transform(localTm));
            setupFiltering(body_dynamic, FilterGroup::eBox, FilterGroup::eLanuchBall);
            //attachShape绑定形状到刚体上;
            body_dynamic->attachShape(*shape);
            //更新质量和惯性（数值表示密度）
            PxRigidBodyExt::updateMassAndInertia(*body_dynamic, Density);

            body_dynamic->userData = body_dynamic;

            gScene->addActor(*body_dynamic);

            bodies_dynamic[i * size + j] = body_dynamic;
        }
    }

    //释放
    shape->release();

    return bodies_dynamic;
}

struct Triangle
{
    PxU32 ind0, ind1, ind2;
};

PxTriangleMesh* GameApp::createMeshGround()
{
    const PxU32 gridSize = gGridSize;

    PxVec3 verts[gridSize * gridSize];

    const PxU32 nbTriangles = 2 * (gridSize - 1) * (gridSize - 1);

    Triangle indices[nbTriangles];

    updateVertices(verts);

    for (PxU32 a = 0; a < (gridSize - 1); ++a)
    {
        for (PxU32 b = 0; b < (gridSize - 1); ++b)
        {
            Triangle& tri0 = indices[(a * (gridSize - 1) + b) * 2];
            Triangle& tri1 = indices[((a * (gridSize - 1) + b) * 2) + 1];

            tri0.ind0 = a * gridSize + b + 1;
            tri0.ind1 = a * gridSize + b;
            tri0.ind2 = (a + 1) * gridSize + b + 1;

            tri1.ind0 = (a + 1) * gridSize + b + 1;
            tri1.ind1 = a * gridSize + b;
            tri1.ind2 = (a + 1) * gridSize + b;
        }
    }

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.data = verts;
    meshDesc.points.count = gridSize * gridSize;
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.triangles.count = nbTriangles;
    meshDesc.triangles.data = indices;
    meshDesc.triangles.stride = sizeof(Triangle);

    PxTriangleMesh* triMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());

    return triMesh;
}

void GameApp::updateVertices(PxVec3* verts, float amplitude)
{
    const PxU32 gridSize = gGridSize;
    const PxReal gridStep = gGridStep;

    for (PxU32 a = 0; a < gridSize; a++)
    {
        const float coeffA = float(a) / float(gridSize);
        for (PxU32 b = 0; b < gridSize; b++)
        {
            const float coeffB = float(b) / float(gridSize);

            const float y = 20.0f + sinf(coeffA * PxTwoPi) * cosf(coeffB * PxTwoPi) * amplitude;
            //地面网格的中心点在世界坐标系下的位置为(location.x , location.y + 20.0f, location.z )
            verts[a * gridSize + b] = PxVec3(b * gridStep, y, a * gridStep);
        }
    }
}

PxRigidStatic* GameApp::createDeformablePlatform(PxVec3 location) {
    PxTriangleMesh* mesh = createMeshGround();
    platformMesh = mesh;

    PxTriangleMeshGeometry geom(mesh);

    PxRigidStatic* groundMesh = gPhysics->createRigidStatic(PxTransform(location));
    platformActor = groundMesh;
    groundMesh->createShape(geom, *gMaterial);

    {
        PxShape* shape;
        groundMesh->getShapes(&shape, 1);
        shape->setContactOffset(0.02f);
        // A negative rest offset helps to avoid jittering when the deformed mesh moves away from objects resting on it.
        shape->setRestOffset(-0.5f);
    }

    gScene->addActor(*groundMesh);
    return groundMesh;
}

void GameApp::createArticulation(PxVec3 location)
{

    // 创建一个关节对象
    gArticulation = gPhysics->createArticulation();

    // 禁用稳定化以避免关节物体产生伪影
    gArticulation->setStabilizationThreshold(0.0f);

    // 设置最大投影迭代次数和分离容差
    gArticulation->setMaxProjectionIterations(16);
    gArticulation->setSeparationTolerance(0.001f);

    // 定义一些常量
    const float scale = 0.25f;
    const float radius = 0.5f;
    const float halfHeight = 1.0f;
    const PxU32 nbCapsules = 40;
    const float capsuleMass = 1.0f;

    // 在给定位置创建一个胶囊形状
    PxVec3 pos = location;
    PxShape* capsuleShape = gPhysics->createShape(PxCapsuleGeometry(radius, halfHeight), *gMaterial);

    // 初始化第一个链接和父链接
    PxArticulationLink* firstLink = NULL;
    PxArticulationLink* parent = NULL;

    // 设置是否重叠链接
    const bool overlappingLinks = true;	// Change this for another kind of rope

    // 创建绳索
    for (PxU32 i = 0; i < nbCapsules; i++)
    {
        // 创建一个链接并将其附加到胶囊形状上
        PxArticulationLink* link = gArticulation->createLink(parent, PxTransform(pos));
        if (!firstLink)
            firstLink = link;

        link->attachShape(*capsuleShape);

        // 设置链接的质量并更新惯性
        PxRigidBodyExt::setMassAndUpdateInertia(*link, capsuleMass);

        // 获取链接的入站关节
        PxArticulationJoint* joint = link->getInboundJoint();

        // 如果关节不为空（对于根链接将为空）
        if (joint)
        {
            if (overlappingLinks)
            {
                joint->setParentPose(PxTransform(PxVec3(halfHeight, 0.0f, 0.0f)));
                joint->setChildPose(PxTransform(PxVec3(-halfHeight, 0.0f, 0.0f)));
            }
            else
            {
                joint->setParentPose(PxTransform(PxVec3(radius + halfHeight, 0.0f, 0.0f)));
                joint->setChildPose(PxTransform(PxVec3(-radius - halfHeight, 0.0f, 0.0f)));
            }
        }

        if (overlappingLinks)
            pos.x += (radius + halfHeight * 2.0f);
        else
            pos.x += (radius + halfHeight) * 2.0f;

        // 将父链接设置为当前链接
        parent = link;
    }

    // 在绳索末端附加一个大且重的盒子
    {
        // 定义盒子的质量和大小
        const float boxMass = 150.0f;
        const float boxSize = 1.2f;

        // 创建一个盒子形状
        PxShape* boxShape = gPhysics->createShape(PxBoxGeometry(boxSize, boxSize, boxSize), *gMaterial);

        // 更新位置
        pos.x -= (radius + halfHeight) * 2.0f;
        pos.x += (radius + halfHeight) + boxSize;

        // 创建一个链接并将其附加到盒子形状上
        PxArticulationLink* link = gArticulation->createLink(parent, PxTransform(pos));

        link->attachShape(*boxShape);

        // 设置链接的质量并更新惯性
        PxRigidBodyExt::setMassAndUpdateInertia(*link, boxMass);

        // 获取链接的入站关节
        PxArticulationJoint* joint = link->getInboundJoint();

        // 如果关节不为空（对于根链接将为空）
        if (joint)
        {
            joint->setParentPose(PxTransform(PxVec3(radius + halfHeight, 0.0f, 0.0f)));
            joint->setChildPose(PxTransform(PxVec3(-boxSize, 0.0f, 0.0f)));
        }
    }

    // 将关节添加到场景中
    gScene->addArticulation(*gArticulation);

    // 将关节附加到静态世界上
    {
        // 创建一个锚点形状
        PxShape* anchorShape = gPhysics->createShape(PxSphereGeometry(0.05f), *gMaterial);

        // 创建一个静态锚点
        PxRigidStatic* anchor = PxCreateStatic(*gPhysics, PxTransform(location), *anchorShape);

        // 将锚点添加到场景中
        gScene->addActor(*anchor);

        // 创建一个球形关节并将其连接到第一个链接上
        PxSphericalJoint* j = PxSphericalJointCreate(*gPhysics, anchor, PxTransform(PxVec3(0.0f)), firstLink, PxTransform(PxVec3(0.0f)));

        // 不使用变量j，防止编译器警告
        PX_UNUSED(j);
    }

    // 创建障碍物
    {
        // 创建一个盒子形状
        PxShape* boxShape = gPhysics->createShape(PxBoxGeometry(1.0f, 0.1f, 2.0f), *gMaterial);

        // 创建一个静态障碍物并将其添加到场景中
        PxRigidStatic* obstacle = PxCreateStatic(*gPhysics, PxTransform(location + PxVec3(10.0f, -3.0f, 0.0f)), *boxShape);

        gScene->addActor(*obstacle);
    }

}

void GameApp::createTerrain_Seed()
{
    //if (addTime >= 10.09f && !bodiesQueue.empty())
    if (addTime >= 12.00f)
    {
        if (iText == 0)
        {
            stackX = 104.0f;
            stackY = 158.0f;
            stackZ = 274.0f;
        }
        createTerrain_Week5_StageOf(CreateTextOfTerrain[iText]);

        if (CreateTextOfTerrain[iText] == 4 || CreateTextOfTerrain[iText] == 5)
            stackY += 4.0f;
        if (CreateTextOfTerrain[iText] == 5 && CreateTextOfTerrain[iText + 1] <= 4)
        {
            stackX += 20.0f; stackY -= 4.0f; stackZ += 5.0f;
        }
        else if (CreateTextOfTerrain[iText + 1] == 5)
        {
            stackY -= 4.0f; stackZ += 10.0f;
        }
        else if (CreateTextOfTerrain[iText] == 6 && CreateTextOfTerrain[iText + 1] <= 4)
        {
            stackY -= 8.0f; stackZ -= 15.0f;
        }



        //stackY += 4.0f;
        //deleteTerrain_Week5_StageOf(textOfTerrain[iText]);

        iText = iText + 1;
        addTime = 0.0f;
    }
    else if (delTime >= 20.0f)
    {

        if (iText2 >= 0)
        {

            deleteTerrain_Week5_StageOf(DeleteTextOfTerrain[iText2]);
            iText2 = iText2 + 1;
            addTime = 0.0f;
            delTime = 0.0f;
        }
    }



}

void GameApp::createTerrain_Week5_StageOf(const int i)
{
    switch (i)
    {
    case 0:createTerrain_Stage0_Week5(); break;
    case 1:createTerrain_Stage1_Week5(); break;
    case 2:createTerrain_Stage2_Week5(); break;
    case 3:createTerrain_Stage3_Week5(); break;
    case 4:createTerrain_Stage4_Week5(); break;
    case 5:createTerrain_Stage5_Week5(); break;
    case 6:createTerrain_Stage6_Week5(); break;
    case 7:createTerrain_Stage8_Week5(); break;
    case 8:createTerrain_Stage8_Week5(); break;
    }

}

void GameApp::createTerrain_Stage0_Week5()
{
    //创建初始平台,以及角色球
    bool direction = 0;

    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 7, 7, 1, 2.0f, PxVec3(1, 0, 0)));

    //createPlane(PxVec3(stackX, stackY, stackZ), 1, 10, 1, 2.0f, PxVec3(0, 1, 0));

    //createPlaneNew(PxVec3(stackX, stackY, stackZ), 7, 7, 1, 2.0f, PxVec3(1, 0, 0));
    //第零关，没有障碍的路径
    stackX += 28.0f;
    stackZ += 4.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
    //std::cout << "sizeof bodiesQueue: " << bodiesQueue.size() << std::endl;
}

void GameApp::createTerrain_Stage1_Week5()
{
    //第一关，有一个高度为2的凸起
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackY += 4.0f;
    stackZ += 8.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 1, 1, 5, 2.0f, PxVec3(0, 0, 1)));
    stackZ += -8.0f;
    stackX += 24.0f;
    stackY -= 4.0f;

}

void GameApp::createTerrain_Stage2_Week5()
{
    //第二关，发射小球砸垮方块堆以继续前进
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackY += 4.0f;
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f, density_CrashStack));
    stackY += 20.0f;
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 5, 2.0f, density_CrashStack));
    stackX += 20.0f;
    stackY -= 24.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX += 20.0f;
    stackZ += 8.0f;
}

void GameApp::createTerrain_Stage3_Week5()
{
    //第三关：缝隙间有方块，玩家需要推开方块才能继续前进
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 11, 1, 11, 2.0f, PxVec3(0, 1, 0)));
    stackY += 8.0f;
    stackZ += 4.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 10, 5, 3, 2.0f, PxVec3(1, 0, 0)));
    stackZ += -24.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 10, 5, 3, 2.0f, PxVec3(1, 0, 0)));
    stackZ += 20.0f;
    stackY -= 4.0f;
    stackX += 4.0f;
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f, density_PushStack));
    stackX += 40.0f;
    stackY -= 4.0f;
}

void GameApp::createTerrain_Stage4_Week5()
{
    //第四关，台阶
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackZ += 20.0f;
    createStairsNew(PxVec3(stackX, stackY, stackZ), 5, 2, 10, 2.0f, PxVec3(1, 0, 0));
    stackY += 40.0f;
    stackZ += 80.0f;
}

//创建关卡，第5关
void GameApp::createTerrain_Stage5_Week5()
{
    //第五关，摇摆的障碍物
    bodiesQueue.push(createPlaneNew(PxVec3(stackX - 16.0f, stackY, stackZ), 3, 4, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX -= 16.0f;
    stackY -= 4.0f;
    stackZ += 20.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 3, 1, 2.0f, PxVec3(1, 0, 0)));
    stackZ += 20.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 3, 1, 2.0f, PxVec3(1, 0, 0)));
    for (int i = 0; i < 3; i++)
    {
        //bodies_ArticulationQueue.push(createArticulation(PxVec3(stackX - 10.0f, stackY + 18.0f, stackZ - 16.0f + i * 10)));
        //createChain(PxTransform(PxVec3(stackX - 10, stackY + 40 - (bool)i * 5, stackZ -25 + i * 15)), 4, PxBoxGeometry(4.0f, 0.5f, 0.5f), PxBoxGeometry(4.0f, 4.0f, 4.0f), 8.0f, &createLimitedSpherical);
        bodies_ChainJointQueue.push(createChain(PxTransform(PxVec3(stackX, stackY + 40 - (bool)i * 5, stackZ - 25 + i * 15)), 4, PxBoxGeometry(4.0f, 0.5f, 0.5f), PxBoxGeometry(4.0f, 4.0f, 4.0f), 8.0f, &createLimitedSpherical));
    }

    stackX += 8.0f;
    stackY += 4.0f;
    stackZ += 18.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 2, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX -= 16.0f;

}

void GameApp::createTerrain_Stage6_Week5()
{
    //第六关，十字路口，障碍方块阻拦
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 3, 5, 1, 2.0f, PxVec3(1, 0, 0)));
    stackX -= 20.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 11, 1, 2.0f, PxVec3(1, 0, 0)));
    stackY += 6.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 2, 2, 2.0f, PxVec3(1, 0, 0)));
    stackX += 8.0f;
    stackY += 2.0f;
    stackZ += 10.0f;
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f, density_PushStack));
    stackX += -4.0f;
    stackZ += 4.0f;
    bodies_dynamicQueue.push(createStackNew(PxTransform(PxVec3(stackX, stackY, stackZ)), 1, 2.0f, density_PushStack));
    stackX += -4.0f;
    stackY -= 2.0f;
    stackZ -= 2.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 1, 7, 2, 2.0f, PxVec3(1, 0, 0)));
    stackX += 8.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 3, 7, 2, 2.0f, PxVec3(1, 0, 0)));
    stackX -= 8.0f;
    stackY -= 6.0f;
    stackZ += 44.0f;

}

void GameApp::createTerrain_Stage7_Week5()
{
    //第7关，创建弹簧平面
    //createDeformablePlatform(PxVec3(stackX - 24.0f, stackY - 20.0f, stackZ));
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackX -= 20.0f;
    stackZ += 20.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackZ += 20.0f;
    bodies_RigidStaticQueue.push(createDeformablePlatform(PxVec3(stackX - 24.0f, stackY - 20.0f, stackZ)));
    stackX -= 42.0f;
    stackZ += 76.0f;

    //std::cout << " stackX:" << stackX << " stackY:" << stackY << " stackZ:" << stackZ << std::endl;
    //createPlane(PxVec3(132, 140, 274), 5, 1, 5, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));

    stackX -= 20.0f;
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackX -= 20.0f;
    //stackZ -= 8.0f;
}

void GameApp::createTerrain_Stage8_Week5()
{
    //第8关，上下移动的平面
    maxHeight = stackY + 12.0f;
    minHeight = stackY - 12.0f;
    //planeActor = createKinematicPlatform(PxVec3(stackX, stackY, stackZ - 8.0f));
    bodies_KinematicPlatformQueue.push(createKinematicPlatform(PxVec3(stackX, stackY, stackZ - 8.0f)));
    stackX += -20.0f;
    stackZ += -12.0f;

    stackZ += 12.0f;
    //createRoadNew(PxTransform(PxVec3(stackX, stackY + 18.0f, stackZ)), 11, 2.0f);
    //createPlane(PxVec3(80, 158, 274), 11, 1, 11, 2.0f, PxVec3(0, 1, 0));
    //createPlaneNew(PxVec3(stackX, stackY, stackZ), 11, 1, 11, 2.0f, PxVec3(0, 1, 0));
    bodiesQueue.push(createPlaneNew(PxVec3(stackX, stackY, stackZ), 5, 1, 5, 2.0f, PxVec3(0, 1, 0)));
    stackZ += -12.0f;


    stackX += -20.0f;
    stackZ += 12.0f;

}

void GameApp::createStairsNew(PxVec3 pos, PxU32 x, PxU32 y, PxU32 h, PxF32 size, PxVec3 rotation)
{
    for (PxU32 i = 0; i < h; i++)
    {
        //stairs[i] = createPlaneNew(PxVec3(pos.x, pos.y + (i+1) * size * 2, pos.z+ i * size * size * y), x, y, 1, size, PxVec3(1,0,0));
        bodiesQueue.push(createPlaneNew(PxVec3(pos.x, pos.y + (i + 1) * size * 2, pos.z + i * size * size * y), x, y, 1, size, PxVec3(1, 0, 0)));
    }
}

void GameApp::deleteTerrain_Week5_StageOf(const int Stage)
{
    int deleteNum = 0;
    int deleteDynamic = 0;
    switch (Stage)
    {
    case 0:deleteNum = 3; break;
    case 1:deleteNum = 2; break;
    case 2:deleteNum = 3; deleteTerrain_Dynamic(bodies_dynamicQueue, (PxU32)5.0f); deleteTerrain_Dynamic(bodies_dynamicQueue, (PxU32)5.0f); break;
    case 3:deleteNum = 3; deleteTerrain_Dynamic(bodies_dynamicQueue, (PxU32)1.0f); break;
    case 4:deleteNum = 11; break;
    case 5:deleteNum = 4; deleteTerrain_Chain(bodies_ChainJointQueue, bodies_ChainDynamicQueue); deleteTerrain_Chain(bodies_ChainJointQueue, bodies_ChainDynamicQueue); deleteTerrain_Chain(bodies_ChainJointQueue, bodies_ChainDynamicQueue); break;//deleteTerrain_Articulation(bodies_ArticulationQueue); deleteTerrain_Articulation(bodies_ArticulationQueue); deleteTerrain_Articulation(bodies_ArticulationQueue); deleteTerrain_Articulation(bodies_ArticulationQueue); break;
    case 6:deleteNum = 5; deleteTerrain_Dynamic(bodies_dynamicQueue, (PxU32)1.0f); deleteTerrain_Dynamic(bodies_dynamicQueue, (PxU32)1.0f); break;
        //case 7:deleteNum = 4; deleteTerrain_DeformablePlatform(bodies_RigidStaticQueue); break;
    case 7:deleteNum = 1; deleteTerrain_KinematicPlatform(bodies_KinematicPlatformQueue); break;//deleteTerrain_DeformablePlatform(bodies_RigidStaticQueue); 
    case 8:deleteNum = 1; deleteTerrain_KinematicPlatform(bodies_KinematicPlatformQueue); break;
    }

    for (int i = 0; i < deleteNum; i++)
    {
        deleteTerrain(bodiesQueue);
    }
}

//删除动态方块堆，接受传入的队列
void GameApp::deleteTerrain_Dynamic(std::queue<PxRigidDynamic**>& n_bodies_dynamicQueue, PxU32 size)
{
    if (!n_bodies_dynamicQueue.empty())
    {
        PxRigidDynamic** n_bodies_dynamic = n_bodies_dynamicQueue.front();
        for (PxU32 i = 0; i < size * size; i++)
        {
            gScene->removeActor(*n_bodies_dynamic[i]);
            n_bodies_dynamic[i]->release();
        }
        delete[] n_bodies_dynamic;
        n_bodies_dynamicQueue.pop();
    }
}

void GameApp::deleteTerrain_Dynamic(PxU32 size)
{
    for (PxU32 i = 0; i < size * size; i++)
    {
        gScene->removeActor(*bodies_dynamic[i]);
        bodies_dynamic[i]->release();
    }
    delete[] bodies_dynamic;
    bodies_dynamic = nullptr;

}

void GameApp::deleteTerrain_DeformablePlatform(std::queue<PxRigidStatic*>& n_bodies_RigidStaticQueue)
{
    if (!n_bodies_RigidStaticQueue.empty())
    {
        PxRigidStatic* n_bodies = n_bodies_RigidStaticQueue.front();
        gScene->removeActor(*n_bodies);
        n_bodies->release();
        n_bodies_RigidStaticQueue.pop();
    }
}

void GameApp::deleteTerrain_KinematicPlatform(std::queue<PxRigidDynamic*>& n_bodies_KinematicPlatformQueue)
{
    if (!n_bodies_KinematicPlatformQueue.empty())
    {
        PxRigidDynamic* n_bodies = n_bodies_KinematicPlatformQueue.front();
        gScene->removeActor(*n_bodies);
        n_bodies->release();
        n_bodies_KinematicPlatformQueue.pop();
    }
}

//删除静态地形
void GameApp::deleteTerrain(std::queue<PxRigidStatic*>& n_bodiesQueue)
{
    if (!n_bodiesQueue.empty())
    {

        PxRigidStatic* n_bodies = n_bodiesQueue.front();
        gScene->removeActor(*n_bodies);
        n_bodies->release();
        n_bodiesQueue.pop();
    }
}

// spherical joint limited to an angle of at most pi/4 radians (45 degrees)
PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
    PxSphericalJoint* j = PxSphericalJointCreate(*gPhysics, a0, t0, a1, t1);
    j->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 3, 0.05f));
    j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
    j->setProjectionLinearTolerance(0.8f);
    j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
    return j;
}

PxJoint** createChain(const PxTransform& t, PxU32 length, const PxGeometry& g1, const PxGeometry& g2, PxReal separation, JointCreateFunction createJoint)
{
    // offset，表示链接之间的偏移量
    PxVec3 offset(separation / 2, 0, 0);
    // localTm，表示当前链接的位置
    PxTransform localTm(offset);
    // prev，用于存储上一个链接
    PxRigidDynamic* prev = NULL;
    //存储所有创建出的关节对象
    PxJoint** joints = new PxJoint * [length - 1];
    PxU32 i;
    for (i = 0; i < length - 1; i++)
    {
        // 使用PxCreateDynamic函数创建一个新的动态刚体
        PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t * localTm, g1, *gMaterial, 1.0f);

        bodies_ChainDynamicQueue.push(current);

        // 使用createJoint函数创建链接之间的关节，并将其存储到joints数组中
        joints[i] = (*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
        // 将新创建的动态刚体添加到场景中
        gScene->addActor(*current);
        // 更新prev指针
        prev = current;
        // 更新localTm变量，使其指向下一个链接的位置
        localTm.p.x += separation;
    }
    if (i == length - 1)
    {
        // 如果i等于length-1，则表示已经创建了所有链接
        // 再创建一个动态刚体，并将其添加到场景中
        PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t * localTm, g2, *gMaterial, 1.0f);

        bodies_ChainDynamicQueue.push(current);

        // 使用createJoint函数创建最后一个链接与前一个链接之间的关节，并将其存储到joints数组中
        joints[i] = (*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
        // 将新创建的动态刚体添加到场景中
        gScene->addActor(*current);
        // 更新prev指针
        prev = current;
        // 更新localTm变量，使其指向下一个链接的位置
        localTm.p.x += separation;
    }
    // 返回joints数组
    return joints;
}

void deleteTerrain_Dynamic(std::queue<PxRigidDynamic*>& n_bodies_dynamicQueue)
{
    if (!n_bodies_dynamicQueue.empty())
    {
        PxRigidDynamic* n_bodies_dynamic = n_bodies_dynamicQueue.front();
        gScene->removeActor(*n_bodies_dynamic);
        n_bodies_dynamic->release();
        n_bodies_dynamicQueue.pop();
    }
}

void deleteTerrain_Chain(std::queue<PxJoint**>& n_bodies_JointQueue, std::queue<PxRigidDynamic*>& n_bodies_ChainDynamicQueue)
{
    // 判断队列是否为空
    if (!n_bodies_JointQueue.empty())
    {
        // 获取队列中第一个元素
        PxJoint** joints = n_bodies_JointQueue.front();
        // 删除队列中第一个元素
        n_bodies_JointQueue.pop();
        // 遍历joints数组，删除所有创建出的关节对象
        for (PxU32 i = 0; i < 4 - 1; i++)
        {
            // 获取第i个关节对象
            PxJoint* joint = joints[i];
            // 将关节对象释放
            joint->release();

            deleteTerrain_Dynamic(n_bodies_ChainDynamicQueue);

        }

        deleteTerrain_Dynamic(n_bodies_ChainDynamicQueue);
        // 释放joints数组

        //delete[] joints;
        //joints = nullptr;
    }
}


ID3D11ShaderResourceView* GameApp::loadImage(const char* path) {
    int width, height, channels;
    unsigned char* image_data = stbi_load(path, &width, &height, &channels, 4);

    if (!image_data)
    {
        // 图片加载失败处理
        printf("failed to load...");
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = image_data;
    initData.SysMemPitch = width * 4;
    ID3D11Texture2D* texture;
    m_pd3dDevice.Get()->CreateTexture2D(&desc, &initData, &texture);

    ID3D11ShaderResourceView* textureView;
    m_pd3dDevice.Get()->CreateShaderResourceView(texture, nullptr, &textureView);

    return textureView;
}

bool isResetNum(int last_num, int num)
{
    if (last_num == num)
        return true;
    else if (last_num == 5 && num == 8)
        return true;
    else if (last_num == 8 && num <= 4)
        return true;
    else if (last_num <= 3 && num >= 6)
        return true;
    else if (last_num == 6 && num == 3)
        return true;
    else if (last_num <= 4 && num <= 4)
        return false;
    else if (last_num >= 5 && num >= 5)
        return false;
    else
        return false;
}

void setTerrainSeed()
{
    int weights[] = { 14, 11, 12, 13, 11, 9, 12, 8, 10 };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(std::begin(weights), std::end(weights));


    int index = 0;
    int last_num = -1;

    CreateTextOfTerrain[index++] = 0;

    for (int i = index; i < 50; ++i)
    {
        int num = d(gen);
        while (isResetNum(last_num, num))
            num = d(gen);
        CreateTextOfTerrain[index++] = num;
        last_num = num;
    }



    for (int i = 0; i < index; ++i)
    {
        std::cout << CreateTextOfTerrain[i] << ' ';
    }
}

