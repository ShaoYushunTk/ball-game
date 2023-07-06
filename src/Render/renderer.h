//#pragma once
//#pragma once
//
//#ifndef SHAPESDEMO_H
//#define SHAPESDEMO_H
//
//#include <d3dcompiler.h>
//#include <directxmath.h>
//#include <DDSTextureLoader.h>
//
//#include "resource.h"
//#include "d3dApp.h"
//#include "LightHelper.h"
//
//using namespace DirectX;
//
//class ShapesDemo : public D3DApp
//{
//public:
//	// 顶点Shader的传入参数
//	struct Vertex
//	{
//		XMFLOAT3 Pos;
//		XMFLOAT3 Normal;
//		XMFLOAT2 Tex;
//	};
//
//	// 常量缓存
//	struct ConstantBuffer
//	{
//		XMMATRIX gWorld;
//		XMMATRIX gWorldInvTranspose;
//		XMMATRIX gWorldViewProj;
//		XMMATRIX gTexTransform;
//	};
//
//	struct FrameConstantBuffer
//	{
//		DirectionalLight gDirLight;
//		Material gMaterial;
//		XMFLOAT4 gEyePosW;
//	};
//
//public:
//	ShapesDemo(HINSTANCE hInstance);
//	~ShapesDemo();
//
//	bool Init();
//	void OnResize();
//	void UpdateScene(float dt);
//	void DrawScene();
//
//	void OnMouseDown(WPARAM btnState, int x, int y);
//	void OnMouseUp(WPARAM btnState, int x, int y);
//	void OnMouseMove(WPARAM btnState, int x, int y);
//
//private:
//	void BuildGeometryBuffers();
//	void BuildFX();
//
//private:
//	ID3D11InputLayout* mVertexLayout;	// 顶点布局
//	ID3D11VertexShader* mVertexShader;	// 顶点Shader
//	ID3D11PixelShader* mPixelShader;	// 片元Shader
//
//	ID3D11Buffer* mBoxVB;	// 顶点缓冲
//	ID3D11Buffer* mBoxIB;	// 索引缓冲
//
//	// 常量缓冲
//	ID3D11Buffer* mBoxCB;
//	ID3D11Buffer* mFrameBoxCB;
//
//	// 常量数据结构
//	ConstantBuffer mCBuffer;
//	FrameConstantBuffer mFrameCBuffer;
//
//	float mTheta;
//	float mPhi;
//	float mRadius;
//
//	POINT mLastMousePos;
//
//	UINT totalVertexCount;
//	UINT totalIndexCount;
//
//	int mBoxVertexOffset;
//	int mBoxIndexCount;
//	int mBoxIndexOffset;
//
//	int mGridVertexOffset;
//	int mGridIndexCount;
//	int mGridIndexOffset;
//
//	int mSphereVertexOffset;
//	int mSphereIndexCount;
//	int mSphereIndexOffset;
//
//	int mCylinderVertexOffset;
//	int mCylinderIndexCount;
//	int mCylinderIndexOffset;
//
//	int mSkullVertexOffset;
//	int mSkullIndexCount;
//	int mSkullIndexOffset;
//
//	DirectX::XMFLOAT4X4 mSphereWorld[10];
//	DirectX::XMFLOAT4X4 mCylWorld[10];
//	DirectX::XMFLOAT4X4 mBoxWorld;
//	DirectX::XMFLOAT4X4 mGridWorld;
//	DirectX::XMFLOAT4X4 mCenterSphere;
//
//	DirectX::XMFLOAT4X4 mView;
//	DirectX::XMFLOAT4X4 mProj;
//	XMFLOAT4X4 mTexTransform;
//	XMFLOAT4 mEyePosW;
//
//	DirectionalLight mDirLight;
//	Material mMat;
//
//	ID3D11ShaderResourceView* mTextureDDS;
//	ID3D11SamplerState* mSamplerState;
//};
//
//#endif