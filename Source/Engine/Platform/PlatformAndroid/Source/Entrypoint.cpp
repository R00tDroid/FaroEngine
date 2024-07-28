#if FARO_OS_ANDROID
#include <MainThread.hpp>

int main()
{
    Faro::MainThread app;
    return app.Run();
}
#endif
