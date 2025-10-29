#include "hw2.h"
#include "hw2_scenes.h"
#include "hw1.h"

using namespace hw2;

Vector3 calcCameraProj(Vector3 point)
{
    return Vector3{-point.x / point.z, -point.y / point.z, -1.0};
}

Vector2 calcScreenProj(Vector3 p, Image3 img, Real s, Real a)
{
    auto x = img.width * ((p.x + s * a) / (2 * s * a));
    auto y = img.height * ((p.y + s) / (2 * s));
    return Vector2{x, y};
}

Image3 hw_2_1(const std::vector<std::string> &params)
{
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Real aspectRatio = (Real)img.width / (Real)img.height;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Vector3 bgColor = Vector3{0.5, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++)
    {
        if (params[i] == "-s")
        {
            s = std::stof(params[++i]);
        }
        else if (params[i] == "-p0")
        {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        }
        else if (params[i] == "-p1")
        {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        }
        else if (params[i] == "-p2")
        {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        }
        else if (params[i] == "-color")
        {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
        else if (params[i] == "-znear")
        {
            z_near = std::stof(params[++i]);
        }
    }

    // Calculate camera projection points
    Vector3 p0_prime = calcCameraProj(p0);
    Vector3 p1_prime = calcCameraProj(p1);
    Vector3 p2_prime = calcCameraProj(p2);

    // Calculate screen space points
    Vector2 p0_prime_2 = calcScreenProj(p0_prime, img, s, aspectRatio);
    Vector2 p1_prime_2 = calcScreenProj(p1_prime, img, s, aspectRatio);
    Vector2 p2_prime_2 = calcScreenProj(p2_prime, img, s, aspectRatio);

    // Create polyline
    std::vector<Vector2> polyline;
    polyline.push_back(p0_prime_2);
    polyline.push_back(p1_prime_2);
    polyline.push_back(p2_prime_2);

    // Render polyline
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            // Check if any points are "behind" the camera (near-clipping plane)
            if (-p0.z < z_near || -p1.z < z_near || -p2.z < z_near)
            {
                img(x, y) = bgColor;
                continue;
            }

            Vector3 avgColor = bgColor;

            for (Real k = 0.125; k < 1; k += 0.25)
            {
                for (Real j = 0.125; j < 1; j += 0.25)
                {
                    Vector2 subpoint = Vector2{x + j, img.height - y + k};

                    // Check if subpoint is in shape & update avgColor
                    avgColor += pointInShape(subpoint, polyline) ? color : bgColor;
                }
            }

            // set img(x, y) to avgColor
            img(x, y) = avgColor / Real(16);
        }
    }
    return img;
}

std::vector<Vector2> createTriangleFromPoints(Vector3 p0, Vector3 p1, Vector3 p2, Image3 img, Real s, Real aspectRatio)
{
    // Calculate camera projection points
    Vector3 p0_prime = calcCameraProj(p0);
    Vector3 p1_prime = calcCameraProj(p1);
    Vector3 p2_prime = calcCameraProj(p2);

    // Calculate screen space points
    Vector2 p0_prime_2 = calcScreenProj(p0_prime, img, s, aspectRatio);
    Vector2 p1_prime_2 = calcScreenProj(p1_prime, img, s, aspectRatio);
    Vector2 p2_prime_2 = calcScreenProj(p2_prime, img, s, aspectRatio);

    // Create triangle
    std::vector<Vector2> triangle;
    triangle.push_back(p0_prime_2);
    triangle.push_back(p1_prime_2);
    triangle.push_back(p2_prime_2);

    return triangle;
}

Vector3 screenToCameraProj(Vector2 p, Image3 img, Real s, Real a)
{
    auto x = ((2 * s * a * p.x) / Real(img.width)) - (s * a);
    auto y = ((2 * s * p.y) / Real(img.height)) - (s);
    return Vector3{x, y, -1.0};
}

double calcBaryCoef(std::vector<Vector3> pts, double triangleArea)
{
    Vector3 v1 = pts[1] - pts[0];
    Vector3 v2 = pts[2] - pts[0];

    auto area = length(cross(v1, v2)) / 2.0;

    return area / triangleArea;
}

Image3 hw_2_2(const std::vector<std::string> &params)
{
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real aspectRatio = (Real)img.width / (Real)img.height;
    Real z_near = 1e-6; // distance of the near clipping plane
    Vector3 bgColor = Vector3{0.5, 0.5, 0.5};
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++)
    {
        if (params[i] == "-s")
        {
            s = std::stof(params[++i]);
        }
        else if (params[i] == "-znear")
        {
            z_near = std::stof(params[++i]);
        }
        else if (params[i] == "-scene_id")
        {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s);
    UNUSED(z_near);
    UNUSED(scene_id);

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    Image3 largeImg(4 * img.width, 4 * img.height);

    Image1 z_buffer(4 * img.width, 4 * img.height);
    for (int i = 0; i < z_buffer.width; i++)
    {
        for (int j = 0; j < z_buffer.height; j++)
        {
            z_buffer(i, j) = -10000.0;
            largeImg(i, j) = bgColor;
        }
    }

    // foreach triangle
    for (int i = 0; i < (int)mesh.faces.size(); i++)
    {
        // project the triangle
        Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
        Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
        Vector3 p2 = mesh.vertices[mesh.faces[i][2]];
        // std::vector<Vector2> triangle = createTriangleFromPoints(p0, p1, p2, img, s, aspectRatio);

        Vector3 p0_prime = calcCameraProj(p0);
        Vector3 p1_prime = calcCameraProj(p1);
        Vector3 p2_prime = calcCameraProj(p2);

        // Calculate screen space points
        Vector2 p0_prime_2 = calcScreenProj(p0_prime, img, s, aspectRatio);
        Vector2 p1_prime_2 = calcScreenProj(p1_prime, img, s, aspectRatio);
        Vector2 p2_prime_2 = calcScreenProj(p2_prime, img, s, aspectRatio);

        // Create triangle
        std::vector<Vector2> triangle;
        triangle.push_back(p0_prime_2);
        triangle.push_back(p1_prime_2);
        triangle.push_back(p2_prime_2);

        double triangleArea = length(cross(p0_prime - p1_prime, p0_prime - p2_prime)) / 2.0;

        for (int y = 0; y < img.height; y++)
        {
            for (int x = 0; x < img.width; x++)
            {
                int b = 0;
                for (Real k = 0.125; k < 1; k += 0.25)
                {
                    int a = 0;
                    for (Real j = 0.125; j < 1; j += 0.25)
                    {
                        Vector2 subpoint = Vector2{x + j, img.height - y + k};

                        if (pointInShape(subpoint, triangle))
                        {
                            Vector3 cameraProj = screenToCameraProj(subpoint, img, s, aspectRatio);

                            double b0_prime = calcBaryCoef({cameraProj, p1_prime, p2_prime}, triangleArea);
                            double b1_prime = calcBaryCoef({p0_prime, cameraProj, p2_prime}, triangleArea);
                            double b2_prime = calcBaryCoef({p0_prime, p1_prime, cameraProj}, triangleArea);

                            auto denominator = (b0_prime / p0.z) + (b1_prime / p1.z) + (b2_prime / p2.z);
                            double b0 = (b0_prime / p0.z) / denominator;
                            double b1 = (b1_prime / p1.z) / denominator;
                            double b2 = (b2_prime / p2.z) / denominator;

                            double depth = (b0 * p0.z) + (b1 * p1.z) + (b2 * p2.z);

                            if (depth > z_buffer((4 * x) + a, (4 * y) + b))
                            {
                                z_buffer((4 * x) + a, (4 * y) + b) = depth;
                                largeImg((4 * x) + a, (4 * y) + b) = mesh.face_colors[i];
                            }
                        }

                        a++;
                    }

                    b++;
                }
            }
        }
    }

    for (int j = 0; j < largeImg.height; j += 4)
    {
        for (int i = 0; i < largeImg.width; i += 4)
        {
            Vector3 addedColor = largeImg(i, j) + largeImg(i + 1, j) + largeImg(i + 2, j) + largeImg(i + 3, j) + largeImg(i, j + 1) + largeImg(i + 1, j + 1) + largeImg(i + 2, j + 1) + largeImg(i + 3, j + 1) + largeImg(i, j + 2) + largeImg(i + 1, j + 2) + largeImg(i + 2, j + 2) + largeImg(i + 3, j + 2) + largeImg(i, j + 3) + largeImg(i + 1, j + 3) + largeImg(i + 2, j + 3) + largeImg(i + 3, j + 3);
            img(i / 4, j / 4) = addedColor / Real(16);
        }
    }

    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params)
{
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real aspectRatio = (Real)img.width / (Real)img.height;
    Real z_near = 1e-6; // distance of the near clipping plane
    Vector3 bgColor = Vector3{0.5, 0.5, 0.5};
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++)
    {
        if (params[i] == "-s")
        {
            s = std::stof(params[++i]);
        }
        else if (params[i] == "-znear")
        {
            z_near = std::stof(params[++i]);
        }
        else if (params[i] == "-scene_id")
        {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s);
    UNUSED(z_near);
    UNUSED(scene_id);

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    Image3 largeImg(4 * img.width, 4 * img.height);

    Image1 z_buffer(4 * img.width, 4 * img.height);
    for (int i = 0; i < z_buffer.width; i++)
    {
        for (int j = 0; j < z_buffer.height; j++)
        {
            z_buffer(i, j) = -10000.0;
            largeImg(i, j) = bgColor;
        }
    }

    // foreach triangle
    for (int i = 0; i < (int)mesh.faces.size(); i++)
    {
        // Get vertex colors
        Vector3 c0 = mesh.vertex_colors[mesh.faces[i][0]];
        Vector3 c1 = mesh.vertex_colors[mesh.faces[i][1]];
        Vector3 c2 = mesh.vertex_colors[mesh.faces[i][2]];

        // project the triangle
        Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
        Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
        Vector3 p2 = mesh.vertices[mesh.faces[i][2]];
        // std::vector<Vector2> triangle = createTriangleFromPoints(p0, p1, p2, img, s, aspectRatio);

        Vector3 p0_prime = calcCameraProj(p0);
        Vector3 p1_prime = calcCameraProj(p1);
        Vector3 p2_prime = calcCameraProj(p2);

        // Calculate screen space points
        Vector2 p0_prime_2 = calcScreenProj(p0_prime, img, s, aspectRatio);
        Vector2 p1_prime_2 = calcScreenProj(p1_prime, img, s, aspectRatio);
        Vector2 p2_prime_2 = calcScreenProj(p2_prime, img, s, aspectRatio);

        // Create triangle
        std::vector<Vector2> triangle;
        triangle.push_back(p0_prime_2);
        triangle.push_back(p1_prime_2);
        triangle.push_back(p2_prime_2);

        double triangleArea = length(cross(p0_prime - p1_prime, p0_prime - p2_prime)) / 2.0;

        for (int y = 0; y < img.height; y++)
        {
            for (int x = 0; x < img.width; x++)
            {
                int b = 0;
                for (Real k = 0.125; k < 1; k += 0.25)
                {
                    int a = 0;
                    for (Real j = 0.125; j < 1; j += 0.25)
                    {
                        Vector2 subpoint = Vector2{x + j, img.height - y + k};

                        if (pointInShape(subpoint, triangle))
                        {
                            Vector3 cameraProj = screenToCameraProj(subpoint, img, s, aspectRatio);

                            double b0_prime = calcBaryCoef({cameraProj, p1_prime, p2_prime}, triangleArea);
                            double b1_prime = calcBaryCoef({p0_prime, cameraProj, p2_prime}, triangleArea);
                            double b2_prime = calcBaryCoef({p0_prime, p1_prime, cameraProj}, triangleArea);

                            auto denominator = (b0_prime / p0.z) + (b1_prime / p1.z) + (b2_prime / p2.z);
                            double b0 = (b0_prime / p0.z) / denominator;
                            double b1 = (b1_prime / p1.z) / denominator;
                            double b2 = (b2_prime / p2.z) / denominator;

                            double depth = (b0 * p0.z) + (b1 * p1.z) + (b2 * p2.z);

                            if (depth > z_buffer((4 * x) + a, (4 * y) + b))
                            {
                                z_buffer((4 * x) + a, (4 * y) + b) = depth;
                                Vector3 color = (b0 * c0) + (b1 * c1) + (b2 * c2);
                                largeImg((4 * x) + a, (4 * y) + b) = color;
                            }
                        }

                        a++;
                    }

                    b++;
                }
            }
        }
    }

    for (int j = 0; j < largeImg.height; j += 4)
    {
        for (int i = 0; i < largeImg.width; i += 4)
        {
            Vector3 addedColor = largeImg(i, j) + largeImg(i + 1, j) + largeImg(i + 2, j) + largeImg(i + 3, j) + largeImg(i, j + 1) + largeImg(i + 1, j + 1) + largeImg(i + 2, j + 1) + largeImg(i + 3, j + 1) + largeImg(i, j + 2) + largeImg(i + 1, j + 2) + largeImg(i + 2, j + 2) + largeImg(i + 3, j + 2) + largeImg(i, j + 3) + largeImg(i + 1, j + 3) + largeImg(i + 2, j + 3) + largeImg(i + 3, j + 3);
            img(i / 4, j / 4) = addedColor / Real(16);
        }
    }

    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params)
{
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0)
    {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    auto aspectRatio = (double)img.width / (double)img.height;

    Matrix4x4 P = Matrix4x4::identity();
    P(2, 2) = 0;
    P(3, 2) = -1;
    P(3, 3) = 0;
    P(3, 2) = 1;

    Matrix4x4 V = inverse(scene.camera.cam_to_world);

    // foreach mesh
    for (int m = 0; m < (int)scene.meshes.size(); m++)
    {
        TriangleMesh mesh = scene.meshes[m];

        // foreach triangle
        for (int i = 0; i < scene.meshes[m].faces.size(); i++)
        {
            // Get vertex colors
            Vector3 c0 = mesh.vertex_colors[mesh.faces[i][0]];
            Vector3 c1 = mesh.vertex_colors[mesh.faces[i][1]];
            Vector3 c2 = mesh.vertex_colors[mesh.faces[i][2]];

            // Get points of triangle
            Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
            Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
            Vector3 p2 = mesh.vertices[mesh.faces[i][2]];

            // Multiply by M then V then P to get cameraProj space
            Matrix4x4 T = P * V * mesh.model_matrix;
            Vector4 h0 = T * Vector4{p0.x, p0.y, p0.z, 1.0};
            Vector4 h1 = T * Vector4{p1.x, p1.y, p1.z, 1.0};
            Vector4 h2 = T * Vector4{p2.x, p2.y, p2.z, 1.0};

            Vector3 p0_prime = Vector3{h0.x, h0.y, -1.0};
            Vector3 p1_prime = Vector3{h1.x, h1.y, -1.0};
            Vector3 p2_prime = Vector3{h2.x, h2.y, -1.0};

            // Calculate screen space points
            Vector2 p0_prime_2 = calcScreenProj(p0_prime, img, scene.camera.s, aspectRatio);
            Vector2 p1_prime_2 = calcScreenProj(p1_prime, img, scene.camera.s, aspectRatio);
            Vector2 p2_prime_2 = calcScreenProj(p2_prime, img, scene.camera.s, aspectRatio);

            // Create triangle
            std::vector<Vector2> triangle;
            triangle.push_back(p0_prime_2);
            triangle.push_back(p1_prime_2);
            triangle.push_back(p2_prime_2);

            double triangleArea = length(cross(p0_prime - p1_prime, p0_prime - p2_prime)) / 2.0;

            for (int y = 0; y < img.height; y++)
            {
                for (int x = 0; x < img.width; x++)
                {
                    // Get cameraProj from screenSpace coords of pixel using P^-1

                    //
                }
            }
        }
    }

    return img;
}
