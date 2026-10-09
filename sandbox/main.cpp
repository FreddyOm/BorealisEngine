#include <borealis_engine.h>
#include <core/graphics/graphics.h>
#include <core/memory/memory.h>

using namespace Borealis::Core;
using namespace Borealis::Graphics;
using namespace Borealis::Runtime::Debug;
using namespace Borealis::Types;
using namespace Borealis::Input;
using namespace Borealis::Memory;

int main()
{
    {
        Application app = Application("Borealis Sandbox");

        while(app.IsRunning())
        {
            app.Update();
        }
    }

    ReportLiveHandles();

#ifdef BOREALIS_WIN
    ReportD3D12LiveObjects();    // The corresponding initialization is done in Graphics initialization code
                                 // -> dependency on ID3D12Device!
#endif
    // #endif

    return 0;
}
