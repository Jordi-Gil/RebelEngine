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


#pragma once


#include <infoware/linkage.hpp>

#include <cstdint>
#include <string>
#include <vector>


namespace iware {
	namespace gpu {
		enum class vendor_t {
			intel,
			amd,
			nvidia,
			microsoft,
			qualcomm,
			unknown,
		};

		struct device_properties_t {
			vendor_t vendor;
			std::string name;
			std::uint64_t budget_memory;
			std::uint64_t available_memory;
			std::uint64_t usage_memory;
			std::uint64_t reserved_memory;
		};


		void device_properties(std::vector<iware::gpu::device_properties_t>& devices);

		/// Returns all GPU's properties.
		INFOWARE_API_LINKAGE std::vector<device_properties_t> device_properties();
	}  // namespace gpu
}  // namespace iware
