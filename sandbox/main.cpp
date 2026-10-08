#include <borealis_engine.h>
#include <core/graphics/graphics.h>
#include <core/memory/memory.h>

using namespace Borealis::Core;
using namespace Borealis::Graphics;
using namespace Borealis::Runtime::Debug;
using namespace Borealis::Types;
using namespace Borealis::Input;

int main()
{
    {
        Borealis::Core::Application app = Borealis::Core::Application("Borealis Sandbox");

        while(app.IsRunning())
        {
            app.Update();
        }
    }

    Borealis::Memory::ReportLiveHandles();

#ifdef BOREALIS_WIN
    Borealis::Graphics::ReportD3D12LiveObjects();    // The corresponding initialization is done in Graphics initialization code
                                                     // -> dependency on ID3D12Device!
#endif
    // #endif

    return 0;
}
