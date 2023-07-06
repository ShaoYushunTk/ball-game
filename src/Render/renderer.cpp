//#include "renderer.h"
//#include "d3dUtil.h"
//#include "MathHelper.h"
//#include "LightHelper.h"
//#include "GeometryGenerator.h"
//#include "PxPhysicsAPI.h"
//#include <filesystem>
//using namespace physx;
//// �������
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//	_In_opt_ HINSTANCE hPrevInstance,
//	_In_ LPWSTR    lpCmdLine,
//	_In_ int       nCmdShow)
//{
//	// ����ʵʱ�ڴ���
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//	ShapesDemo theApp(hInstance);
//	if (!theApp.Init())
//		return 0;
//	return theApp.Run();
//}
//
//ShapesDemo::ShapesDemo(HINSTANCE hInstance) : D3DApp(hInstance),
//mVertexShader(0), mPixelShader(0), mVertexLayout(0),
//mBoxVB(0), mBoxIB(0), mBoxCB(0), mFrameBoxCB(0),
//mTheta(1.5f * MathHelper::Pi), mPhi(0.25f * MathHelper::Pi), mRadius(5.0f)
//{
//	mMainWndCaption = L"Box Demo";
//
//	mLastMousePos.x = 0;
//	mLastMousePos.y = 0;
//
//	XMMATRIX I = XMMatrixIdentity();
//
//	// Ĭ��Ϊ��λ����
//	// �������󣬵����ȸ���ֵ
//	DirectX::XMStoreFloat4x4(&mView, I);
//	DirectX::XMStoreFloat4x4(&mProj, I);
//	XMStoreFloat4x4(&mTexTransform, I);
//
//	// ��ʼ������ģ�͵�λ��
//	DirectX::XMStoreFloat4x4(&mGridWorld, I);
//
//	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
//	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
//	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));
//
//	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
//	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
//	XMStoreFloat4x4(&mCenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));
//
//	for (size_t i = 0; i < 5; i++)
//	{
//		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
//		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));
//
//		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
//		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
//	}
//
//	// ֱ���
//	mDirLight.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
//	mDirLight.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
//	mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
//	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
//
//	mMat.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
//	mMat.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
//	mMat.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 96.0f);
//}
//
//ShapesDemo::~ShapesDemo()
//{
//	ReleaseCOM(mBoxVB);
//	ReleaseCOM(mBoxIB);
//	ReleaseCOM(mBoxCB);
//	ReleaseCOM(mFrameBoxCB);
//	ReleaseCOM(mVertexLayout);
//	ReleaseCOM(mVertexShader);
//	ReleaseCOM(mPixelShader);
//}
//
//bool ShapesDemo::Init()
//{
//	if (!D3DApp::Init())
//		return false;
//	//createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);
//	BuildGeometryBuffers();
//	BuildFX();
//
//	// ��ʼ������
//	HR(CreateDDSTextureFromFile(md3dDevice, L"Textures\\darkbrickdxt1.dds", nullptr, &mTextureDDS));
//
//	// ��ʼ��������״̬
//	D3D11_SAMPLER_DESC sampDesc;
//	ZeroMemory(&sampDesc, sizeof(sampDesc));
//	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;	// ���˷�������Ϊ��������
//	sampDesc.MaxAnisotropy = 4;
//	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;	// ѹ������
//	sampDesc.MinLOD = 0;
//	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
//	HR(md3dDevice->CreateSamplerState(&sampDesc, &mSamplerState));
//
//	return true;
//}
//
//void ShapesDemo::OnResize()
//{
//	D3DApp::OnResize();
//
//	// �����ڳߴ�仯������ͶӰ����
//	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
//	DirectX::XMStoreFloat4x4(&mProj, P);
//}
//
//// ���������������Ϣ��������GPU�Ļ���
//void ShapesDemo::BuildGeometryBuffers()
//{
//	// Mesh��Ϣ
//	GeometryGenerator::MeshData box;
//	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, box);
//
//	GeometryGenerator::MeshData grid;
//	GeometryGenerator::CreateGrid(20.0f, 30.0f, 60, 40, grid);
//
//	GeometryGenerator::MeshData sphere;
//	GeometryGenerator::CreateSphere(0.5f, 20, 20, sphere);
//
//	GeometryGenerator::MeshData cylinder;
//	GeometryGenerator::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);
//
//	// ��ȡͷ������
//	GeometryGenerator::MeshData skull;
//
//	std::ifstream fin("Models/skull.txt");
//	if (!fin)
//	{
//		//MessageBox(0, L"Models/skull.txt not found.", 0, 0);
//		return;
//	}
//
//	UINT vcount = 0;
//	UINT tcount = 0;
//	std::string ignore;	// ���Զ�ȡ
//
//	fin >> ignore >> vcount;
//	fin >> ignore >> tcount;
//	fin >> ignore >> ignore >> ignore >> ignore;
//
//	for (UINT i = 0; i < vcount; i++)
//	{
//		GeometryGenerator::Vertex vertex;
//		fin >> vertex.Position.x >> vertex.Position.y >> vertex.Position.z;
//		fin >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
//		skull.Vertices.push_back(vertex);
//	}
//
//	fin >> ignore;
//	fin >> ignore;
//	fin >> ignore;
//
//	for (UINT i = 0; i < tcount; i++)
//	{
//		UINT tmp1;
//		UINT tmp2;
//		UINT tmp3;
//		fin >> tmp1 >> tmp2 >> tmp3;
//
//		skull.Indices.push_back(tmp1);
//		skull.Indices.push_back(tmp2);
//		skull.Indices.push_back(tmp3);
//	}
//
//	fin.close();
//
//	// ���㡢ƫ����Ϣ
//	mBoxVertexOffset = 0;
//	mGridVertexOffset = box.Vertices.size();
//	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
//	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();
//	mSkullVertexOffset = mCylinderVertexOffset + cylinder.Vertices.size();
//
//	mBoxIndexCount = box.Indices.size();
//	mGridIndexCount = grid.Indices.size();
//	mSphereIndexCount = sphere.Indices.size();
//	mCylinderIndexCount = cylinder.Indices.size();
//	mSkullIndexCount = skull.Indices.size();
//
//	mBoxIndexOffset = 0;
//	mGridIndexOffset = mBoxIndexCount;
//	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
//	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;
//	mSkullIndexOffset = mCylinderIndexOffset + mCylinderIndexCount;
//
//	totalVertexCount =
//		box.Vertices.size()
//		+ grid.Vertices.size()
//		+ sphere.Vertices.size()
//		+ cylinder.Vertices.size()
//		+ skull.Vertices.size();
//	totalIndexCount =
//		mBoxIndexCount
//		+ mGridIndexCount
//		+ mSphereIndexCount
//		+ mCylinderIndexCount
//		+ mSkullIndexCount;
//
//	// ��Ӷ�����Ϣ
//	std::vector<Vertex> vertices(totalVertexCount);
//	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);
//
//	UINT k = 0;	// ÿ�����㶼ƫ��k������Ϊ���ķֱ���Ⱦ��������Ϣ������Ҫƫ��k
//	for (size_t i = 0; i < box.Vertices.size(); i++, k++)
//	{
//		vertices[k].Pos = box.Vertices[i].Position;
//		vertices[k].Normal = box.Vertices[i].Normal;
//		vertices[k].Tex = box.Vertices[i].TexC;
//	}
//
//	for (size_t i = 0; i < grid.Vertices.size(); i++, k++)
//	{
//		vertices[k].Pos = grid.Vertices[i].Position;
//		vertices[k].Normal = grid.Vertices[i].Normal;
//		vertices[k].Tex = grid.Vertices[i].TexC;
//	}
//
//	for (size_t i = 0; i < sphere.Vertices.size(); i++, k++)
//	{
//		vertices[k].Pos = sphere.Vertices[i].Position;
//		vertices[k].Normal = sphere.Vertices[i].Normal;
//		vertices[k].Tex = sphere.Vertices[i].TexC;
//	}
//
//	for (size_t i = 0; i < cylinder.Vertices.size(); i++, k++)
//	{
//		vertices[k].Pos = cylinder.Vertices[i].Position;
//		vertices[k].Normal = cylinder.Vertices[i].Normal;
//		vertices[k].Tex = cylinder.Vertices[i].TexC;
//	}
//
//	for (size_t i = 0; i < skull.Vertices.size(); i++, k++)
//	{
//		vertices[k].Pos = skull.Vertices[i].Position;
//		vertices[k].Normal = skull.Vertices[i].Normal;
//		vertices[k].Tex = skull.Vertices[i].TexC;
//	}
//
//	// ���������Ϣ
//	std::vector<UINT> indices;
//	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
//	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
//	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
//	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
//	indices.insert(indices.end(), skull.Indices.begin(), skull.Indices.end());
//
//	// ���㻺�������
//	D3D11_BUFFER_DESC vbd;
//	ZeroMemory(&vbd, sizeof(vbd));
//	vbd.Usage = D3D11_USAGE_DEFAULT;
//	vbd.ByteWidth = sizeof(Vertex) * totalVertexCount;
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vbd.CPUAccessFlags = 0;
//
//	// ���������õ�������
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = &vertices[0];
//
//	// ��������
//	HR(md3dDevice->CreateBuffer(&vbd, &InitData, &mBoxVB));
//
//	// �󶨵���Ⱦ������
//	UINT stride = sizeof(Vertex);
//	UINT offset = 0;
//	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
//
//	// �������������
//	D3D11_BUFFER_DESC ibd;
//	ZeroMemory(&ibd, sizeof(ibd));
//	ibd.Usage = D3D11_USAGE_DEFAULT;
//	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.CPUAccessFlags = 0;
//
//	// ���������õ�������
//	InitData.pSysMem = &indices[0];
//	HR(md3dDevice->CreateBuffer(&ibd, &InitData, &mBoxIB));
//
//	// �󶨵���Ⱦ������
//	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// �趨ͼԪ����
//	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);
//
//
//	// ���ó���������
//	D3D11_BUFFER_DESC cbd;
//	ZeroMemory(&cbd, sizeof(cbd));
//	cbd.Usage = D3D11_USAGE_DEFAULT;
//	cbd.ByteWidth = sizeof(ConstantBuffer);
//	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbd.CPUAccessFlags = 0;
//	HR(md3dDevice->CreateBuffer(&cbd, nullptr, &mBoxCB));
//	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mBoxCB);	// VS������������Ӧb0
//
//	D3D11_BUFFER_DESC cbdFrame;
//	ZeroMemory(&cbdFrame, sizeof(cbdFrame));
//	cbdFrame.Usage = D3D11_USAGE_DEFAULT;
//	cbdFrame.ByteWidth = sizeof(FrameConstantBuffer);
//	cbdFrame.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cbdFrame.CPUAccessFlags = 0;
//	HR(md3dDevice->CreateBuffer(&cbdFrame, nullptr, &mFrameBoxCB));
//	md3dImmediateContext->PSSetConstantBuffers(1, 1, &mFrameBoxCB);	// PS������������Ӧb1
//}
//
//// ������ɫ��
//void ShapesDemo::BuildFX()
//{
//	ID3DBlob* blob;
//
//	// ����������ɫ��
//	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Shapes_VS.vso", L"HLSL\\Shapes_VS.hlsl", "VS", "vs_5_0", &blob));
//	HR(md3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader));
//
//	// �������㲼��
//	D3D11_INPUT_ELEMENT_DESC vertexLayout[3] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};
//	HR(md3dDevice->CreateInputLayout(vertexLayout, ARRAYSIZE(vertexLayout), blob->GetBufferPointer(), blob->GetBufferSize(), &mVertexLayout));
//	md3dImmediateContext->IASetInputLayout(mVertexLayout);// �趨���벼��
//	ReleaseCOM(blob);
//
//	// ����������ɫ��
//	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Shapes_PS.pso", L"HLSL\\Shapes_PS.hlsl", "PS", "ps_5_0", &blob));
//	HR(md3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mPixelShader));
//	ReleaseCOM(blob);
//
//	md3dImmediateContext->VSSetShader(mVertexShader, nullptr, 0);
//	md3dImmediateContext->PSSetShader(mPixelShader, nullptr, 0);
//}
//
//void ShapesDemo::UpdateScene(float dt)
//{
//	float x = mRadius * sinf(mPhi) * cosf(mTheta);
//	float z = mRadius * sinf(mPhi) * sinf(mTheta);
//	float y = mRadius * cosf(mPhi);
//
//	mEyePosW = XMFLOAT4(x, y, z, 0.0f);
//
//	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
//	XMVECTOR target = XMVectorZero();
//	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//
//	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
//	DirectX::XMStoreFloat4x4(&mView, V);
//
//	// ƬԪ����
//	mFrameCBuffer.gEyePosW = mEyePosW;
//	mFrameCBuffer.gDirLight = mDirLight;
//	mFrameCBuffer.gMaterial = mMat;
//	md3dImmediateContext->UpdateSubresource(mFrameBoxCB, 0, nullptr, &mFrameCBuffer, 0, 0);
//
//	md3dImmediateContext->PSSetSamplers(0, 1, &mSamplerState);	// ��ӦShader�е�register(s0)
//	md3dImmediateContext->PSSetShaderResources(0, 1, &mTextureDDS);	// ��ӦShader�е�regiser(t0)
//}
//
//void ShapesDemo::DrawScene()
//{
//	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
//	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//	// vp���󲻱�
//	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
//	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
//	DirectX::XMMATRIX vp = view * proj;
//
//	// world������ݲ�ͬ���������ͬ
//	// ��Ⱦ����
//	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mGridWorld);
//	mCBuffer.gWorldViewProj = world * view * proj;
//	mCBuffer.gWorld = world;
//	mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
//	mCBuffer.gTexTransform = XMLoadFloat4x4(&mTexTransform);
//	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
//	md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);
//
//	// ��Ⱦ����
//	world = DirectX::XMLoadFloat4x4(&mBoxWorld);
//	mCBuffer.gWorldViewProj = world * view * proj;
//	mCBuffer.gWorld = world;
//	mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
//	mCBuffer.gTexTransform = XMLoadFloat4x4(&mTexTransform);
//	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
//	md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
//
//	// ��Ⱦ�м���
//	world = DirectX::XMLoadFloat4x4(&mCenterSphere);
//	mCBuffer.gWorldViewProj = world * view * proj;
//	mCBuffer.gWorld = world;
//	mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
//	mCBuffer.gTexTransform = XMLoadFloat4x4(&mTexTransform);
//	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
//	md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
//
//	// ��Ⱦͷ��
//	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
//	md3dImmediateContext->DrawIndexed(mSkullIndexCount, mSkullIndexOffset, mSkullVertexOffset);
//
//	// ��Ⱦ10������
//	for (size_t i = 0; i < 10; i++)
//	{
//		world = DirectX::XMLoadFloat4x4(&mCylWorld[i]);
//		mCBuffer.gWorldViewProj = world * view * proj;
//		mCBuffer.gWorld = world;
//		mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
//		mCBuffer.gTexTransform = XMLoadFloat4x4(&mTexTransform);
//		md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
//		md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
//	}
//
//	// ��Ⱦ10�������ϵ���
//	for (size_t i = 0; i < 10; i++)
//	{
//		world = DirectX::XMLoadFloat4x4(&mSphereWorld[i]);
//		mCBuffer.gWorldViewProj = world * view * proj;
//		mCBuffer.gWorld = world;
//		mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
//		mCBuffer.gTexTransform = XMLoadFloat4x4(&mTexTransform);
//		md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
//		md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
//	}
//
//	HR(mSwapChain->Present(0, 0));
//}
//
//void ShapesDemo::OnMouseDown(WPARAM btnState, int x, int y)
//{
//	mLastMousePos.x = x;
//	mLastMousePos.y = y;
//	SetCapture(mhMainWnd);	// �������
//}
//
//void ShapesDemo::OnMouseUp(WPARAM btnState, int x, int y)
//{
//	ReleaseCapture();
//}
//
//void ShapesDemo::OnMouseMove(WPARAM btnState, int x, int y)
//{
//	if ((btnState & MK_LBUTTON) != 0)
//	{
//		// ��������ƶ��ľ���
//		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
//		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));
//
//		// �����ӽ���Ϣ
//		mTheta += dx;
//		mPhi += dy;
//
//		// �ü�mPhi�Ƕ�
//		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
//	}
//	else if ((btnState & MK_RBUTTON) != 0)
//	{
//		// �趨ÿ�����ص�Ϊ0.005�ľ���
//		float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
//		float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);
//
//		// �������������ľ���
//		mRadius += dx - dy;
//
//		// �ü�����
//		mRadius = MathHelper::Clamp(mRadius, 3.0f, 50.0f);
//	}
//
//	mLastMousePos.x = x;
//	mLastMousePos.y = y;
//}
