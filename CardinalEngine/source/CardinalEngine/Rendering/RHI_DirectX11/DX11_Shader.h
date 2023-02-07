#pragma once
#include "CardinalEngine/Framework/Core/Headers.h"
#include "CardinalEngine/Rendering/RHI/Shader.h"

#include "DirectX11/D3D11.h"
#include <wrl.h>

namespace CE
{
	class DX11_DynamicRHI;

	class DX11_Shader
	{
	public:
		DX11_Shader(DX11_DynamicRHI* Owner) : m_Owner(Owner) {}

		const Microsoft::WRL::ComPtr<ID3DBlob>& NativeShaderByteCode() const
		{
			return m_ShaderByteCode;
		}

	protected:
		bool Reflect(EShaderStage Stage, ShaderReflection& Result, Array<String>* Errors) const;

	protected:
		DX11_DynamicRHI* m_Owner = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> m_ShaderByteCode = nullptr;
	};



	class DX11_VertexShader : public VertexShader, public DX11_Shader
	{
	public:
		DX11_VertexShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

		virtual bool Reflect(ShaderReflection& Result, Array<String>* Errors) const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11VertexShader>& NativeShader() const
		{
			return m_Shader;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_Shader = nullptr;
	};



	class DX11_TessellationControlShader : public TessellationControlShader, public DX11_Shader
	{
	public:
		DX11_TessellationControlShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

		virtual bool Reflect(ShaderReflection& Result, Array<String>* Errors) const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11HullShader>& NativeShader() const
		{
			return m_Shader;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11HullShader> m_Shader = nullptr;
	};



	class DX11_TessellationEvaluationShader : public TessellationEvaluationShader, public DX11_Shader
	{
	public:
		DX11_TessellationEvaluationShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

		virtual bool Reflect(ShaderReflection& Result, Array<String>* Errors) const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11DomainShader>& NativeShader() const
		{
			return m_Shader;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11DomainShader> m_Shader = nullptr;
	};



	class DX11_GeometryShader : public GeometryShader, public DX11_Shader
	{
	public:
		DX11_GeometryShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

		virtual bool Reflect(ShaderReflection& Result, Array<String>* Errors) const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11GeometryShader>& NativeShader() const
		{
			return m_Shader;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_Shader = nullptr;
	};



	class DX11_FragmentShader : public FragmentShader, public DX11_Shader
	{
	public:
		DX11_FragmentShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

		virtual bool Reflect(ShaderReflection& Result, Array<String>* Errors) const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11PixelShader>& NativeShader() const
		{
			return m_Shader;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_Shader = nullptr;
	};



	class DX11_ComputeShader : public ComputeShader, public DX11_Shader
	{
	public:
		DX11_ComputeShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry);

		virtual bool IsValid() const override;

		virtual DynamicRHI* Owner() const override;

		virtual bool Reflect(ShaderReflection& Result, Array<String>* Errors) const override;

	protected:
		virtual void Destroy() override;

	public:
		const Microsoft::WRL::ComPtr<ID3D11ComputeShader>& NativeShader() const
		{
			return m_Shader;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_Shader = nullptr;
	};
}
