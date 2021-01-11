#include "qt/Application.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    qt::Application app(argc, argv);
    return app.run();
}
