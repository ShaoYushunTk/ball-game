#ifndef GAMEAPP_H
#define GAMEAPP_H
#include<vector>
#include <random>
#include <WinMin.h>
#include "d3dApp.h"
#include <CameraController.h>
#include <RenderStates.h>
#include <GameObject.h>
#include <Texture2D.h>
#include <Buffer.h>
#include <Collision.h>
#include <ModelManager.h>
#include <TextureManager.h>
#include <LightHelper.h>
#include <chrono>
#include  "EffectsX.h"

#include <windows.h>
#include <mmsystem.h>
#include "common/stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <queue>


typedef PxJoint* (*JointCreateFunction)(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);

class GameApp : public D3DApp
{

public:
    
    GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    ~GameApp();

    bool Init();
    void OnResize();
    void UpdateScene(float dt);
    void DrawScene();
    void stepPhysics(float dt);
    bool initPhysics();
    bool initGame();
    void cleanupPhysics();
    void RenderShadow();
    void RenderForward();
    void RenderSkybox();
    void RenderActors(PxRigidActor** actors, const PxU32 numActors);
    void RenderArticulations(PxRigidActor** actors, const PxU32 numActors);
    //PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);
    //void createChain(const PxTransform& t, PxU32 length, const PxGeometry& g1, const PxGeometry& g2, PxReal separation, JointCreateFunction createJoint);
    void createTrigger();
    void createTerrainWeek3();
    void createTerrain_Week3_StageOf(const int i);
    void createTerrain_Stage0_Week3();
    void createTerrain_Stage1_Week3();
    void createTerrain_Stage2_Week3();
    void createTerrain_Stage3_Week3();
    void createTerrain_Stage4_Week3();
    void createTerrain_Stage5_Week3();
    void createTerrain_Stage6_Week3();
    void createTerrain_Stage7_Week3();
    void createTerrain_Stage8_Week3();
    void createTerrain_Stage9_Week5();
    PxRigidDynamic* createKinematicPlatform(PxVec3 location);
    PxRigidDynamic* createKinematicPlatform2(PxVec3 location);
    PxRigidDynamic* createKinematicPlatform3(PxVec3 location, PxReal x, PxReal y, PxReal z);
    void updateKinematicPlatform(PxVec3* verts,PxRigidDynamic* n_planeActor);
    void updateKinematicPlatform2(PxVec3* verts);
    void UpdatePhysics(GameObject& ob);
    static PxTriangleMesh* createMeshGround();
    PxRigidStatic* createDeformablePlatform(PxVec3 location);
    void createArticulation(PxVec3 location);
    static void updateVertices(PxVec3* verts, float amplitude = 0.0f);
    PxRigidStatic* createPlane(PxVec3 pos, PxU32 x, PxU32 y, PxU32 z, PxF32 size, PxVec3 rotation);
    PxRigidDynamic* createBall(PxReal r, PxVec3 location);
    PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity);
    void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
    PxRigidDynamic** createStackNew(const PxTransform& t, PxU32 size, PxReal halfExtent, PxReal Density);
    template<class Effect>
    void DrawScene(Effect& effect, std::function<void(Effect&, ID3D11DeviceContext*)> func = [](Effect&, ID3D11DeviceContext*) {})
    {
        // 这些物体有法线贴图
        {
            for (auto& plane : m_Plane)
            {
                plane.Draw(m_pd3dImmediateContext.Get(), effect,plane.m_pModel);
            }
            // 石柱
           /* for (auto& cylinder : m_Cylinders)
                cylinder.Draw(m_pd3dImmediateContext.Get(), effect);*/
        }

        // 没有法线贴图的物体调用普通绘制
        func(effect, m_pd3dImmediateContext.Get());
        for (auto& sphere : m_Sphere)
        {
            sphere.Draw(m_pd3dImmediateContext.Get(), effect,sphere.m_pModel);
        }
        //立方体
        for (auto& box : m_Box)
        {
            box.Draw(m_pd3dImmediateContext.Get(), effect, box.m_pModel);
        }
        for (auto& cplane : m_ChangePlane)
        {
            cplane.Draw(m_pd3dImmediateContext.Get(), effect, cplane.m_pModel);
        }

    }
    void setGameTotalTime() { m_GameTotalTime = m_Timer.TotalTime(); };
    float getGameTotalTime() { return m_GameTotalTime; };
    void setHP(int hp) { m_HP = hp; }
    int getHP() { return m_HP; }


    ID3D11ShaderResourceView* loadImage(const char* path);

    void freeObject();
    void createTerrain_Seed();
    void createTerrain_Stage0_Week5();
    void createTerrain_Stage1_Week5();
    void createTerrain_Stage2_Week5();
    void createTerrain_Stage3_Week5();
    void createTerrain_Stage4_Week5();
    void createTerrain_Stage5_Week5();
    void createTerrain_Stage6_Week5();
    void createTerrain_Stage7_Week5();
    void createTerrain_Stage8_Week5();
    PxRigidStatic* createPlaneNew(PxVec3 pos, PxU32 x, PxU32 y, PxU32 h, PxF32 size, PxVec3 rotation);
    void createStairsNew(PxVec3 pos, PxU32 x, PxU32 y, PxU32 h, PxF32 size, PxVec3 rotation);
    void createTerrain_Week5_StageOf(const int i);
    void deleteTerrain_Week5_StageOf(const int i);
    void deleteTerrain_Dynamic(PxU32 size);
    void deleteTerrain_Dynamic(std::queue<PxRigidDynamic**>& n_bodies_dynamicQueue, PxU32 size);
    void deleteTerrain_DeformablePlatform(std::queue<PxRigidStatic*>& n_bodies_RigidStaticQueue);
    void deleteTerrain_KinematicPlatform(std::queue<PxRigidDynamic*>& n_bodies_KinematicPlatformQueue);
    void deleteTerrain(std::queue<PxRigidStatic*>& n_bodiesQueue);
    //void deleteTerrain_Dynamic(std::queue<PxRigidDynamic**>& n_bodies_dynamicQueue, PxU32 size);


private:
    bool InitResource();

private:

    TextureManager m_TextureManager;
    ModelManager m_ModelManager;

    bool m_UpdateLight = true;                                  // 更新灯光
    bool m_EnableNormalMap = true;								// 开启法线贴图
    bool m_EnableDebug = false;									// 开启调试模式
    int m_SlopeIndex = 0;			  							// 斜率索引

    std::vector<GameObject> m_Plane;							// 平面
    //GameObject m_Cylinders[8];								// 圆柱体
    std::vector <GameObject> m_Box;                             //立方体
    std::vector < GameObject>m_Sphere;							// 球体
    GameObject m_Skybox;                                        // 天空盒
    std::vector <GameObject> m_ChangePlane;                     //动态平面

    std::vector<GameObject> m_BoxA;                             //连接体box
    std::vector<GameObject> m_SphereA;                          //连接体球
    std::vector<GameObject> m_Cylinder;                         //圆柱替代胶囊体

    std::unique_ptr<Depth2D> m_pDepthTexture;                   // 深度纹理
    std::unique_ptr<Texture2D> m_pLitTexture;                   // 场景渲染缓冲区
    std::unique_ptr<Depth2D> m_pShadowMapTexture;               // 阴影贴图
    std::unique_ptr<Texture2D> m_pDebugShadowTexture;           // 调试用阴影纹理

    DirectionalLight m_DirLights[3] = {};						// 方向光
    DirectX::XMFLOAT3 m_OriginalLightDirs[3] = {};				// 初始光方向

    BasicEffect m_BasicEffect;				                    // 基础特效
    ShadowEffect m_ShadowEffect;				                // 阴影特效
    SkyboxEffect m_SkyboxEffect;					            // 天空盒特效

    std::shared_ptr<FirstPersonCamera> m_pCamera;	            // 摄像机
    FirstPersonCameraController m_CameraController;             // 摄像机控制器

    float m_GameTotalTime;                                      // 游戏总时间
    int m_HP;                                                   // 角色血量


};


#endif