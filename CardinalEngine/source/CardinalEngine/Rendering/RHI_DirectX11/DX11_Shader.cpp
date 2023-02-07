#include "DX11_Shader.h"
#include "DX11_DynamicRHI.h"

#define INITGUID
#include "DX11_Utilities.h"
#include "DX11_Functions.h"

namespace CE
{
	bool DX11_Shader::Reflect(EShaderStage Stage, ShaderReflection& OutResult, Array<String>* Errors) const
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector = NULL;
		HRESULT Result = DX11_Functions::D3DReflect(m_ShaderByteCode->GetBufferPointer(), m_ShaderByteCode->GetBufferSize(), __uuidof(pReflector), &pReflector);
		if (Result != S_OK)
		{
			if (Errors != nullptr)
			{
				Errors->Emplace(String::Format("Failed to Reflect Shader (%s)", DX11_Utilities::ResultToString(Result)));
			}
			else
			{
				CE_LOG(ELog::Warning, "Failed to Reflect Shader (%s)", DX11_Utilities::ResultToString(Result));
			}
			return false;
		}

		D3D11_SHADER_DESC ShaderDesc{};
		Result = pReflector->GetDesc(&ShaderDesc);
		if (Result != S_OK)
		{
			if (Errors != nullptr)
			{
				Errors->Emplace(String::Format("Failed to Get Reflected Shader Description (%s)", DX11_Utilities::ResultToString(Result)));
			}
			else
			{
				CE_LOG(ELog::Warning, "Failed to Get Reflected Shader Description (%s)", DX11_Utilities::ResultToString(Result));
			}
			return false;
		}

		for (uint32 i = 0; i < ShaderDesc.BoundResources; i++)
		{
			D3D11_SHADER_INPUT_BIND_DESC InputBoundDesc;
			pReflector->GetResourceBindingDesc(i, &InputBoundDesc);

			switch (InputBoundDesc.Type)
			{
			case D3D_SIT_CBUFFER:
			{
				if (OutResult.Images.find(InputBoundDesc.BindPoint) != OutResult.Images.end())
				{
					if (Errors != nullptr)
					{
						Errors->Emplace(String::Format("Shader Reflection Failed: Buffer Binding %u already used as Image", InputBoundDesc.BindPoint));
					}
					else
					{
						CE_LOG(ELog::Warning, "Shader Reflection Failed: Buffer Binding %u already used as Image", InputBoundDesc.BindPoint);
					}
					return false;
				}

				if (OutResult.Samplers.find(InputBoundDesc.BindPoint) != OutResult.Samplers.end())
				{
					if (Errors != nullptr)
					{
						Errors->Emplace(String::Format("Shader Reflection Failed: Buffer Binding %u already used as Sampler", InputBoundDesc.BindPoint));
					}
					else
					{
						CE_LOG(ELog::Warning, "Shader Reflection Failed: Buffer Binding %u already used as Sampler", InputBoundDesc.BindPoint);
					}
					return false;
				}

				ID3D11ShaderReflectionConstantBuffer* CurrentBuffer = pReflector->GetConstantBufferByName(InputBoundDesc.Name);
				D3D11_SHADER_BUFFER_DESC BufferDesc{};
				CurrentBuffer->GetDesc(&BufferDesc);

				ShaderReflection_Buffer BufferInfo;
				BufferInfo.Name = InputBoundDesc.Name;
				BufferInfo.Size = BufferDesc.Size;

				for (uint32 VariableIndex = 0; VariableIndex < BufferDesc.Variables; VariableIndex++)
				{
					ID3D11ShaderReflectionVariable* Variable = CurrentBuffer->GetVariableByIndex(VariableIndex);
					D3D11_SHADER_VARIABLE_DESC VariableDesc;
					Variable->GetDesc(&VariableDesc);
					ID3D11ShaderReflectionType* Type = Variable->GetType();

					D3D11_SHADER_TYPE_DESC TypeDesc;
					Type->GetDesc(&TypeDesc);

					ShaderReflection_Variable& VariableInfo = BufferInfo.Variables.EmplaceRef();
					VariableInfo.Name = VariableDesc.Name;
					VariableInfo.Size = VariableDesc.Size;

					if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_STRUCT)
					{
						VariableInfo.Type = EShaderVariableType::Struct;
					}
					else if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_SCALAR)
					{
						switch (TypeDesc.Type)
						{
						case D3D_SVT_BOOL:
							VariableInfo.Type = EShaderVariableType::Bool;
							break;
						case D3D_SVT_INT:
							VariableInfo.Type = EShaderVariableType::Int;
							break;
						case D3D_SVT_FLOAT:
							VariableInfo.Type = EShaderVariableType::Float;
							break;
						case D3D_SVT_UINT:
							VariableInfo.Type = EShaderVariableType::UInt;
							break;
						}
					}
					else if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_VECTOR)
					{
						if (TypeDesc.Columns == 2)
						{
							switch (TypeDesc.Type)
							{
							case D3D_SVT_INT:
								VariableInfo.Type = EShaderVariableType::Int2;
								break;
							case D3D_SVT_FLOAT:
								VariableInfo.Type = EShaderVariableType::Float2;
								break;
							case D3D_SVT_UINT:
								VariableInfo.Type = EShaderVariableType::UInt2;
								break;
							}
						}
						else if (TypeDesc.Columns == 3)
						{
							switch (TypeDesc.Type)
							{
							case D3D_SVT_INT:
								VariableInfo.Type = EShaderVariableType::Int3;
								break;
							case D3D_SVT_FLOAT:
								VariableInfo.Type = EShaderVariableType::Float3;
								break;
							case D3D_SVT_UINT:
								VariableInfo.Type = EShaderVariableType::UInt3;
								break;
							}
						}
						else if (TypeDesc.Columns == 4)
						{
							switch (TypeDesc.Type)
							{
							case D3D_SVT_INT:
								VariableInfo.Type = EShaderVariableType::Int4;
								break;
							case D3D_SVT_FLOAT:
								VariableInfo.Type = EShaderVariableType::Float4;
								break;
							case D3D_SVT_UINT:
								VariableInfo.Type = EShaderVariableType::UInt4;
								break;
							}
						}
					}
					else if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_MATRIX_ROWS || TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_MATRIX_COLUMNS)
					{
						if (TypeDesc.Type == D3D_SVT_FLOAT && TypeDesc.Rows == 4 && TypeDesc.Columns == 4)
						{
							VariableInfo.Type = EShaderVariableType::Float4x4;
						}
					}

					if (VariableInfo.Type == EShaderVariableType::Unknown)
					{
						if (Errors != nullptr)
						{
							Errors->Emplace(String::Format("Shader Reflection Failed: Unsupported Type in Buffer '%s' at %u", BufferDesc.Name, VariableIndex));
						}
						else
						{
							CE_LOG(ELog::Warning, "Shader Reflection Failed: Unsupported Type in Buffer '%s' at %u", BufferDesc.Name, VariableIndex);
						}
						return false;
					}
				}

				auto Inserted = OutResult.Buffers.insert(std::pair<uint32, ShaderReflection_Buffer>(InputBoundDesc.BindPoint, BufferInfo));
				if (!Inserted.second)
				{
					bool Equal = Inserted.first->second.Variables.Size() == BufferInfo.Variables.Size();
					if (Equal)
					{
						for (size_t CmpIdx = 0; CmpIdx < Inserted.first->second.Variables.Size(); CmpIdx++)
						{
							ShaderReflection_Variable& Info1 = Inserted.first->second.Variables[CmpIdx];
							ShaderReflection_Variable& Info2 = BufferInfo.Variables[CmpIdx];
							if (Info1.Name != Info2.Name || Info1.Type != Info2.Type || Info1.Size != Info2.Size)
							{
								Equal = false;
								break;
							}
						}
					}

					if (!Equal || Inserted.first->second.Name != BufferInfo.Name || Inserted.first->second.Size != BufferInfo.Size)
					{
						if (Errors != nullptr)
						{
							Errors->Emplace(String::Format("Shader Reflection Failed: Buffer Binding %i already used"));
						}
						else
						{
							CE_LOG(ELog::Warning, "Shader Reflection Failed: Buffer Binding %i already used", InputBoundDesc.BindPoint);
						}
						return false;
					}
				}

				break;
			}
			case D3D_SIT_TBUFFER:
				break;
			case D3D_SIT_TEXTURE:
			{
				if (OutResult.Buffers.find(InputBoundDesc.BindPoint) != OutResult.Buffers.end())
				{
					if (Errors != nullptr)
					{
						Errors->Emplace(String::Format("Shader Reflection Failed: Image Binding %u already used as Buffer", InputBoundDesc.BindPoint));
					}
					else
					{
						CE_LOG(ELog::Warning, "Shader Reflection Failed: Image Binding %u already used as Buffer", InputBoundDesc.BindPoint);
					}
					return false;
				}

				if (OutResult.Samplers.find(InputBoundDesc.BindPoint) != OutResult.Samplers.end())
				{
					if (Errors != nullptr)
					{
						Errors->Emplace(String::Format("Shader Reflection Failed: Image Binding %u already used as Sampler", InputBoundDesc.BindPoint));
					}
					else
					{
						CE_LOG(ELog::Warning, "Shader Reflection Failed: Image Binding %u already used as Sampler", InputBoundDesc.BindPoint);
					}
					return false;
				}

				ShaderReflection_Image ImageInfo;
				ImageInfo.Name = InputBoundDesc.Name;
				ImageInfo.ArraySize = 1;	// @TODO: Find Array Size

				switch (InputBoundDesc.Dimension)
				{
				default:
					ImageInfo.Dimension = ERHIImageDimension::Undefined;
					break;
				case D3D_SRV_DIMENSION_TEXTURE1D:
				case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
					ImageInfo.Dimension = ERHIImageDimension::Texture_1D;
					break;
				case D3D_SRV_DIMENSION_TEXTURE2D:
				case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
					ImageInfo.Dimension = ERHIImageDimension::Texture_2D;
					break;
				case D3D_SRV_DIMENSION_TEXTURE3D:
					ImageInfo.Dimension = ERHIImageDimension::Texture_3D;
					break;
				case D3D_SRV_DIMENSION_TEXTURECUBE:
				case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
					ImageInfo.Dimension = ERHIImageDimension::Texture_Cube;
					break;
				}

				auto Inserted = OutResult.Images.insert(std::pair<uint32, ShaderReflection_Image>(InputBoundDesc.BindPoint, ImageInfo));
				if (!Inserted.second)
				{
					if (Inserted.first->second.Name != ImageInfo.Name || Inserted.first->second.Dimension != ImageInfo.Dimension || Inserted.first->second.ArraySize != ImageInfo.ArraySize)
					{
						if (Errors != nullptr)
						{
							Errors->Emplace(String::Format("Shader Reflection Failed: Image Binding %i already used"));
						}
						else
						{
							CE_LOG(ELog::Warning, "Shader Reflection Failed: Image Binding %i already used", InputBoundDesc.BindPoint);
						}
						return false;
					}
				}
				break;
			}
			case D3D_SIT_SAMPLER:
			{
				if (OutResult.Buffers.find(InputBoundDesc.BindPoint) != OutResult.Buffers.end())
				{
					if (Errors != nullptr)
					{
						Errors->Emplace(String::Format("Shader Reflection Failed: Sampler Binding %u already used as Buffer", InputBoundDesc.BindPoint));
					}
					else
					{
						CE_LOG(ELog::Warning, "Shader Reflection Failed: Sampler Binding %u already used as Buffer", InputBoundDesc.BindPoint);
					}
					return false;
				}

				if (OutResult.Images.find(InputBoundDesc.BindPoint) != OutResult.Images.end())
				{
					if (Errors != nullptr)
					{
						Errors->Emplace(String::Format("Shader Reflection Failed: Sampler Binding %u already used as Image", InputBoundDesc.BindPoint));
					}
					else
					{
						CE_LOG(ELog::Warning, "Shader Reflection Failed: Sampler Binding %u already used as Image", InputBoundDesc.BindPoint);
					}
					return false;
				}

				ShaderReflection_Sampler SamplerInfo;
				SamplerInfo.Name = InputBoundDesc.Name;

				auto Inserted = OutResult.Samplers.insert(std::pair<uint32, ShaderReflection_Sampler>(InputBoundDesc.BindPoint, SamplerInfo));
				if (!Inserted.second)
				{
					if (Inserted.first->second.Name != SamplerInfo.Name)
					{
						if (Errors != nullptr)
						{
							Errors->Emplace(String::Format("Shader Reflection Failed: Sampler Binding %i already used"));
						}
						else
						{
							CE_LOG(ELog::Warning, "Shader Reflection Failed: Sampler Binding %i already used", InputBoundDesc.BindPoint);
						}
						return false;
					}
				}

				break;
			}
			case D3D_SIT_UAV_RWTYPED:
				break;
			case D3D_SIT_STRUCTURED:
				break;
			case D3D_SIT_UAV_RWSTRUCTURED:
				break;
			case D3D_SIT_BYTEADDRESS:
				break;
			case D3D_SIT_UAV_RWBYTEADDRESS:
				break;
			case D3D_SIT_UAV_APPEND_STRUCTURED:
				break;
			case D3D_SIT_UAV_CONSUME_STRUCTURED:
				break;
			case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
				break;
			case D3D_SIT_RTACCELERATIONSTRUCTURE:
				break;
			case D3D_SIT_UAV_FEEDBACKTEXTURE:
				break;
			default:
				break;
			}
		}

		return true;
	}

	DX11_VertexShader::DX11_VertexShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry)
		: DX11_Shader(Owner)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderByteCode;
		DX11_Functions::D3DCreateBlob(DataSize, &ShaderByteCode);
		memcpy(ShaderByteCode->GetBufferPointer(), Data, DataSize);
		m_ShaderByteCode = ShaderByteCode;

		HRESULT Result = m_Owner->Device()->CreateVertexShader(ShaderByteCode->GetBufferPointer(), ShaderByteCode->GetBufferSize(), NULL, &m_Shader);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Vertex Shader (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_VertexShader::IsValid() const
	{
		return m_ShaderByteCode != nullptr && m_Shader != nullptr;
	}

	DynamicRHI* DX11_VertexShader::Owner() const
	{
		return m_Owner;
	}

	bool DX11_VertexShader::Reflect(ShaderReflection& Result, Array<String>* Errors) const
	{
		return DX11_Shader::Reflect(Stage(), Result, Errors);
	}

	void DX11_VertexShader::Destroy()
	{
		m_ShaderByteCode = nullptr;
		m_Shader = nullptr;
	}

	DX11_TessellationControlShader::DX11_TessellationControlShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry)
		: DX11_Shader(Owner)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderByteCode;
		DX11_Functions::D3DCreateBlob(DataSize, &ShaderByteCode);
		memcpy(ShaderByteCode->GetBufferPointer(), Data, DataSize);
		m_ShaderByteCode = ShaderByteCode;

		HRESULT Result = m_Owner->Device()->CreateHullShader(ShaderByteCode->GetBufferPointer(), ShaderByteCode->GetBufferSize(), NULL, &m_Shader);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Tesselation Control Shader (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_TessellationControlShader::IsValid() const
	{
		return m_Shader != nullptr;
	}

	DynamicRHI* DX11_TessellationControlShader::Owner() const
	{
		return m_Owner;
	}

	bool DX11_TessellationControlShader::Reflect(ShaderReflection& Result, Array<String>* Errors) const
	{
		return DX11_Shader::Reflect(Stage(), Result, Errors);
	}

	void DX11_TessellationControlShader::Destroy()
	{
		m_Shader = nullptr;
	}

	DX11_TessellationEvaluationShader::DX11_TessellationEvaluationShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry)
		: DX11_Shader(Owner)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderByteCode;
		DX11_Functions::D3DCreateBlob(DataSize, &ShaderByteCode);
		memcpy(ShaderByteCode->GetBufferPointer(), Data, DataSize);
		m_ShaderByteCode = ShaderByteCode;

		HRESULT Result = m_Owner->Device()->CreateDomainShader(ShaderByteCode->GetBufferPointer(), ShaderByteCode->GetBufferSize(), NULL, &m_Shader);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Tesselation Evaluation Shader (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_TessellationEvaluationShader::IsValid() const
	{
		return m_Shader != nullptr;
	}

	DynamicRHI* DX11_TessellationEvaluationShader::Owner() const
	{
		return m_Owner;
	}

	bool DX11_TessellationEvaluationShader::Reflect(ShaderReflection& Result, Array<String>* Errors) const
	{
		return DX11_Shader::Reflect(Stage(), Result, Errors);
	}

	void DX11_TessellationEvaluationShader::Destroy()
	{
		m_Shader = nullptr;
	}

	DX11_GeometryShader::DX11_GeometryShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry)
		: DX11_Shader(Owner)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderByteCode;
		DX11_Functions::D3DCreateBlob(DataSize, &ShaderByteCode);
		memcpy(ShaderByteCode->GetBufferPointer(), Data, DataSize);
		m_ShaderByteCode = ShaderByteCode;

		HRESULT Result = m_Owner->Device()->CreateGeometryShader(ShaderByteCode->GetBufferPointer(), ShaderByteCode->GetBufferSize(), NULL, &m_Shader);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Geometry Shader (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_GeometryShader::IsValid() const
	{
		return m_Shader != nullptr;
	}

	DynamicRHI* DX11_GeometryShader::Owner() const
	{
		return m_Owner;
	}

	bool DX11_GeometryShader::Reflect(ShaderReflection& Result, Array<String>* Errors) const
	{
		return DX11_Shader::Reflect(Stage(), Result, Errors);
	}

	void DX11_GeometryShader::Destroy()
	{
		m_Shader = nullptr;
	}

	DX11_FragmentShader::DX11_FragmentShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry)
		: DX11_Shader(Owner)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderByteCode;
		DX11_Functions::D3DCreateBlob(DataSize, &ShaderByteCode);
		memcpy(ShaderByteCode->GetBufferPointer(), Data, DataSize);
		m_ShaderByteCode = ShaderByteCode;

		HRESULT Result = m_Owner->Device()->CreatePixelShader(ShaderByteCode->GetBufferPointer(), ShaderByteCode->GetBufferSize(), NULL, &m_Shader);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Fragment Shader (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_FragmentShader::IsValid() const
	{
		return m_Shader != nullptr;
	}

	DynamicRHI* DX11_FragmentShader::Owner() const
	{
		return m_Owner;
	}

	bool DX11_FragmentShader::Reflect(ShaderReflection& Result, Array<String>* Errors) const
	{
		return DX11_Shader::Reflect(Stage(), Result, Errors);
	}

	void DX11_FragmentShader::Destroy()
	{
		m_Shader = nullptr;
	}

	DX11_ComputeShader::DX11_ComputeShader(DX11_DynamicRHI* Owner, const char* Data, size_t DataSize, const char* Entry)
		: DX11_Shader(Owner)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> ShaderByteCode;
		DX11_Functions::D3DCreateBlob(DataSize, &ShaderByteCode);
		memcpy(ShaderByteCode->GetBufferPointer(), Data, DataSize);
		m_ShaderByteCode = ShaderByteCode;

		HRESULT Result = m_Owner->Device()->CreateComputeShader(ShaderByteCode->GetBufferPointer(), ShaderByteCode->GetBufferSize(), NULL, &m_Shader);
		if (Result != S_OK)
		{
			CE_LOG(ELog::Error, "Failed to create Compute Shader (%s)", DX11_Utilities::ResultToString(Result));
			Destroy();
			return;
		}
	}

	bool DX11_ComputeShader::IsValid() const
	{
		return m_Shader != nullptr;
	}

	DynamicRHI* DX11_ComputeShader::Owner() const
	{
		return m_Owner;
	}

	bool DX11_ComputeShader::Reflect(ShaderReflection& Result, Array<String>* Errors) const
	{
		return DX11_Shader::Reflect(Stage(), Result, Errors);
	}

	void DX11_ComputeShader::Destroy()
	{
		m_Shader = nullptr;
	}
}
