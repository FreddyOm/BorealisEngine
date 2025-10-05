#pragma once 
#include "../../../config.h"
#include "../../debug/logger.h"
#include "../../types/types.h"
#include <vector>

#if defined(BOREALIS_WIN)

#include <d3d12.h>
#include <wrl.h>

namespace Borealis::Graphics::Helpers
{
	struct D3D12DescriptorHeapAllocator
	{
        ID3D12DescriptorHeap*       Heap = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
        D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
        D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
        Types::uint64               HeapHandleIncrement;
        std::vector<Types::int32>            FreeIndices;

        void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
        {
            Assert(Heap == nullptr && FreeIndices.empty(), "Cannot construct an already existing D3D12RTVDescriptorHeapAllocator!");
            
            Heap = heap;
            
            D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
            HeapType = desc.Type;
            HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
            HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
            
			// GPU handle is only valid for CBV_SRV_UAV descriptor heaps
            if (HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
            {
                HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
            }

            FreeIndices.reserve((int)desc.NumDescriptors);
            
            for (Types::int32 n = desc.NumDescriptors; n > 0; n--)
                FreeIndices.push_back(n - 1);
        }
        
        void Destroy()
        {
            Heap = nullptr;
            FreeIndices.clear();
        }

        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
        {
            Assert(FreeIndices.size() > 0, "No more free descriptor heaps!");
            
            Types::int32 idx = FreeIndices.back();
            
            FreeIndices.pop_back();
           
            out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
            out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
        }
        
        void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
        {
            Types::int32 cpu_idx = (Types::int32)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
            Types::int32 gpu_idx = (Types::int32)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
            
            Assert(cpu_idx == gpu_idx, "Failed to calculate the correct descriptor heap index!");
            
            FreeIndices.push_back(cpu_idx);
        }
	};

    /// <summary>
    /// A context object used for each frame. It contains a command allocator pointer and a fence value
	/// for synchronization.
    /// </summary>
    struct FrameContext
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator>  CommandAllocator;
        Types::uint64                                   FenceValue;
    };

}

#endif