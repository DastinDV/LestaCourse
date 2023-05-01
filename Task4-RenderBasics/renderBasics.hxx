#pragma once
#include "../Task3-Engine/Renderers/lineRenderer.hxx"
#include "../Task3-Engine/Renderers/triangleRenderer.hxx"
#include "../Task3-Engine/engine.hxx"
#include "../Task3-Engine/game.hxx"

#include <cmath>
#include <functional>
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

class RenderBasicsGame : public core::Game {
public:
  RenderBasicsGame();
  void Render() override;
  void Update() override;
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
};

IZ_DECLSPEC core::Game *CreateGame() { return new RenderBasicsGame(); }