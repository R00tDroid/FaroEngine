#if FARO_OS_WINDOWS
#include <MainThread.hpp>

int main()
{
    Faro::MainThread app;
    return app.Run();
}
#endif
