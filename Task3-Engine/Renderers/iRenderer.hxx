#include "../canvas.hxx"

namespace core {

class iRenderer {
public:
  virtual void draw() = 0;

private:
  Canvas canvas;
};
} // namespace core