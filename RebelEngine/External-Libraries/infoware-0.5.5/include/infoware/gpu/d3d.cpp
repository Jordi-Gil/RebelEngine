// infoware - C++ System information Library
//
// Written in 2016-2020 by nabijaczleweli <nabijaczleweli@gmail.com> and ThePhD <phdofthehouse@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>


#ifdef INFOWARE_USE_D3D


#include "infoware/detail/memory.hpp"
#include "infoware/detail/winstring.hpp"
#include "infoware/gpu.hpp"
#include "infoware/pci.hpp"
#include "dxgi1_6.h"
#include <d3d11_4.h>
#include <memory>


static iware::gpu::vendor_t vendor_from_name(const std::string& v) {
	if(v.find("NVidia") != std::string::npos || v.find("NVIDIA") != std::string::npos)
		return iware::gpu::vendor_t::nvidia;
	else if(v.find("AMD") != std::string::npos || v.find("ATi") != std::string::npos || v.find("Advanced Micro Devices") != std::string::npos)
		return iware::gpu::vendor_t::amd;
	else if(v.find("Intel") != std::string::npos)
		return iware::gpu::vendor_t::intel;
	else if(v.find("Microsoft") != std::string::npos)
		return iware::gpu::vendor_t::microsoft;
	else if(v.find("Qualcomm") != std::string::npos)
		return iware::gpu::vendor_t::qualcomm;
	else
		return iware::gpu::vendor_t::unknown;
}


std::vector<iware::gpu::device_properties_t> iware::gpu::device_properties() {
	IDXGIFactory4* dxgiFactory = nullptr;
	CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	std::vector<iware::gpu::device_properties_t> devices{};
	
	IDXGIAdapter3* dxgiAdapter = nullptr;
	IDXGIAdapter1* tmpDxgiAdapter = nullptr;
	UINT adapterIndex = 0;
	while (dxgiFactory->EnumAdapters1(adapterIndex, &tmpDxgiAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 adapterdesc;
		tmpDxgiAdapter ->GetDesc1(&adapterdesc);

		const auto device = iware::pci::identify_device(adapterdesc.VendorId, adapterdesc.DeviceId);
		std::string device_name = device.device_name ? device.device_name : iware::detail::narrowen_winstring(adapterdesc.Description);

		if (!dxgiAdapter && adapterdesc.Flags == 0)
		{
			tmpDxgiAdapter->QueryInterface(IID_PPV_ARGS(&dxgiAdapter));

			DXGI_QUERY_VIDEO_MEMORY_INFO memInfo = {};
			dxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);

			devices.push_back({ vendor_from_name(device.vendor_name), device_name,
				(std::uint64_t) memInfo.Budget,
				(std::uint64_t) memInfo.AvailableForReservation,
				(std::uint64_t) memInfo.CurrentUsage,
				(std::uint64_t) memInfo.CurrentReservation});

			
		}
		else {
			devices.push_back({ vendor_from_name(device.vendor_name), device_name,
				(std::uint64_t) adapterdesc.DedicatedVideoMemory,
				(std::uint64_t) adapterdesc.SharedSystemMemory,
				(std::uint64_t) 0,
				(std::uint64_t) 0
				});
		}
		tmpDxgiAdapter->Release();
		++adapterIndex;
	}
	dxgiAdapter->Release();
	return devices;
}

//Author: Jordi Gil Gonzalez
//Modified by avoid retrurning vectors
//Also, gather information accurately
void iware::gpu::device_properties(std::vector<iware::gpu::device_properties_t> &devices) {
	
	IDXGIFactory4* dxgiFactory = nullptr;
	CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

	IDXGIAdapter3* dxgiAdapter = nullptr;
	IDXGIAdapter1* tmpDxgiAdapter = nullptr;
	UINT adapterIndex = 0;
	while (dxgiFactory->EnumAdapters1(adapterIndex, &tmpDxgiAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 adapterdesc;
		tmpDxgiAdapter->GetDesc1(&adapterdesc);

		const auto device = iware::pci::identify_device(adapterdesc.VendorId, adapterdesc.DeviceId);
		std::string device_name = device.device_name ? device.device_name : iware::detail::narrowen_winstring(adapterdesc.Description);

		if (!dxgiAdapter && adapterdesc.Flags == 0)
		{
			tmpDxgiAdapter->QueryInterface(IID_PPV_ARGS(&dxgiAdapter));

			DXGI_QUERY_VIDEO_MEMORY_INFO memInfo = {};
			dxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);

			devices.push_back({ vendor_from_name(device.vendor_name), device_name,
				(std::uint64_t) memInfo.Budget,
				(std::uint64_t) memInfo.AvailableForReservation,
				(std::uint64_t) memInfo.Budget - memInfo.AvailableForReservation,
				(std::uint64_t) memInfo.CurrentReservation });

		}
		else {
			devices.push_back({ vendor_from_name(device.vendor_name), device_name,
				(std::uint64_t) adapterdesc.DedicatedVideoMemory,
				(std::uint64_t) adapterdesc.SharedSystemMemory,
				(std::uint64_t) adapterdesc.DedicatedVideoMemory - adapterdesc.SharedSystemMemory,
				(std::uint64_t) 0
				});
		}
		tmpDxgiAdapter->Release();
		++adapterIndex;
	}
	dxgiAdapter->Release();
	dxgiFactory->Release();
}

#endif
