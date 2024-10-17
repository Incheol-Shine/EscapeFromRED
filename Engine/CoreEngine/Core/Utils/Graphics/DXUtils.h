#pragma once
#include "common_include.h"
#include "Core/Graphics/graphics_common_include.h"


namespace Utils::DX
{
#pragma region ----------------------- Device Creation -----------------------
	void CreateRenderTarget(ID3D11Device*                    InDevice,
							uint32_t                         InWidth,
							uint32_t                         InHeight,
							_Out_ ID3D11RenderTargetView**   OutRTV,
							_Out_ ID3D11ShaderResourceView** OutSRV,
							_Out_ ID3D11Texture2D**          OutTexture);

	void CreateD2DRenderTarget(ID2D1Factory*             InFactory,
							   _In_ ID3D11Texture2D*     InTexture,
							   _Out_ ID2D1RenderTarget** OutRenderTarget);

	void CreateViewport(uint32_t              InWidth,
						uint32_t              InHeight,
						_Out_ D3D11_VIEWPORT* OutViewport);

	void CreateDepthStencilView(ID3D11Device*                  InDevice,
								uint32_t                       InWidth,
								uint32_t                       InHeight,
								_Out_ ID3D11DepthStencilView** OutDSV,
								_Out_ ID3D11Texture2D**        OutTexture);

	void CreateDepthStencilState(ID3D11Device*                   InDevice,
								 bool                            InDepthEnable,
								 D3D11_DEPTH_WRITE_MASK          InDepthWriteMask,
								 D3D11_COMPARISON_FUNC           InDepthFunc,
								 _Out_ ID3D11DepthStencilState** OutState);
#pragma endregion

	HRESULT LoadVertexShader(ID3D11Device*        Device, const JWText& VertexFileName,
							 ID3D11VertexShader** VertexShader,
							 ID3DBlob**           OutBlob = nullptr, LPCSTR EntryPoint = nullptr);
	HRESULT LoadPixelShader(ID3D11Device*       Device, const JWText& PixelFileName,
							ID3D11PixelShader** pixelShader,
							ID3DBlob**          OutBlob = nullptr, LPCSTR EntryPoint = nullptr);

	HRESULT LoadGeometryShader(ID3D11Device*          Device, const JWText&      ShaderFile,
							   ID3D11GeometryShader** geometryShader, ID3DBlob** OutBlob = nullptr,
							   LPCSTR                 FuncName                           = nullptr);
	HRESULT LoadHullShaderFile(ID3D11Device*      Device, const JWText&,
							   ID3D11HullShader** hullShader, ID3DBlob** OutBlob = nullptr,
							   LPCSTR             FuncName                       = nullptr);
	HRESULT LoadDomainShaderFile(ID3D11Device*        Device, const JWText&    ShaderFile,
								 ID3D11DomainShader** domainShader, ID3DBlob** OutBlob = nullptr,
								 LPCSTR               FuncName                         = nullptr);
	HRESULT LoadComputeShaderFile(ID3D11Device*         Device, const JWText& ShaderFile,
								  ID3D11ComputeShader** computeShader,
								  ID3DBlob**            OutBlob  = nullptr,
								  LPCSTR                FuncName = nullptr);


	HRESULT CompileShader(const WCHAR* FileName, LPCSTR        EntryPoint,
						  LPCSTR       ShaderModel, ID3DBlob** OutBlob);

	/**
	 * @brief ID3D11Buffer ���� ������ �õ� ���н� ����
	 * @param InDevice
	 * @param InBindFlag Vertex, Index, Constant ����
	 * @param InData ���ۿ� ���� ������
	 * @param InDataSize ������ ��� ����
	 * @param InDataNum ������ �迭 ����
	 * @param OutBuffer ��ȯ ����
	 * @param InUsage ��Ÿ�ӿ� ���� ������Ʈ �ȴٸ� D3D11_USAGE_DYNAMIC
	 * @param InAccessFlag Map�� ����ҰŸ� D3D11_CPU_ACCESS_WRITE�� ����
	 */
	void CreateBuffer(
		ID3D11Device*        InDevice,
		D3D11_BIND_FLAG      InBindFlag,
		void**               InData,
		uint32_t             InDataSize,
		uint32_t             InDataNum,
		_Out_ ID3D11Buffer** OutBuffer,
		D3D11_USAGE          InUsage      = D3D11_USAGE_DEFAULT,
		UINT                 InAccessFlag = 0
	);

	/**
	 * @brief ���۳��� �ϰ� ������Ʈ (Usage�� Dynamic�� �ƴϿ��� �������)
	 * @param InDeviceContext
	 * @param InBuffer ������Ʈ �� ����
	 * @param InData �� ������
	 */
	void UpdateBuffer(ID3D11DeviceContext* InDeviceContext,
					  ID3D11Buffer*        InBuffer,
					  const void*          InData);

	/**
	 * @brief ���۳��� ������Ʈ (������ Usage�� D3D11_USAGE_DYNAMIC�̿��� �� (Default X))
	 * �ణ�� ���ɻ� ������ ��������..
	 * @param InDeviceContext
	 * @param InBuffer
	 * @param InData
	 * @param InSize
	 */
	void UpdateDynamicBuffer(ID3D11DeviceContext* InDeviceContext,
							 ID3D11Buffer*        InBuffer,
							 const void*          InData,
							 UINT                 InSize);


	FVector ComputeFaceNormal(int32_t Index0, int32_t Index1, int32_t Index2, const float* InHeightMap);
}
