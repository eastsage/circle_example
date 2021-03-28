#include "context.h"

ContextUPtr Context::Create() {
  auto context = ContextUPtr(new Context());
  if (!context->Init())
    return nullptr;
  return std::move(context);
}

bool Context::Init() {

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());
 
    glClearColor(0.0f, 0.5f, 0.5f, 0.0f);

    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glDrawElements(GL_TRIANGLE_STRIP, m_indexCount, GL_UNSIGNED_INT, 0);
}

void Context::CreateCircle(float outerRadius, float innerRadius, int segment, int startAngle, int endAngle,
    float red, float green, float blue) {
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    const float pi = 3.141592f;
    for (int i = 0; i < segment ; i++) {
        float angle = (360.0f / segment * i) * pi / 180.0f;
        angle += (startAngle * pi / 180.0f);

        if (angle > endAngle * pi / 180.0f)
            break;

        float outer_x = cosf(angle) * outerRadius;
        float outer_y = sinf(angle) * outerRadius;
        float inner_x = cosf(angle) * innerRadius;
        float inner_y = sinf(angle) * innerRadius;
        
        vertices.push_back(inner_x);
        vertices.push_back(inner_y);
        vertices.push_back(0.0f);
        vertices.push_back(outer_x);
        vertices.push_back(outer_y);
        vertices.push_back(0.0f);
    }

    for (int i = 0; i < segment * 2; i++){
            if (i == segment * 2 - 1){
                indices.push_back(i);
                indices.push_back(0);
                indices.push_back(1);
            }
            else
                indices.push_back(i);
    }
    
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER,
        GL_STATIC_DRAW, vertices.data(), sizeof(float) * vertices.size());
    
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER,
        GL_STATIC_DRAW, indices.data(), sizeof(uint32_t) * indices.size());

    m_indexCount = (int)indices.size();

    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, red, green, blue, 1.0f);
}