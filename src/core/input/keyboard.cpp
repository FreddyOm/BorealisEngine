#include "keyboard.h"
//#include "../debug/logger.h"

namespace Borealis::Input
{
	using namespace Borealis::Types;
	using namespace Borealis::Debug;


	//void Keyboard::DetectButtonUpDownEvents()
	//{
	//	std::bitset<256> buttonDelta = keyboardKeyStates ^ prevKeyboardKeyStates;

	//	std::bitset<256> buttonDownEvents = buttonDelta & keyboardKeyStates;
	//	std::bitset<256> buttonUpEvents = buttonDelta & (~keyboardKeyStates);

	//	// TODO: Process buttonDownEvents and buttonUpEvents to trigger the appropriate actions.
	//
	//	for(uint8 buttonIdx = 0; buttonIdx < buttonDownEvents.count(); ++buttonIdx)
	//	{
	//		if (buttonDownEvents.test(buttonIdx) == 0)
	//			continue;
	//		else
	//		{
	//			// Button down event for buttonIdx
	//		}
	//	}
	//}
}