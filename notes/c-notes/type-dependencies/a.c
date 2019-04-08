#include "b.h"
#include "a.h"

int main() {
  A a = (A) {
      .b = (B) {
          .dbl = 6
      }
  };

  return a.b.dbl > 4;
}
