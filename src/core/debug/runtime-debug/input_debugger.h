#pragma once

#include "IGUI_drawable.h"
#include "../../input/input.h"
#include "../../input/borealis_devices.h"
#include <cstdio>

namespace Borealis::Runtime::Debug
{
	class InputDebugger : public IGUIDrawable
	{
	public:
		InputDebugger(Input::InputSystem* pInputSystem)
			: pInputSystem(pInputSystem), IGUIDrawable(true)
		{ }

		void UpdateDrawable() override
		{
			if (isOpen)
			{
				ImGui::Begin("Input Debugger", &isOpen, ImGuiWindowFlags_NoCollapse);
				OnGui();
				ImGui::End();
			}
		}

		void DrawXBOXOneDebugLayout(const Input::Gamepad& gamepad)
		{
			// TODO: Draw the image(s) of the controller and highlight / move the parts accordingly to the actual input

			ImGui::Text("Left Thumbstick (x: %f | y: %f)", gamepad.InputState.LeftThumbstickX, gamepad.InputState.LeftThumbstickY);
			ImGui::Text("Right Thumbstick (x: %f | y: %f)", gamepad.InputState.RightThumbstickX, gamepad.InputState.RightThumbstickY);

			ImGui::Text("Shoulder Trigger (Left: %f | Right: %f)", gamepad.InputState.LeftTrigger, gamepad.InputState.RightTrigger);
			
			ImGui::Text("Left Shoulder Button: %s", gamepad.InputState.ButtonState & Input::LEFT_SHOULDER ? "Pressed" : "Released");
			ImGui::Text("Right Shoulder Button: %s", gamepad.InputState.ButtonState & Input::RIGHT_SHOULDER ? "Pressed" : "Released");

			ImGui::Text("Y: %s", gamepad.InputState.ButtonState & Input::BUTTON_NORTH ? "Pressed" : "Released");
			ImGui::Text("X: %s", gamepad.InputState.ButtonState & Input::BUTTON_WEST ? "Pressed" : "Released");
			ImGui::Text("A: %s", gamepad.InputState.ButtonState & Input::BUTTON_SOUTH ? "Pressed" : "Released");
			ImGui::Text("B: %s", gamepad.InputState.ButtonState & Input::BUTTON_EAST ? "Pressed" : "Released");

			ImGui::Text("DPad Up: %s", gamepad.InputState.ButtonState & Input::DPAD_UP ? "Pressed" : "Released");
			ImGui::Text("DPad Left: %s", gamepad.InputState.ButtonState & Input::DPAD_LEFT ? "Pressed" : "Released");
			ImGui::Text("DPad Down: %s", gamepad.InputState.ButtonState & Input::DPAD_DOWN ? "Pressed" : "Released");
			ImGui::Text("DPad Right: %s", gamepad.InputState.ButtonState & Input::DPAD_RIGHT ? "Pressed" : "Released");

			ImGui::Text("Left Thumbstick: %s", gamepad.InputState.ButtonState & Input::LEFT_THUMBSTICK ? "Pressed" : "Released");
			ImGui::Text("Right Thumbstick: %s", gamepad.InputState.ButtonState & Input::RIGHT_THUMBSTICK ? "Pressed" : "Released");

			ImGui::Text("Select: %s", gamepad.InputState.ButtonState & Input::OPTIONS_LEFT ? "Pressed" : "Released");
			ImGui::Text("Start: %s", gamepad.InputState.ButtonState & Input::OPTIONS_RIGHT ? "Pressed" : "Released");

			ImGui::Text("Accelerometer: (x: %.2f | y: %.2f | z: %.2f)", gamepad.InputState.Accelerometer.x, gamepad.InputState.Accelerometer.y, gamepad.InputState.Accelerometer.z);
			ImGui::Text("Battery level: (%f)", gamepad.BatteryChargeLevel);
		}

		void DrawXBOX360DebugLayout(const Input::Gamepad& gamepad)
		{
			// TODO: Draw the image(s) of the controller and highlight / move the parts accordingly to the actual input
			
			ImGui::Text("Left Thumbstick (x: %f | y: %f)", gamepad.InputState.LeftThumbstickX, gamepad.InputState.LeftThumbstickY);
			ImGui::Text("Right Thumbstick (x: %f | y: %f)", gamepad.InputState.RightThumbstickX, gamepad.InputState.RightThumbstickY);

			ImGui::Text("Shoulder Trigger (Left: %f | Right: %f)", gamepad.InputState.LeftTrigger, gamepad.InputState.RightTrigger);

			ImGui::Text("Left Shoulder Button: %s", gamepad.InputState.ButtonState & Input::LEFT_SHOULDER ? "Pressed" : "Released");
			ImGui::Text("Right Shoulder Button: %s", gamepad.InputState.ButtonState & Input::RIGHT_SHOULDER ? "Pressed" : "Released");

			ImGui::Text("Y: %s", gamepad.InputState.ButtonState & Input::BUTTON_NORTH ? "Pressed" : "Released");
			ImGui::Text("X: %s", gamepad.InputState.ButtonState & Input::BUTTON_WEST ? "Pressed" : "Released");
			ImGui::Text("A: %s", gamepad.InputState.ButtonState & Input::BUTTON_SOUTH ? "Pressed" : "Released");
			ImGui::Text("B: %s", gamepad.InputState.ButtonState & Input::BUTTON_EAST ? "Pressed" : "Released");

			ImGui::Text("DPad Up: %s", gamepad.InputState.ButtonState & Input::DPAD_UP ? "Pressed" : "Released");
			ImGui::Text("DPad Left: %s", gamepad.InputState.ButtonState & Input::DPAD_LEFT ? "Pressed" : "Released");
			ImGui::Text("DPad Down: %s", gamepad.InputState.ButtonState & Input::DPAD_DOWN ? "Pressed" : "Released");
			ImGui::Text("DPad Right: %s", gamepad.InputState.ButtonState & Input::DPAD_RIGHT ? "Pressed" : "Released");

			ImGui::Text("Left Thumbstick: %s", gamepad.InputState.ButtonState & Input::LEFT_THUMBSTICK ? "Pressed" : "Released");
			ImGui::Text("Right Thumbstick: %s", gamepad.InputState.ButtonState & Input::RIGHT_THUMBSTICK ? "Pressed" : "Released");

			ImGui::Text("Select: %s", gamepad.InputState.ButtonState & Input::OPTIONS_LEFT ? "Pressed" : "Released");
			ImGui::Text("Start: %s", gamepad.InputState.ButtonState & Input::OPTIONS_RIGHT ? "Pressed" : "Released");

			ImGui::Text("Accelerometer: (x: %.2f | y: %.2f | z: %.2f)", gamepad.InputState.Accelerometer.x, gamepad.InputState.Accelerometer.y, gamepad.InputState.Accelerometer.z);
			ImGui::Text("Battery level: (%f)", gamepad.BatteryChargeLevel);
		}

		void DrawDualShockDebugLayout(const Input::Gamepad& gamepad)
		{
			// TODO: Draw the image(s) of the controller and highlight / move the parts accordingly to the actual input
		}

		void DrawDualSenseDebugLayout(const Input::Gamepad& gamepad)
		{
			// TODO: Draw the image(s) of the controller and highlight / move the parts accordingly to the actual input
			ImGui::Text("Left Thumbstick (x: %.2f | y: %.2f)", gamepad.InputState.LeftThumbstickX, gamepad.InputState.LeftThumbstickY);
			ImGui::Text("Right Thumbstick (x: %.2f | y: %.2f)", gamepad.InputState.RightThumbstickX, gamepad.InputState.RightThumbstickY);

			ImGui::Text("Shoulder Trigger (Left: %.2f | Right: %.2f)", gamepad.InputState.LeftTrigger, gamepad.InputState.RightTrigger);

			ImGui::Text("Left Shoulder Button: %s", gamepad.InputState.ButtonState & Input::LEFT_SHOULDER ? "Pressed" : "Released");
			ImGui::Text("Right Shoulder Button: %s", gamepad.InputState.ButtonState & Input::RIGHT_SHOULDER ? "Pressed" : "Released");

			ImGui::Text("Triangle: %s", gamepad.InputState.ButtonState & Input::BUTTON_NORTH ? "Pressed" : "Released");
			ImGui::Text("Square: %s", gamepad.InputState.ButtonState & Input::BUTTON_WEST ? "Pressed" : "Released");
			ImGui::Text("Cross: %s", gamepad.InputState.ButtonState & Input::BUTTON_SOUTH ? "Pressed" : "Released");
			ImGui::Text("Circle: %s", gamepad.InputState.ButtonState & Input::BUTTON_EAST ? "Pressed" : "Released");

			ImGui::Text("DPad Up: %s", gamepad.InputState.ButtonState & Input::DPAD_UP ? "Pressed" : "Released");
			ImGui::Text("DPad Left: %s", gamepad.InputState.ButtonState & Input::DPAD_LEFT ? "Pressed" : "Released");
			ImGui::Text("DPad Down: %s", gamepad.InputState.ButtonState & Input::DPAD_DOWN ? "Pressed" : "Released");
			ImGui::Text("DPad Right: %s", gamepad.InputState.ButtonState & Input::DPAD_RIGHT ? "Pressed" : "Released");

			ImGui::Text("Left Thumbstick: %s", gamepad.InputState.ButtonState & Input::LEFT_THUMBSTICK ? "Pressed" : "Released");
			ImGui::Text("Right Thumbstick: %s", gamepad.InputState.ButtonState & Input::RIGHT_THUMBSTICK ? "Pressed" : "Released");

			ImGui::Text("Create: %s", gamepad.InputState.ButtonState & Input::OPTIONS_LEFT ? "Pressed" : "Released");
			ImGui::Text("Options: %s", gamepad.InputState.ButtonState & Input::OPTIONS_RIGHT ? "Pressed" : "Released");
		
			ImGui::Text("Touchpad: %s", gamepad.InputState.ButtonState & Input::BUTTON_PAD? "Pressed" : "Released");
			ImGui::Text("PS Logo: %s", gamepad.InputState.ButtonState & Input::BUTTON_LOGO ? "Pressed" : "Released");

			ImGui::Text("Touchpad P1 (x: %.2f | y: %.2f)", gamepad.InputState.Touchpad1X, gamepad.InputState.Touchpad1Y);
			ImGui::Text("Touchpad P2 (x: %.2f | y: %.2f)", gamepad.InputState.Touchpad2X, gamepad.InputState.Touchpad2Y);
		
			ImGui::Text("Battery level: (%f)", gamepad.BatteryChargeLevel);

			ImGui::Spacing();
			ImGui::Text("Accelerometer: (x: %.2f | y: %.2f | z: %.2f)", gamepad.InputState.Accelerometer.x, gamepad.InputState.Accelerometer.y, gamepad.InputState.Accelerometer.z);

			DrawAccelerometerPlot(gamepad);
		}

		void DrawAccelerometerPlot(const Borealis::Input::Gamepad& gamepad)
		{
			// Fill plot with new values each frame
			static float accelerometerValuesX[30]{ 0 };
			static float accelerometerValuesY[30]{ 0 };
			static float accelerometerValuesZ[30]{ 0 };

			for (int i = 0; i < 29; ++i)
			{
				accelerometerValuesX[i] = accelerometerValuesX[i + 1];
			}

			for (int i = 0; i < 29; ++i)
			{
				accelerometerValuesY[i] = accelerometerValuesY[i + 1];
			}

			for (int i = 0; i < 29; ++i)
			{
				accelerometerValuesZ[i] = accelerometerValuesZ[i + 1];
			}

			accelerometerValuesX[29] = gamepad.InputState.Accelerometer.x;
			accelerometerValuesY[29] = gamepad.InputState.Accelerometer.y;
			accelerometerValuesZ[29] = gamepad.InputState.Accelerometer.z;

			ImGui::Spacing();

			ImGui::BeginGroup();

			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1,0,0,1));
			ImGui::PlotLines("X", accelerometerValuesX, 30, 0, 0, -2500.0f, 2500.0f, ImVec2(250, 30.0f));
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0, 1, 0, 1));
			ImGui::PlotLines("Y", accelerometerValuesY, 30, 0, 0, -2500.0f, 2500.0f, ImVec2(250, 30.0f));
			ImGui::PopStyleColor();
			
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.3, 0.3, 1, 1));
			ImGui::PlotLines("Z", accelerometerValuesZ, 30, 0, 0, -2500.0f, 2500.0f, ImVec2(250, 30.0f));
			ImGui::PopStyleColor();

			ImGui::EndGroup();
		}

		void DrawMouseDebugLayout(const Input::Mouse& mouse)
		{
			// TODO: Draw the image(s) of the mouse and highlight / move the parts accordingly to the actual input

			ImGui::Text("Primary Mouse buttons:(Left: %s | Right: %s)",
				mouse.InputState.buttonState & Input::BUTTON_LEFT ? "Pressed" : "Release",
				mouse.InputState.buttonState & Input::BUTTON_RIGHT ? "Pressed" : "Release");

			ImGui::Text("Middle Mouse button: (%s | WheelX: %f | WheelY: %f)",
				mouse.InputState.buttonState & Input::BUTTON_MIDDLE ? "Pressed" : "Release",
				mouse.InputState.WheelX, mouse.InputState.WheelY);

			ImGui::Text("Mouse Position: (x: %.2f | y: %.2f )", mouse.InputState.PositionX, mouse.InputState.PositionY);
		}

		void DrawKeyboardDebugLayout(const Input::Keyboard& keyboard)
		{
			// TODO: Draw the image of the keyboard and highlight / move the parts accordingly to the actual input
			// In the case of the keyboard, evaluate if a list of pressed buttons is a better debug format!
		}

		void OnGui() override
		{
			ImGui::SeparatorText("Device Connections");

			//ImGui::Text("Input Devices connected: %u", pInputSystem->GetAllDevices().size());
			ImGui::Spacing();

			ImGui::Text("Mouse connected: %u / 1", pInputSystem->GetMouse() == nullptr ? 0 : 1);
			ImGui::Text("Keyboards connected: %u / 1", pInputSystem->GetKeyboard() == nullptr ? 0 : 1);
			ImGui::Text("Gamepads connected: %lu / %u", pInputSystem->GetGamepads().size(), Input::MAX_GAMEPADS());
			
			//ImGui::Text("More stuff: %u", 123);

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SeparatorText("Input Devices");

			Types::uint8 deviceIdx = 0;

			if (ImGui::TreeNode("All Devices"))
			{
				std::set<Input::IInputDevice*> allDevices = pInputSystem->GetAllDevices();

				for (std::set<Input::IInputDevice*>::iterator it = allDevices.begin(); it != allDevices.end(); ++it)
				{
					Input::IInputDevice* device = *it;

					std::string displayName = '[' + std::to_string(deviceIdx) + ']' + " " + std::string(Types::ValueFromStringId(device->DisplayName));

					if (ImGui::CollapsingHeader(displayName.c_str()))	// <- This name is not unique yet, and the device ID is gibberish...
					{
						ImGui::SeparatorText("Device Info");

						// I know...
						std::string idtxt = "UNKNOWN DEVICE ID";
						sprintf(idtxt.data(), "Device ID: %X%X%X%X%X%X%X%X", 
							device->DeviceID[0]
							, device->DeviceID[1]
							, device->DeviceID[2]
							, device->DeviceID[3]
							, device->DeviceID[4]
							, device->DeviceID[5]
							, device->DeviceID[6]
							, device->DeviceID[7]
							);

						ImGui::Text("%s", idtxt.c_str());

						ImGui::Spacing();

						switch (device->DeviceType)
						{
							case Input::InputDeviceCategory::GAMEPAD:
							{
								ImGui::Text("Type: Gamepad");

								Input::Gamepad* gamepad = dynamic_cast<Input::Gamepad*>(device);
								
								switch (gamepad->VendorType)
								{
									case Input::GamepadType::XBOX_360:
									{
										DrawXBOX360DebugLayout(*gamepad);
										break;
									}
									case Input::GamepadType::XBOX_ONE:
									{
										DrawXBOXOneDebugLayout(*gamepad);
										break;
									}
									case Input::GamepadType::DUAL_SHOCK:
									{
										DrawDualShockDebugLayout(*gamepad);
										break;
									}
									case Input::GamepadType::DUAL_SENSE:
									{
										DrawDualSenseDebugLayout(*gamepad);
										break;
									}
									default:
									{
										break;
									}
								}

								break;
							}
							case Input::InputDeviceCategory::KEYBOARD:
							{
								ImGui::Text("Type: Keyboard");
								DrawKeyboardDebugLayout(*pInputSystem->GetKeyboard());
								break;
							}
							case Input::InputDeviceCategory::MOUSE:
							{
								ImGui::Text("Type: Mouse");
								DrawMouseDebugLayout(*pInputSystem->GetMouse());
								break;
							}
							default:
							{
								break;
							}
						}

						ImGui::Spacing();
					}

					++deviceIdx;
				}
				ImGui::TreePop();
			}
		}

		

	private:
		Input::InputSystem* pInputSystem;
	};
}