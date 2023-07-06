//#include "GameApp.h"
//#include <XUtil.h>
//#include <DXTrace.h>
//#include<DemoProject.h>
//#include<PxPhysicsAPI.h>
//#include<fstream>
//#include "../Common/Print.h"
//#include "../Common/PVD.h"
//#include "../Utils/Utils.h"
//#include<vector>
//#include "foundation/PxPreprocessor.h"
//#include<GameObject.h>
//#include<string>
//#include<imgui.h>
//#define MAX_NUM_ACTOR_SHAPES 500
//using namespace DirectX;
//using namespace physx;
//#include <iostream>
//using namespace physx;
//void GameApp::RenderActors(PxRigidActor** actors, const PxU32 numActors)
//{
//    int numofbox = -1;
//    int numofsphere = -1;
//    int numofplane = -1;
//    int numofall = -1;
//    int numofTmesh = -1;
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
//            const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
//            PxGeometryHolder h = shapes[j]->getGeometry();
//            shapes[j]->getFlags()& PxShapeFlag::eTRIGGER_SHAPE;
//            switch (h.getType())
//            {
//            case PxGeometryType::eBOX:
//
//            {   numofbox++;
//            numofall++;
//            if (actors[i]->is<PxRigidDynamic>())
//            {
//                float x1 = 2 * h.box().halfExtents.x;
//                float x2 = 2 * h.box().halfExtents.y;
//                float x3 = 2 * h.box().halfExtents.z;
//                pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateBox(x1, x2, x3));
//                //pModels[i]->SetDebugObjectName("Box"+ numofbox);
//                m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\water1.dds", false, true);
//                pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\water1.dds");
//                pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
//                pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
//                pModels[i]->SetPhysicsData(actors[i]);
//                //PxTransform transform = pModels[i]->GetPhysicsData()->getGlobalPose();//更新位置信息
//                //XMFLOAT3 position(transform.p.x, transform.p.y, transform.p.z); 
//                GameObject box;
//                box.SetModel(pModels[i]);
//                m_Box.push_back(box);
//                UpdatePhysics(m_Box[numofbox]);
//                break;
//            }
//            else
//            {
//                float x1 = 2 * h.box().halfExtents.x;
//                float x2 = 2 * h.box().halfExtents.y;
//                float x3 = 2 * h.box().halfExtents.z;
//                pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateBox(x1, x2, x3));
//                //pModels[i]->SetDebugObjectName("Box"+ numofbox);
//                m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\stone.dds", false, true);
//                pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\stone.dds");
//                pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
//                pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
//                pModels[i]->SetPhysicsData(actors[i]);
//                //PxTransform transform = pModels[i]->GetPhysicsData()->getGlobalPose();//更新位置信息
//                //XMFLOAT3 position(transform.p.x, transform.p.y, transform.p.z); 
//                GameObject box;
//                box.SetModel(pModels[i]);
//                m_Box.push_back(box);
//                UpdatePhysics(m_Box[numofbox]);
//            }
//            }
//            case PxGeometryType::ePLANE:
//            {   numofplane++;
//            numofall++;
//            pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreatePlane(XMFLOAT2(800.0f, 800.0f), XMFLOAT2(6.0f, 9.0f)));
//            //pModels[i]->SetDebugObjectName("Ground"+numofplane);
//            m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\floor.dds", false, true);
//            pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\floor.dds");
//            m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\floor_nmap.dds");
//            pModels[i]->materials[0].Set<std::string>("$Normal", "..\\..\\src\\Texture\\floor_nmap.dds");
//            pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
//            pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
//            pModels[i]->materials[0].Set<float>("$SpecularPower", 16.0f);
//            pModels[i]->SetPhysicsData(actors[i]);
//            GameObject plane;
//            m_Plane.push_back(plane);
//            m_Plane[numofplane].SetModel(pModels[i]);
//            m_Plane[numofplane].GetTransform().SetPosition(0.0f, 0.0f, 0.0f);
//            break;
//            }
//
//
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
//            m_Sphere.push_back(sphere);
//            m_Sphere[numofsphere].SetModel(pModels[i]);
//            pModels[i]->SetPhysicsData(actors[i]);
//            UpdatePhysics(m_Sphere[numofsphere]);
//            break;
//            }
//            case PxGeometryType::eTRIANGLEMESH:
//            {   numofall++;
//                numofTmesh++;
//                const PxTriangleMeshGeometry& triGeom = h.triangleMesh();
//                const PxTriangleMesh& mesh = *triGeom.triangleMesh;
//                const PxVec3 scale = triGeom.scale.scale;
//                //获取三角形的几何体相关属性，包括三角形网格的指针，缩放比例
//                const PxU32 triangleCount = mesh.getNbTriangles();
//                const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
//                const void* indexBuffer = mesh.getTriangles();
//                //获取三角形的数量和索引类型 16 或 32
//                std::function<float(float, float)> heightFunc = [](float x, float z) -> float {
//                    return sinf(x) * cosf(z);
//                };
//                std::function<DirectX::XMFLOAT3(float, float)> normalFunc = [](float x, float z) -> DirectX::XMFLOAT3 {
//                    return DirectX::XMFLOAT3(-sinf(x), 1.0f, -cosf(z));
//                };
//                std::function<DirectX::XMFLOAT4(float, float)> colorFunc = [](float x, float z) -> DirectX::XMFLOAT4 {
//                    return DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
//                };
//                pModels[i] = m_ModelManager.CreateFromGeometry(numofall, Geometry::CreateGrid(XMFLOAT2(5.0f, 5.0f), XMUINT2(10, 10), XMFLOAT2(6.0f, 9.0f), heightFunc, normalFunc, colorFunc));
//                m_TextureManager.CreateFromFile("..\\..\\src\\Texture\\flare.dds", false, true);
//                pModels[i]->materials[0].Set<std::string>("$Diffuse", "..\\..\\src\\Texture\\flare.dds");
//                pModels[i]->materials[0].Set<XMFLOAT4>("$AmbientColor", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$DiffuseColor", XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f));
//                pModels[i]->materials[0].Set<XMFLOAT4>("$SpecularColor", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
//                GameObject cplane;
//                m_ChangePlane.push_back(cplane);
//                m_ChangePlane[numofTmesh].SetModel(pModels[i]);
//                pModels[i]->SetPhysicsData(actors[i]);
//                UpdatePhysics(m_Sphere[numofsphere]);
//             
//            }break;
//            // 创建并设置顶点缓冲区
//            case PxGeometryType::eCAPSULE:
//            {
//
//
//            }
//
//
//            
//            default:
//            break;
//
//
//            }
//        }
//
//
//    }
//
//
//}