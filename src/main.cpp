////////////////////////////////////////////////////////////////////////////////
//
// Minimalistic Vulkan Triangle sample
//
// 

// vulkan utilities.
#include "vku.hpp"

class triangle_example : public vku::window
{
public:
  struct {
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
  } uniform_data;

  vku::buffer vertex_buffer;
  vku::buffer index_buffer;
  vku::buffer uniform_buffer;

  vku::descriptorPool descPool;
  vku::pipeline pipe;

  vku::shaderModule vertexShader;
  vku::shaderModule fragmentShader;
  size_t num_indices;

  static const int VERTEX_BUFFER_BIND_ID = 0;


  triangle_example() : vku::window(false, 1280, 720, -2.5f, "triangle") {

    // Vertices
    struct Vertex { float pos[3]; float col[3]; };

    static const Vertex vertex_data[] = {
      { { 1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
      { { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
      { { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f} }
    };

    vertex_buffer = vku::buffer(device(), (void*)vertex_data, sizeof(vertex_data), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    // Indices
    static const uint32_t index_data[] = { 0, 1, 2 };
    index_buffer = vku::buffer(device(), (void*)index_data, sizeof(index_data), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    num_indices = 3;

    // Binding state
    vku::pipelineCreateHelper pipeHelper;
    pipeHelper.binding(VERTEX_BUFFER_BIND_ID, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
    pipeHelper.attrib(0, VERTEX_BUFFER_BIND_ID, VK_FORMAT_R32G32B32_SFLOAT, 0);
    pipeHelper.attrib(1, VERTEX_BUFFER_BIND_ID, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3);

    uniform_buffer = vku::buffer(device(), (void*)nullptr, sizeof(uniform_data), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    
    vertexShader = vku::shaderModule(device(), "data/shaders/triangle.vert", VK_SHADER_STAGE_VERTEX_BIT);
    fragmentShader = vku::shaderModule(device(), "data/shaders/triangle.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

    pipeHelper.uniformBuffers(1, VK_SHADER_STAGE_VERTEX_BIT);

    pipeHelper.shader(vertexShader, VK_SHADER_STAGE_VERTEX_BIT);
    pipeHelper.shader(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT);

    pipe = vku::pipeline(device(), swapChain().renderPass(), pipelineCache(), pipeHelper);

    descPool = vku::descriptorPool(device());

    pipe.allocateDescriptorSets(descPool);
    pipe.updateDescriptorSets(uniform_buffer);

    for (int32_t i = 0; i < swapChain().imageCount(); ++i) {
      const vku::cmdBuffer &cmdbuf = drawCmdBuffer(i);
      cmdbuf.begin(swapChain().renderPass(), swapChain().frameBuffer(i), width(), height());

      cmdbuf.bindPipeline(pipe);
      cmdbuf.bindVertexBuffer(vertex_buffer, VERTEX_BUFFER_BIND_ID);
      cmdbuf.bindIndexBuffer(index_buffer);
      cmdbuf.drawIndexed((uint32_t)num_indices, 1, 0, 0, 1);

      cmdbuf.end(swapChain().image(i));
    }

    updateUniformBuffers();
  }

  void updateUniformBuffers()
  {
    uniform_data.projectionMatrix = defaultProjectionMatrix();
    uniform_data.viewMatrix = defaultViewMatrix();
    uniform_data.modelMatrix = defaultModelMatrix();

    void *dest = uniform_buffer.map();
    memcpy(dest, &uniform_data, sizeof(uniform_data));
    uniform_buffer.unmap();
  }

  void render() override
  {
    device().waitIdle();
    present();
    device().waitIdle();
  }

  void viewChanged() override
  {
    // This function is called by the base example class 
    // each time the view is changed by user input
    updateUniformBuffers();
  }
};



int main(const int argc, const char *argv[]) {
  try {
    triangle_example my_example;

    while (vku::window::poll()) {
      if (my_example.windowIsClosed()) {
        break;
      }
      my_example.render();
    }
  } catch(std::runtime_error &e) {
    printf("fail: %s\n", e.what());
  }

  return 0;
}
