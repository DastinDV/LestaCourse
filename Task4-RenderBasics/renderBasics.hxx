#pragma once
#include "../Task3-Engine/Renderers/lineRenderer.hxx"
#include "../Task3-Engine/Renderers/triangleRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"

#include <cmath>
#include <functional>
#include <iostream>
#include <memory>

struct CoolShader : core::gfx_program {
  double mouse_x{};
  double mouse_y{};
  double radius{};

  void set_uniforms(const core::Uniforms &a_uniforms) override {
    mouse_x = a_uniforms.f0;
    mouse_y = a_uniforms.f1;
    radius = a_uniforms.f2;
  }
  core::Vertex vertex_shader(const core::Vertex &v_in) override {
    core::Vertex out = v_in;

    double x = out.x;
    double y = out.y;

    double dx = x - mouse_x;
    double dy = y - mouse_y;
    if (dx * dx + dy * dy < radius * radius) {
      // un magnet from mouse
      double len = std::sqrt(dx * dx + dy * dy);
      if (len > 0) {
        // normalize vector from vertex to mouse pos
        double norm_dx = dx / len;
        double norm_dy = dy / len;
        // find position of point on radius from mouse pos in center
        double radius_pos_x = mouse_x + norm_dx * radius;
        double radius_pos_y = mouse_y + norm_dy * radius;
        // find middle point
        x = (x + radius_pos_x) / 2;
        y = (y + radius_pos_y) / 2;
      }
    }

    out.x = x;
    out.y = y;

    return out;
  }

  core::Color fragment_shader(const core::Vertex &v_in) override {
    core::Color out = v_in.color;
    return out;
  }
};

struct SpiralShader : core::gfx_program {
  double mouse_x{};
  double mouse_y{};
  double radius{};

  float deltaTime;
  bool isDrawUp = true;

  void set_uniforms(const core::Uniforms &a_uniforms) override {
    mouse_x = a_uniforms.f0;
    mouse_y = a_uniforms.f1;
    radius = a_uniforms.f2;
  }

  void set_deltaTime(float deltaTime) { this->deltaTime = deltaTime; }

  core::Vertex vertex_shader(const core::Vertex &v_in) override {
    core::Vertex out = v_in;

    double x = out.x;
    double y = out.y;

    double dx = x - mouse_x;
    double dy = y - mouse_y;

    double distance = sqrt(std::pow(dx, 2) + std::pow(dy, 2));
    double speed = 60.f;
    // inside circle.
    if (std::pow(distance, 2) < std::pow(radius * 3, 2)) {
      double val = /*(distance * 100 / radius) / 100*/ speed *
                   std::sin(M_PI * (M_PI / 180) * (deltaTime));
      isDrawUp ? y -= val : y += val;
      isDrawUp ? out.delta += val : out.delta -= val;
    }

    out.y = y;
    return out;
  }

  core::Color fragment_shader(const core::Vertex &v_in) override {
    core::Color out = v_in.color;

    out.r = static_cast<uint8_t>(v_in.color.r);
    out.g = static_cast<uint8_t>(v_in.color.g);
    out.b = static_cast<uint8_t>(v_in.color.b);

    double x = v_in.x;
    double y = v_in.y;
    double delta = v_in.delta;
    double dx = mouse_x - x;
    double dy = mouse_y - y;

    double distance = sqrt(std::pow(dx, 2) + std::pow(dy, 2));
    double intencity = distance / radius;

    double rings = radius * 4; // + 20 * std::sin((x + y + deltaTime));

    if (std::pow(distance, 2) < std::pow(rings, 2)) {
      if (!isDrawUp) {
        out.r = 255 * intencity;
        out.g = 0;
        out.b = 0;
      } else {
        out.r = 0;
        out.g = 255 * intencity;
        out.b = 0;
      }
    }

    return out;
  }
};

class RenderBasicsGame : public core::Game {
public:
  RenderBasicsGame();
  void Render() override;
  void Update(float deltaTime) override;
  void Init() override;
  void OnEvent(core::Event &event) override;
  ~RenderBasicsGame();

private:
  std::unique_ptr<core::LineRenderer> lineRenderer;
  std::unique_ptr<core::TriangleRenderer> triangleRenderer;
  std::unique_ptr<core::Canvas> canvas;
  std::vector<std::function<void()>> tests;

  int width = 640;
  int height = 480;
  void InitTestFunctions();

  CoolShader *coolShader;
  SpiralShader *spiralShader;

  std::vector<core::Vertex> vertexes2;
  core::Position first = {0, 0};
  std::vector<int> indexes;
};

IZ_DECLSPEC core::Game *CreateGame() { return new RenderBasicsGame(); }