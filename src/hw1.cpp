#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params)
{
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Vector3 bgColor = Vector3{0.5, 0.5, 0.5};
    for (int i = 0; i < (int)params.size(); i++)
    {
        if (params[i] == "-center")
        {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, img.height - y};
        }
        else if (params[i] == "-radius")
        {
            radius = std::stof(params[++i]);
        }
        else if (params[i] == "-color")
        {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }
    // silence warnings, feel free to remove it
    UNUSED(radius);
    UNUSED(color);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            Vector2 currPoint = Vector2{x + 0.5, y + 0.5};
            Vector2 vectToCenter = center - currPoint;
            img(x, y) = length(vectToCenter) <= radius ? color : bgColor;
        }
    }
    return img;
}

bool pointInLine(Vector2 q, std::vector<Vector2> polyline, Real stroke_width, bool is_closed)
{
    for (int i = 0; i < polyline.size(); i++)
    {
        Vector2 p0 = polyline[i];
        Vector2 p1 = polyline[(i + 1) % polyline.size()];
        Vector2 line = p1 - p0;
        Real l = dot(line, q - p0) / length(line);
        Vector2 q_prime = p0 + (l * (line / length(line)));

        Real distToCenter = length(q - p1);

        if (!is_closed && i + 1 >= polyline.size())
            return false;

        if (l > 0 && l < length(line) && length(q - q_prime) < (stroke_width / Real(2)))
            return true;
    }

    return false;
}

bool pointInShape(Vector2 q, std::vector<Vector2> polyline)
{
    int count = 0;

    for (int i = 0; i < polyline.size(); i++)
    {
        Vector2 p0 = polyline[i];
        Vector2 p1 = polyline[(i + 1) % polyline.size()];

        Real t = (q.y - p0.y) / (p1.y - p0.y);

        Real x = p0.x + (t * (p1.x - p0.x));

        if (x >= q.x && t >= 0 && t <= 1)
        {
            if (p0.y < p1.y)
                count += 1;
            if (p0.y > p1.y)
                count -= 1;
        }
    }

    return count != 0;
}

bool pointInJoint(Vector2 q, std::vector<Vector2> polyline, Real stroke_width, bool is_closed)
{
    for (int i = 0; i < polyline.size(); i++)
    {
        // Vector2 p0 = polyline[i];
        Vector2 p1 = polyline[(i + 1) % polyline.size()];

        Real distToCenter = length(q - p1);

        if (distToCenter <= stroke_width / 2)
        {
            if (!is_closed && i + 2 >= polyline.size())
                return false;
            return true;
        }
    }

    return false;
}

Image3 hw_1_2(const std::vector<std::string> &params)
{
    // Homework 1.2: render polylines
    if (params.size() == 0)
    {
        return Image3(0, 0);
    }

    Image3 img(640 /* width */, 480 /* height */);
    std::vector<Vector2> polyline;
    // is_closed = true indicates that the last point and
    // the first point of the polyline are connected
    bool is_closed = false;
    std::optional<Vector3> fill_color;
    std::optional<Vector3> stroke_color;
    Real stroke_width = 1;
    for (int i = 0; i < (int)params.size(); i++)
    {
        if (params[i] == "-points")
        {
            while (params.size() > i + 1 &&
                   params[i + 1].length() > 0 &&
                   params[i + 1][0] != '-')
            {
                Real x = std::stof(params[++i]);
                Real y = std::stof(params[++i]);
                polyline.push_back(Vector2{x, y});
            }
        }
        else if (params[i] == "--closed")
        {
            is_closed = true;
        }
        else if (params[i] == "-fill_color")
        {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            fill_color = Vector3{r, g, b};
        }
        else if (params[i] == "-stroke_color")
        {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            stroke_color = Vector3{r, g, b};
        }
        else if (params[i] == "-stroke_width")
        {
            stroke_width = std::stof(params[++i]);
        }
    }
    // silence warnings, feel free to remove it
    UNUSED(stroke_width);

    if (fill_color && !is_closed)
    {
        std::cout << "Error: can't have a non-closed shape with fill color." << std::endl;
        return Image3(0, 0);
    }

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            Vector2 q = Vector2{x + 0.5, img.height - y + 0.5};

            if (pointInLine(q, polyline, stroke_width, is_closed))
            {
                img(x, y) = stroke_color.has_value() ? *stroke_color : Vector3{0.5, 0.5, 0.5};
            }
            else if (pointInJoint(q, polyline, stroke_width, is_closed))
            {
                img(x, y) = stroke_color.has_value() ? *stroke_color : Vector3{0.5, 0.5, 0.5};
            }
            else if (pointInShape(q, polyline))
            {
                img(x, y) = fill_color.has_value() ? *fill_color : Vector3{0.5, 0.5, 0.5};
            }
            else
            {
                img(x, y) = Vector3{0.5, 0.5, 0.5};
            }
        }
    }
    return img;
}

Vector3 renderCircle(Vector2 currPoint, Circle circle, Vector3 bgColor)
{
    Vector2 vectToCenter = circle.center - currPoint;

    if (circle.fill_color.has_value())
    {
        if (length(vectToCenter) <= circle.radius)
            return *circle.fill_color;
    }

    if (circle.stroke_color.has_value())
    {
        if (length(vectToCenter) <= circle.radius + circle.stroke_width && length(vectToCenter) > circle.radius)
            return *circle.stroke_color;
    }

    return bgColor;
}

Vector3 renderPolyline(Vector2 currPoint, Polyline polyline, Vector3 bgColor)
{
    if (pointInLine(currPoint, polyline.points, polyline.stroke_width, polyline.is_closed))
    {
        if (polyline.stroke_color.has_value())
            return *polyline.stroke_color;
    }

    if (pointInJoint(currPoint, polyline.points, polyline.stroke_width, polyline.is_closed))
    {
        if (polyline.stroke_color.has_value())
            return *polyline.stroke_color;
    }

    if (pointInShape(currPoint, polyline.points))
    {
        if (polyline.fill_color.has_value())
            return *polyline.fill_color;
    }

    return bgColor;
}

Image3 hw_1_3(const std::vector<std::string> &params)
{
    // Homework 1.3: render multiple shapes
    if (params.size() == 0)
    {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            Vector2 currPoint = Vector2{x + 0.5, img.height - y + 0.5};
            img(x, y) = scene.background;

            for (int i = scene.shapes.size() - 1; i >= 0; i--)
            {
                if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                {
                    Vector3 color = renderCircle(currPoint, *circle, scene.background);

                    if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                        img(x, y) = color;
                }
                else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i]))
                {
                    Vector3 color = renderPolyline(currPoint, *polyline, scene.background);

                    if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                        img(x, y) = color;
                }
            }
        }
    }

    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params)
{
    // Homework 1.4: render transformed shapes
    if (params.size() == 0)
    {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            Vector2 currPoint = Vector2{x + 0.5, img.height - y + 0.5};
            img(x, y) = scene.background;

            for (int i = scene.shapes.size() - 1; i >= 0; i--)
            {
                if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                {
                    Vector3 objSpacePoint = inverse(circle->transform) * Vector3{currPoint.x, currPoint.y, (Real)1};
                    Vector3 color = renderCircle(Vector2{objSpacePoint.x, objSpacePoint.y}, *circle, scene.background);

                    if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                        img(x, y) = color;
                }
                else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i]))
                {
                    Vector3 objSpacePoint = inverse(polyline->transform) * Vector3{currPoint.x, currPoint.y, (Real)1};
                    Vector3 color = renderPolyline(Vector2{objSpacePoint.x, objSpacePoint.y}, *polyline, scene.background);

                    if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                        img(x, y) = color;
                }
            }
        }
    }
    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params)
{
    // Homework 1.5: antialiasing
    if (params.size() == 0)
    {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            Vector3 avgColor = scene.background;

            for (Real k = 0.125; k < 1; k += 0.25)
            {
                for (Real j = 0.125; j < 1; j += 0.25)
                {
                    Vector2 subpoint = Vector2{x + j, img.height - y + k};
                    bool notInShape = true;

                    for (int i = 0; i < scene.shapes.size(); i++)
                    {
                        if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                        {
                            Vector3 transformedPoint = inverse(circle->transform) * Vector3{subpoint.x, subpoint.y, Real(1)};
                            Vector3 color = renderCircle(Vector2{transformedPoint.x, transformedPoint.y}, *circle, scene.background);

                            if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                            {
                                avgColor += color;
                                notInShape = false;
                                break;
                            }
                        }
                        else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i]))
                        {
                            Vector3 transformedPoint = inverse(polyline->transform) * Vector3{subpoint.x, subpoint.y, Real(1)};
                            Vector3 color = renderPolyline(Vector2{transformedPoint.x, transformedPoint.y}, *polyline, scene.background);

                            if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                            {
                                avgColor += color;
                                notInShape = false;
                                break;
                            }
                        }
                    }

                    if (notInShape)
                    {
                        avgColor += scene.background;
                    }
                }
            }

            img(x, y) = avgColor / Real(16);
        }
    }

    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params)
{
    // Homework 1.6: alpha blending
    if (params.size() == 0)
    {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            Vector3 avgColor = Vector3{0, 0, 0};

            for (Real b = 0.125; b < 1; b += 0.25)
            {
                for (Real a = 0.125; a < 1; a += 0.25)
                {
                    Vector2 subpoint = Vector2{x + a, img.height - y + b};
                    bool notInShape = true;

                    Real lastOverlapAlpha = 0;
                    Vector3 lastOverlapColor;
                    Real n = 1;

                    Vector3 colorWithAlpha = Vector3{0, 0, 0};

                    for (int i = 0; i < scene.shapes.size(); i++)
                    {
                        if (auto *circle = std::get_if<Circle>(&scene.shapes[i]))
                        {
                            Vector3 transformedPoint = inverse(circle->transform) * Vector3{subpoint.x, subpoint.y, Real(1)};
                            Vector3 color = renderCircle(Vector2{transformedPoint.x, transformedPoint.y}, *circle, scene.background);

                            if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                            {
                                // avgColor += color;
                                notInShape = false;

                                Vector3 premult = (circle->fill_alpha) * (color);

                                if (i != 0)
                                    n *= (1 - lastOverlapAlpha);

                                lastOverlapAlpha = (circle->fill_alpha);
                                lastOverlapColor = (color);

                                colorWithAlpha += n * premult;
                            }
                        }
                        else if (auto *polyline = std::get_if<Polyline>(&scene.shapes[i]))
                        {
                            Vector3 transformedPoint = inverse(polyline->transform) * Vector3{subpoint.x, subpoint.y, Real(1)};
                            Vector3 color = renderPolyline(Vector2{transformedPoint.x, transformedPoint.y}, *polyline, scene.background);

                            if (color.x != scene.background.x || color.y != scene.background.y || color.z != scene.background.z)
                            {
                                // avgColor += color;
                                notInShape = false;

                                Vector3 premult = (polyline->fill_alpha) * (color);

                                if (i != 0)
                                    n *= (1 - lastOverlapAlpha);

                                lastOverlapAlpha = (polyline->fill_alpha);
                                lastOverlapColor = (color);

                                colorWithAlpha += n * premult;
                            }
                        }
                    }

                    if (notInShape)
                    {
                        avgColor += scene.background;
                    }
                    else
                    {
                        colorWithAlpha += n * (1 - lastOverlapAlpha) * scene.background;
                        avgColor += colorWithAlpha;
                    }
                }
            }

            img(x, y) = avgColor / Real(16);
        }
    }

    return img;
}
