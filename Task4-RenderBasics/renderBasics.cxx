#include "renderBasics.hxx"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

RenderBasicsGame::RenderBasicsGame() {}

void RenderBasicsGame::Init() {
  canvas = std::make_unique<core::Canvas>(width, height);
  lineRenderer = std::make_unique<core::LineRenderer>(*canvas);
  triangleRenderer = std::make_unique<core::TriangleRenderer>(*canvas);
  coolShader = new CoolShader();
  triangleRenderer->SetGFXProgram(coolShader);

  InitTestFunctions();
}

void RenderBasicsGame::Render() {
  int index = 0;
  // while (index < tests.size()) {
  //   std::cout << index << std::endl;
  //   tests[index++]();
  //   canvas->RenderToSDLWindow();
  //   canvas->Clear({0, 0, 0});
  //   using namespace std::chrono;
  //   std::this_thread::sleep_for(milliseconds(1500));
  // }

  tests[0]();
  canvas->RenderToSDLWindow();
  canvas->Clear({0, 0, 0});

  std::cout << "Hello Render from renderBasics" << std::endl;
}

void RenderBasicsGame::OnEvent(core::Event &event) {
  if (event.eventType == core::EventType::mouse_event &&
      event.mouseInfo.has_value()) {
    coolShader->set_uniforms(
        core::Uniforms{event.mouseInfo->xPos, event.mouseInfo->yPos, 40});
  }
}

void RenderBasicsGame::Update() {
  std::cout << "Hello Update from renderBasics" << std::endl;
}

void RenderBasicsGame::InitTestFunctions() {
  // Line drawing with Bresenhams algorithm.
  std::function<void(void)> CreateLinesInAllDirection = [this]() {
    lineRenderer->Draw({200, 200}, {300, 300}, {0, 255, 0});
    lineRenderer->Draw({200, 200}, {300, 100}, {255, 0, 0});
    lineRenderer->Draw({200, 200}, {100, 300}, {0, 0, 255});
    lineRenderer->Draw({200, 200}, {100, 100}, {255, 255, 255});
    lineRenderer->Draw({210, 200}, {300, 200}, {255, 0, 0});
    lineRenderer->Draw({200, 210}, {200, 300}, {255, 0, 0});
    lineRenderer->Draw({190, 200}, {100, 200}, {255, 0, 0});
    lineRenderer->Draw({200, 190}, {200, 100}, {255, 0, 0});

    canvas->SaveImage("lines.ppm");
  };

  /*
    // Triangles creation with vertexes vector
    std::function<void(void)> CreateTrianglesWithIndexBuf = [this]() {
      using namespace core;
      std::vector<Vertex> vertexes;

      Position v1 = {0, 0};
      Position v2 = {width, height};
      Position v3 = {0, height};
      int step = 10;
      for (int i = 0; i < 1; i++) {
        vertexes.push_back({v1, red});
        vertexes.push_back({v2, green});
        vertexes.push_back({v3, blue});
        v1.x += 10;
        v1.y += 20;
        v2.x -= 30;
        v2.y -= 10;
        v3.x += 10;
        v3.y -= 10;
      }

      std::vector<Vertex> vertexes1;
      Position v11 = {0, 0};
      Position v21 = {width, 0};
      Position v31 = {width, height};

      for (int i = 0; i < 16; i++) {
        vertexes1.push_back({v11, green});
        vertexes1.push_back({v21, green});
        vertexes1.push_back({v31, green});
        v11.x += 30;
        v11.y += 10;
        v21.x -= 10;
        v21.y += 10;
        v31.x -= 10;
        v31.y -= 20;
      }

      BitFlag flags;
      flags.SetFlag(ETriangleSettings::RASTERIZED);
      flags.SetFlag(ETriangleSettings::INTERPOLATED);
      triangleRenderer->Draw(vertexes, {0, 255, 0}, true, flags);
      flags.UnsetFlag(ETriangleSettings::RASTERIZED);
      triangleRenderer->Draw(vertexes1, {255, 0, 0}, false, flags);

      canvas->SaveImage("triangles.ppm");
    };
*/
  // Triangles with vertexes and indexes buffers
  std::function<void(void)> CreateIndexedBufferGrid = [this]() {
    using namespace core;
    std::vector<Vertex> vertexes2;
    Position first = {0, 0};
    std::vector<int> indexes;

    for (int y = 0; y < height; y += 20) {
      for (int x = 0; x < width; x += 40) {
        first.x = x;
        first.y = y;
        vertexes2.push_back({(double)first.x, (double)first.y, red});
      }
    }

    int lastRow = height / 20;
    int lastCol = width / 40;
    for (int y = 0; y < lastRow - 1; y++) {
      for (int x = 0; x < lastCol - 1; x++) {
        int index0 = y * lastCol + x;
        int index1 = y * lastCol + (x + 1);
        int index2 = (y + 1) * lastCol + x;
        int index3 = (y + 1) * lastCol + (x + 1);

        indexes.push_back(index0);
        indexes.push_back(index1);
        indexes.push_back(index3);

        indexes.push_back(index0);
        indexes.push_back(index2);
        indexes.push_back(index3);
      }
    }

    triangleRenderer->Draw(vertexes2, indexes, {0, 0, 255});

    // canvas->SaveImage("indexedTriangles.ppm");
  };

  /*
      // Rasterization of triangles
      std::function<void(void)> CreateSimpleTrianglesRasterization = [this]()
     { using namespace core; std::vector<Vertex> vertexes; Position v1 = {100,
     100}; Position v2 = {50, 300}; Position v3 = {300, 480};

        Position v4 = {200, 200};
        Position v5 = {150, 250};
        Position v6 = {400, 230};

        Position v7 = {320, 0};
        Position v8 = {0, 480};
        Position v9 = {640, 480};

        // vertexes.push_back({v1, red});
        // vertexes.push_back({v2, green});
        // vertexes.push_back({v3, blue});

        // vertexes.push_back({v4, red});
        // vertexes.push_back({v5, green});
        // vertexes.push_back({v6, blue});

        vertexes.push_back({v7, red});
        vertexes.push_back({v8, green});
        vertexes.push_back({v9, blue});

        BitFlag flags;
        flags.SetFlag(ETriangleSettings::RASTERIZED);
        flags.SetFlag(ETriangleSettings::INTERPOLATED);
        triangleRenderer->Draw(vertexes, {0, 255, 0}, true, flags);

        canvas->SaveImage("interpolated.ppm");
      };

      tests.push_back(CreateLinesInAllDirection);
      tests.push_back(CreateTrianglesWithIndexBuf);
      tests.push_back(CreateIndexedBufferGrid);
      tests.push_back(CreateSimpleTrianglesRasterization);
      */

  tests.push_back(CreateIndexedBufferGrid);
}

RenderBasicsGame::~RenderBasicsGame() { delete coolShader; }