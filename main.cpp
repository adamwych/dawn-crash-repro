#include <cstdio>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>

#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_glfw.h>

class Application {
public:
    void initialize();
    void requestAdapter();
    void requestDevice();
    void configureSurface();

    void run();
    void terminate();

    wgpu::Instance instance;
    wgpu::Adapter adapter;
    wgpu::Device device;
    wgpu::Queue queue;
    wgpu::Surface surface;

    GLFWwindow *window;
};

void Application::initialize() {
    if (glfwInit() != GLFW_TRUE) {
        printf("Failed to initialize GLFW.\n");
        return;
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);

    window = glfwCreateWindow(800, 600, "WebGPU GLFW", nullptr, nullptr);
    if (!window) {
        printf("Failed to create window.\n");
        return;
    }

    instance = wgpu::CreateInstance();
    requestAdapter();
}

void Application::requestAdapter() {
    wgpu::RequestAdapterOptions options;
    instance.RequestAdapter(&options, [](
        WGPURequestAdapterStatus status,
        WGPUAdapter adapter,
        char const * message,
        void * userdata
    ) {
        if (status == WGPURequestAdapterStatus_Error) {
            printf("Failed to get adapter: %s\n", message);
            return;
        }

        auto application = (Application*)userdata;
        application->adapter = wgpu::Adapter::Acquire(adapter);
        application->requestDevice();
    }, this);
}

void Application::requestDevice() {
    wgpu::DeviceDescriptor descriptor;
    adapter.RequestDevice(&descriptor, [](
        WGPURequestDeviceStatus status,
        WGPUDevice device,
        char const * message,
        void * userdata
    ) {
        if (status == WGPURequestDeviceStatus_Error) {
            printf("Failed to get device: %s\n", message);
            return;
        }

        auto application = (Application*)userdata;
        application->device = wgpu::Device::Acquire(device);
        application->device.SetUncapturedErrorCallback([](
            WGPUErrorType type,
            char const * message,
            void * userdata
        ) {
            printf("Error: %s\n", message);
        }, application);
        application->queue = application->device.GetQueue();
        application->configureSurface();
    }, this);
}

void Application::configureSurface() {
    wgpu::SurfaceConfiguration config;
    config.device = device;
    config.nextInChain = nullptr;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.format = wgpu::TextureFormat::BGRA8Unorm;
    config.width = 800;
    config.height = 600;
    config.presentMode = wgpu::PresentMode::Fifo;
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;

    surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
    surface.Configure(&config);

    run();
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        wgpu::SurfaceTexture surfaceTexture;
        surface.GetCurrentTexture(&surfaceTexture);
        
        auto surfaceTextureView = surfaceTexture.texture.CreateView();

        auto encoder = device.CreateCommandEncoder();
        
        auto renderPassDescriptor = wgpu::RenderPassDescriptor();
        renderPassDescriptor.nextInChain = nullptr;
        renderPassDescriptor.label = nullptr;
        renderPassDescriptor.depthStencilAttachment = nullptr;
        renderPassDescriptor.timestampWrites = nullptr;
        renderPassDescriptor.occlusionQuerySet = nullptr;

        auto colorAttachment = wgpu::RenderPassColorAttachment();
        colorAttachment.view = surfaceTextureView;
        colorAttachment.resolveTarget = nullptr;
        colorAttachment.loadOp = wgpu::LoadOp::Clear;
        colorAttachment.storeOp = wgpu::StoreOp::Store;
        colorAttachment.clearValue = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassDescriptor.colorAttachmentCount = 1;
        renderPassDescriptor.colorAttachments = &colorAttachment;

        auto renderPass = encoder.BeginRenderPass(&renderPassDescriptor);
        renderPass.End();

        auto commands = encoder.Finish();
        queue.Submit(1, &commands);

        surface.Present();

        instance.ProcessEvents();
        device.Tick();
    }

    terminate();
}

void Application::terminate() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char** argv) {
    Application *app = new Application();
    app->initialize();
    return 0;
}