#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include "shader.h"

// import glm
#include "3rdparty/glm/glm.hpp"
#include "3rdparty/glm/gtc/matrix_transform.hpp"
#include "3rdparty/glm/gtc/type_ptr.hpp"

using namespace hw3;

// camera
glm::vec3 cameraPos, cameraFront, cameraUp;
float deltaTime, lastTime;
float lastX, lastY;
float pitch, yaw;
bool firstMouse = true;
glm::mat4 initialCamToWorld, rotatedCamToWorld;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    glm::mat4 Ryaw = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), cameraUp);
    glm::mat4 Rpitch = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right);
    glm::mat4 R = Rpitch * Ryaw;
    rotatedCamToWorld = R * initialCamToWorld;
    cameraFront = glm::vec3(-rotatedCamToWorld[2][0], -rotatedCamToWorld[2][1], -rotatedCamToWorld[2][2]);
}

void processInput(GLFWwindow *window)
{
    // close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // camera movement
    const float cameraSpeed = 50.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void hw_3_1(const std::vector<std::string> &params)
{
    // HW 3.1: Open a window using GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "balboa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // rendering
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // check & call events, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return;
}

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "uniform float rotation;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   vec3 rotatedPos = vec3(aPos);"
                                 "   rotatedPos.x = aPos.x * cos(rotation) - aPos.y * sin(rotation);"
                                 "   rotatedPos.y = aPos.x * sin(rotation) + aPos.y * cos(rotation);"
                                 "   gl_Position = vec4(rotatedPos, 1.0);\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\0";

void hw_3_2(const std::vector<std::string> &params)
{
    // HW 3.2: Render a single 2D triangle
    // Setup window + glfw (from HW 1)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "balboa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // define vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    // VBO & VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check if compilation of vertex shader was successful
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check if compilation of fragment shader was successful
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    // shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check if linking shader program was successful
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAOs
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // rendering
        // clear color buffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // activate shader
        glUseProgram(shaderProgram);

        // TODO: update the uniform rotation
        float rotateSpeed = 2.0f;
        float timeValue = glfwGetTime();
        float rotation = (timeValue / 60.0f) * 2 * M_PI * rotateSpeed;
        int uniformRotationLocation = glGetUniformLocation(shaderProgram, "rotation");
        glUniform1f(uniformRotationLocation, rotation);

        // render the triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // check & call events, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return;
}

void hw_3_3(const std::vector<std::string> &params)
{
    // HW 3.3: Render a scene
    if (params.size() == 0)
    {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // Setup window + glfw (from HW 1)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(scene.camera.resolution.x, scene.camera.resolution.y, "balboa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, scene.camera.resolution.x, scene.camera.resolution.y);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // enable depth testing & gamma correction
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // VBO, VAO & EBO
    int numMeshes = scene.meshes.size();
    unsigned int VAO[numMeshes], VBO_vertex[numMeshes], VBO_color[numMeshes], EBO[numMeshes];

    glGenVertexArrays(numMeshes, VAO);
    glGenBuffers(numMeshes, EBO);
    glGenBuffers(numMeshes, VBO_vertex);
    glGenBuffers(numMeshes, VBO_color);

    for (int i = 0; i < numMeshes; i++)
    {
        // bind VAO and EBO
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);

        // vertex VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex[i]);
        int verticesSize = 3 * scene.meshes[0].vertices.size();
        float vertices[verticesSize] = {};
        for (int j = 0; j < (int)scene.meshes[i].vertices.size(); j++)
        {
            vertices[(3 * j) + 0] = scene.meshes[i].vertices[j].x;
            vertices[(3 * j) + 1] = scene.meshes[i].vertices[j].y;
            vertices[(3 * j) + 2] = scene.meshes[i].vertices[j].z;
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // color VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color[i]);

        int vColorSize = 3 * scene.meshes[i].vertex_colors.size();
        float vColor[vColorSize];
        for (int j = 0; j < (int)scene.meshes[i].vertex_colors.size(); j++)
        {
            vColor[(3 * j) + 0] = scene.meshes[i].vertex_colors[j].x;
            vColor[(3 * j) + 1] = scene.meshes[i].vertex_colors[j].y;
            vColor[(3 * j) + 2] = scene.meshes[i].vertex_colors[j].z;
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(vColor), vColor, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);

        // EBO
        int indicesSize = 3 * scene.meshes[i].faces.size();
        unsigned int indices[indicesSize];
        for (int j = 0; j < (int)scene.meshes[i].faces.size(); j++)
        {
            indices[(3 * j) + 0] = scene.meshes[i].faces[j].x;
            indices[(3 * j) + 1] = scene.meshes[i].faces[j].y;
            indices[(3 * j) + 2] = scene.meshes[i].faces[j].z;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // unbind VBO, VAO & EBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // shader class
    Shader shader = Shader("C:/Users/Aslan Chan/Classes/2025-2026/CSE 167/cse167_balboa_public/src/shaders/shader.vs", "C:/Users/Aslan Chan/Classes/2025-2026/CSE 167/cse167_balboa_public/src/shaders/shader.fs");

    // Set projection matrix
    float aspectRatio = (float)scene.camera.resolution.x / (float)scene.camera.resolution.y;
    glm::mat4 projection = glm::mat4(1.0f);
    projection[0][0] = 1.0f / (aspectRatio * scene.camera.s);
    projection[1][1] = 1.0f / scene.camera.s;
    projection[2][2] = -scene.camera.z_far / (scene.camera.z_far - scene.camera.z_near);
    projection[2][3] = -1.0f;
    projection[3][2] = -(scene.camera.z_far * scene.camera.z_near) / (scene.camera.z_far - scene.camera.z_near);
    projection[3][3] = 0.0f;

    // set initial cameraPos, cameraUp & cameraFront
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            initialCamToWorld[i][j] = scene.camera.cam_to_world(j, i);
        }
    }

    cameraPos = glm::vec3(scene.camera.cam_to_world(0, 3), scene.camera.cam_to_world(1, 3), scene.camera.cam_to_world(2, 3));
    cameraUp = glm::vec3(scene.camera.cam_to_world(0, 1), scene.camera.cam_to_world(1, 1), scene.camera.cam_to_world(2, 1));
    cameraFront = glm::vec3(-initialCamToWorld[2][0], -initialCamToWorld[2][1], -initialCamToWorld[2][2]);

    lastX = scene.camera.resolution.x / 2.0f;
    lastY = scene.camera.resolution.y / 2.0f;

    while (!glfwWindowShouldClose(window))
    {
        // handle delta time
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // input
        processInput(window);

        // create view matrix
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // clear color buffer
        glClearColor(scene.background.x, scene.background.y, scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use shader
        shader.use();

        // rendering loop
        for (int n = 0; n < numMeshes; n++)
        {
            // create model matrix
            glm::mat4 model = glm::mat4(1.0f);
            Matrix4x4f modelMatrix = scene.meshes[n].model_matrix;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    model[i][j] = modelMatrix(j, i);
                }
            }

            // update uniform MVP matrices
            shader.setMat4("model", model);
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);

            // render object
            glBindVertexArray(VAO[n]);
            glDrawElements(GL_TRIANGLES, 3 * scene.meshes[n].faces.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // check & call events, swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate resources
    glDeleteVertexArrays(numMeshes, VAO);
    glDeleteBuffers(numMeshes, VBO_vertex);
    glDeleteBuffers(numMeshes, VBO_color);
    glDeleteBuffers(numMeshes, EBO);

    glfwTerminate();
    return;
}

void hw_3_4(const std::vector<std::string> &params)
{
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0)
    {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}
