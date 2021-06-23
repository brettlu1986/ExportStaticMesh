//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "pch.h"
#include <stdexcept>
// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
using Microsoft::WRL::ComPtr;

inline string HrToString(HRESULT Hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(Hr));
    return string(s_str);
}

class HrException : public runtime_error
{
public:
    HrException(HRESULT Hr) : runtime_error(HrToString(Hr)), HR(Hr) {}
    HRESULT Error() const { return HR; }
private:
    const HRESULT HR;
};

#define SAFE_RELEASE(p) if (p) (p)->Release()

inline void ThrowIfFailed(HRESULT Hr)
{
    if (FAILED(Hr))
    {
        throw HrException(Hr);
    }
}

inline wstring GetSaveDirectory()
{
	WCHAR CurPath[512];
	GetCurrentDirectory(_countof(CurPath), CurPath);
	wstring Path = CurPath;
	return Path + L"\\Resource\\";
}

inline void GetAssetsPath(_Out_writes_(PathSize) WCHAR* Path, UINT PathSize)
{
    if (Path == nullptr)
    {
        throw exception();
    }

    DWORD size = GetModuleFileName(nullptr, Path, PathSize);
    if (size == 0 || size == PathSize)
    {
        // Method failed or path was truncated.
        throw exception();
    }

    WCHAR* LastSlash = wcsrchr(Path, L'\\');
    if (LastSlash)
    {
        *(LastSlash + 1) = L'\0';
    }
}

inline HRESULT ReadDataFromFile(LPCWSTR Filename, byte** Data, UINT* Size)
{
    using namespace Microsoft::WRL;

#if WINVER >= _WIN32_WINNT_WIN8
    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
    extendedParams.lpSecurityAttributes = nullptr;
    extendedParams.hTemplateFile = nullptr;

    Wrappers::FileHandle file(CreateFile2(Filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
#else
    Wrappers::FileHandle file(CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS, nullptr));
#endif
    if (file.Get() == INVALID_HANDLE_VALUE)
    {
        throw exception();
    }

    FILE_STANDARD_INFO fileInfo = {};
    if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
    {
        throw exception();
    }

    if (fileInfo.EndOfFile.HighPart != 0)
    {
        throw exception();
    }

    *Data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
    *Size = fileInfo.EndOfFile.LowPart;

    if (!ReadFile(file.Get(), *Data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
    {
        throw exception();
    }

    return S_OK;
}

inline HRESULT ReadDataFromDDSFile(LPCWSTR filename, byte** data, UINT* offset, UINT* size)
{
    if (FAILED(ReadDataFromFile(filename, data, size)))
    {
        return E_FAIL;
    }

    // DDS files always start with the same magic number.
    static const UINT DDS_MAGIC = 0x20534444;
    UINT magicNumber = *reinterpret_cast<const UINT*>(*data);
    if (magicNumber != DDS_MAGIC)
    {
        return E_FAIL;
    }

    struct DDS_PIXELFORMAT
    {
        UINT size;
        UINT flags;
        UINT fourCC;
        UINT rgbBitCount;
        UINT rBitMask;
        UINT gBitMask;
        UINT bBitMask;
        UINT aBitMask;
    };

    struct DDS_HEADER
    {
        UINT size;
        UINT flags;
        UINT height;
        UINT width;
        UINT pitchOrLinearSize;
        UINT depth;
        UINT mipMapCount;
        UINT reserved1[11];
        DDS_PIXELFORMAT ddsPixelFormat;
        UINT caps;
        UINT caps2;
        UINT caps3;
        UINT caps4;
        UINT reserved2;
    };

    auto ddsHeader = reinterpret_cast<const DDS_HEADER*>(*data + sizeof(UINT));
    if (ddsHeader->size != sizeof(DDS_HEADER) || ddsHeader->ddsPixelFormat.size != sizeof(DDS_PIXELFORMAT))
    {
        return E_FAIL;
    }

    const ptrdiff_t ddsDataOffset = sizeof(UINT) + sizeof(DDS_HEADER);
    *offset = ddsDataOffset;
    *size = *size - ddsDataOffset;

    return S_OK;
}

inline void CreateBuffer(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, const void* InitData, 
    UINT64 ByteSize, Microsoft::WRL::ComPtr<ID3D12Resource>& DefaultBuffer, 
    Microsoft::WRL::ComPtr<ID3D12Resource>& UploadBuffer)
{

	const CD3DX12_HEAP_PROPERTIES VertexDefaultProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_RESOURCE_DESC VertexDefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(ByteSize);
	//create actual default buffer resource
	ThrowIfFailed(Device->CreateCommittedResource(
		&VertexDefaultProp,
		D3D12_HEAP_FLAG_NONE,
		&VertexDefaultDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&DefaultBuffer)));

	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap. 
	const CD3DX12_HEAP_PROPERTIES VertextUploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC VertextUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(ByteSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&VertextUploadProp,
		D3D12_HEAP_FLAG_NONE,
		&VertextUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&UploadBuffer)));

	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA SubResourceData;
	SubResourceData.pData = InitData;
	SubResourceData.RowPitch = ByteSize;
	SubResourceData.SlicePitch = SubResourceData.RowPitch;

	// Schedule to copy the data to the default buffer resource.  the function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to default buffer.
	const D3D12_RESOURCE_BARRIER RbVertex1 = CD3DX12_RESOURCE_BARRIER::Transition(DefaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
    CmdList->ResourceBarrier(1, &RbVertex1);

	UpdateSubresources<1>(CmdList, DefaultBuffer.Get(), UploadBuffer.Get(), 0, 0, 1, &SubResourceData);
	const D3D12_RESOURCE_BARRIER RbVertex2 = CD3DX12_RESOURCE_BARRIER::Transition(DefaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);
    CmdList->ResourceBarrier(1, &RbVertex2);

	// uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.
}

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
    pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
    WCHAR fullName[50];
    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
    {
        pObject->SetName(fullName);
    }
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_WITHINDEX(x, n) SetNameIndexed((x).Get(), L#x, n)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

inline UINT CalculateConstantBufferByteSize(UINT byteSize)
{
    // Constant buffer size is required to be aligned.
    return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
}

#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
    const wstring& filename,
    const D3D_SHADER_MACRO* defines,
    const string& entrypoint,
    const string& target)
{
    UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr;

    Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

    if (errors != nullptr)
    {
        OutputDebugStringA((char*)errors->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    return byteCode;
}
#endif

// Resets all elements in a ComPtr array.
template<class T>
void ResetComPtrArray(T* comPtrArray)
{
    for (auto &i : *comPtrArray)
    {
        i.Reset();
    }
}


// Resets all elements in a unique_ptr array.
template<class T>
void ResetUniquePtrArray(T* uniquePtrArray)
{
    for (auto &i : *uniquePtrArray)
    {
        i.reset();
    }
}


