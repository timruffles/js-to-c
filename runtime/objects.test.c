#include "objects.h"
#include "language.h"

void itCanCreateAnObject() {
  JsValue* obj = objectCreatePlain();
}

int main(int argc, char** argv) {
    itCanCreateAnObject();
    return 0;
}
