#if FARO_PLATFORM_WINDOWS
#include <MainThread.hpp>

int main()
{
    Faro::MainThread app;
    return app.Run();
}
#endif
