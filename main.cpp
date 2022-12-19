/**
 * This file is part of the "Learn WebGPU for C++" book.
 *   https://github.com/eliemichel/LearnWebGPU
 * 
 * MIT License
 * Copyright (c) 2022 Elie Michel
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "glfw3webgpu.h"

#define WEBGPU_CPP_IMPLEMENTATION
#include "webgpu.hpp"

#include <GLFW/glfw3.h>

#include <wgpu.h> // wgpuTextureViewDrop

#include <iostream>
#include <cassert>

#define UNUSED(x) (void)x;

using namespace wgpu;

int main (int, char**) {
	Instance instance = createInstance(InstanceDescriptor{});
	if (!instance) {
		std::cerr << "Could not initialize WebGPU!" << std::endl;
		return 1;
	}

	if (!glfwInit()) {
		std::cerr << "Could not initialize GLFW!" << std::endl;
		return 1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);
	if (!window) {
		std::cerr << "Could not open window!" << std::endl;
		return 1;
	}

	std::cout << "Requesting adapter..." << std::endl;
	Surface surface = glfwGetWGPUSurface(instance, window);
	RequestAdapterOptions adapterOpts{};
	adapterOpts.compatibleSurface = surface;
	Adapter adapter = instance.requestAdapter(adapterOpts);
	std::cout << "Got adapter: " << adapter << std::endl;

	std::cout << "Requesting device..." << std::endl;
	DeviceDescriptor deviceDesc{};
	deviceDesc.label = "My Device";
	RequiredLimits requiredLimits{};
	requiredLimits.limits.maxBindGroups = 1;
	deviceDesc.requiredLimits = &requiredLimits;
	deviceDesc.defaultQueue.label = "The default queue";
	Device device = adapter.requestDevice(deviceDesc);
	std::cout << "Got device: " << device << std::endl;

	// Get the command queue, through which we send commands to the GPU
	Queue queue = device.getQueue();

	std::cout << "Creating swapchain device..." << std::endl;
	TextureFormat swapChainFormat = surface.getPreferredFormat(adapter);
	SwapChainDescriptor swapChainDesc = {};
	swapChainDesc.usage = TextureUsage::RenderAttachment;
	swapChainDesc.format = swapChainFormat;
	swapChainDesc.width = 640;
	swapChainDesc.height = 480;
	swapChainDesc.presentMode = WGPUPresentMode_Fifo;
	SwapChain swapChain = device.createSwapChain(surface, swapChainDesc);
	std::cout << "Swapchain: " << swapChain << std::endl;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		TextureView nextTexture = swapChain.getCurrentTextureView();
		if (!nextTexture) {
			std::cerr << "Cannot acquire next swap chain texture" << std::endl;
			return 1;
		}
		std::cout << "nextTexture: " << nextTexture << std::endl;

		CommandEncoderDescriptor commandEncoderDesc{};
		commandEncoderDesc.label = "Command Encoder";
		CommandEncoder encoder = device.createCommandEncoder(commandEncoderDesc);
		
		RenderPassDescriptor renderPassDesc{};
		renderPassDesc.colorAttachmentCount = 1;
		WGPURenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.view = nextTexture;
		renderPassColorAttachment.resolveTarget = 0;
		renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
		renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
		renderPassColorAttachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };
		renderPassDesc.colorAttachments = &renderPassColorAttachment;
		renderPassDesc.depthStencilAttachment = nullptr;
		renderPassDesc.timestampWriteCount = 0;
		RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);
		wgpuRenderPassEncoderEnd(renderPass);
		wgpuTextureViewDrop(nextTexture);

		CommandBufferDescriptor cmdBufferDescriptor {};
		cmdBufferDescriptor.label = "Command buffer";
		CommandBuffer command = encoder.finish(cmdBufferDescriptor);
		queue.submit(command);

		swapChain.present();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
