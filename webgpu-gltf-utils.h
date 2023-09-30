/**
 * Conversion utilities from GLTF to WebGPU
 */
#pragma once

#include "tiny_gltf.h"

#include <webgpu/webgpu.hpp>

namespace wgpu::gltf {

// Return Undefined if not supported
wgpu::VertexFormat vertexFormatFromAccessor(const tinygltf::Accessor& accessor);

// Return Undefined if not supported
wgpu::IndexFormat indexFormatFromAccessor(const tinygltf::Accessor& accessor);

// Return Force32 if not supported
wgpu::FilterMode filterModeFromGltf(int tinygltfFilter);

// Return Force32 if not supported
wgpu::MipmapFilterMode mipmapFilterModeFromGltf(int tinygltfFilter);

// Return Force32 if not supported
wgpu::AddressMode addressModeFromGltf(int tinygltfWrap);

// Return Undefined if not supported
// If useClosestMatch is true, return a format that can contain the provided image, even though it has additional channels
wgpu::TextureFormat textureFormatFromGltfImage(const tinygltf::Image& image, bool useClosestMatch = false);

} // namespace wgpu::gltf
