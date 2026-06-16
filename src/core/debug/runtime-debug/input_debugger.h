#pragma once

#include "IGUI_drawable.h"
#include "../../input/input.h"
#include "../../input/borealis_devices.h"
#include "../../memory/ref_cnt_auto_ptr.h"
#include "../../graphics/helpers/texture.h"
#include "../../math/math.h"
#include <cstdio>

namespace Borealis::Runtime::Debug
{
	float g_debugImageScale = 1.0f;

	class InputDebugger : public IGUIDrawable
	{
	public:
		InputDebugger(Input::InputSystem* pInputSystem, Memory::RefCntAutoPtr<Graphics::Texture> debugTexAtlas)
			: pInputSystem(pInputSystem), m_DebugTexAtlas(debugTexAtlas), IGUIDrawable(true)
		{ }

		~InputDebugger() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(InputDebugger)
		BOREALIS_DELETE_MOVE_CONSTRUCT(InputDebugger)
		BOREALIS_DELETE_COPY_ASSIGN(InputDebugger)
		BOREALIS_DELETE_MOVE_ASSIGN(InputDebugger)


		void UpdateDrawable() override
		{
			if (isOpen)
			{
				ImGui::Begin("Input Debugger", &isOpen, ImGuiWindowFlags_NoCollapse);
				OnGui();
				ImGui::End();
			}
		}

	private:

		constexpr float UVFromCoordinate(const Types::uint16 coord)
		{
			return (float)coord / 1024.0f;
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

			ImGui::Text("Battery level: (%f)", gamepad.BatteryChargeLevel);
		}

		void DrawXBOX360DebugLayout(const Input::Gamepad& gamepad)
		{			
			#ifdef BOREALIS_WIN	// Currently only D3D12 backend is supported, and the rendering resources are only available for win at the moment

			ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
			ImVec2 lThumPos = ImVec2(gamepad.InputState.LeftThumbstickX, gamepad.InputState.LeftThumbstickY);
			ImVec2 rThumPos = ImVec2(gamepad.InputState.RightThumbstickX, gamepad.InputState.RightThumbstickY);


			ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(514 * g_debugImageScale, 595 * g_debugImageScale), ImVec2(0, 0), ImVec2(0.501, 0.581));

			// --------------------- Drawing individual parts of the same atlas image texture to represent button states ---------------------

			// LT
			ImGui::SetCursorScreenPos({ startCursorPos.x + 77 * g_debugImageScale , startCursorPos.y + 36 * g_debugImageScale });
			ImGui::ImageWithBg((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(68 * g_debugImageScale, 83 * g_debugImageScale), 
				ImVec2(0, 0.592), ImVec2(0.066, 0.673), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, gamepad.InputState.LeftTrigger)); // TODO: Use trigger value to make opaque

			// RT
			ImGui::SetCursorScreenPos({ startCursorPos.x + 366 * g_debugImageScale , startCursorPos.y + 36 * g_debugImageScale });
			ImGui::ImageWithBg((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(68 * g_debugImageScale, 83 * g_debugImageScale), 
				ImVec2(0.067, 0.592), ImVec2(0.133, 0.673), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, gamepad.InputState.RightTrigger)); // TODO: Use trigger value to make opaque


			// L
			ImGui::SetCursorScreenPos({ (startCursorPos.x + 85 * g_debugImageScale) + m_ThumbstickMovementOffset * lThumPos.x * g_debugImageScale, 
				(startCursorPos.y + 320 * g_debugImageScale) - m_ThumbstickMovementOffset * lThumPos.y * g_debugImageScale });
			ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(80 * g_debugImageScale, 80 * g_debugImageScale), 
				ImVec2(0.408, 0.597), ImVec2(0.486, 0.675));	// Stick itself

			// L Button
			if (gamepad.InputState.ButtonState & Input::LEFT_THUMBSTICK)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 100 * g_debugImageScale) + m_ThumbstickMovementOffset * lThumPos.x * g_debugImageScale, 
					(startCursorPos.y + 332 * g_debugImageScale) - m_ThumbstickMovementOffset * lThumPos.y * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(52 * g_debugImageScale, 52 * g_debugImageScale), 
					ImVec2(0, UVFromCoordinate(691)), ImVec2(UVFromCoordinate(52), UVFromCoordinate(745)));	// Pressed tex
			}
			
			// R
			ImGui::SetCursorScreenPos({ (startCursorPos.x + 280 * g_debugImageScale) + m_ThumbstickMovementOffset * rThumPos.x * g_debugImageScale, 
				(startCursorPos.y + 399 * g_debugImageScale) - m_ThumbstickMovementOffset * rThumPos.y * g_debugImageScale });
			ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(80 * g_debugImageScale, 80 * g_debugImageScale), 
				ImVec2(0.408, 0.597), ImVec2(0.486, 0.675));	// Stick itself

			// R Button
			if (gamepad.InputState.ButtonState & Input::RIGHT_THUMBSTICK)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 295 * g_debugImageScale) + m_ThumbstickMovementOffset * rThumPos.x * g_debugImageScale, 
					(startCursorPos.y + 411 * g_debugImageScale) - m_ThumbstickMovementOffset * rThumPos.y * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(52 * g_debugImageScale, 52 * g_debugImageScale), 
					ImVec2(0, UVFromCoordinate(691)), ImVec2(UVFromCoordinate(52), UVFromCoordinate(745)));	// Pressed tex
			}

			// LT
			if (gamepad.InputState.ButtonState & Input::LEFT_SHOULDER)
			{
				// LB 1 (top view)
				ImGui::SetCursorScreenPos({ startCursorPos.x + 70 * g_debugImageScale , startCursorPos.y + 255 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(112 * g_debugImageScale, 39 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(134), UVFromCoordinate(610)), ImVec2(UVFromCoordinate(246), UVFromCoordinate(649)));	// Pressed tex

				// LB 1 (front view)
				ImGui::SetCursorScreenPos({ startCursorPos.x + 76 * g_debugImageScale , startCursorPos.y + 122 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(136 * g_debugImageScale, 39 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(136), UVFromCoordinate(651)), ImVec2(UVFromCoordinate(272), UVFromCoordinate(690)));	// Pressed tex

			}
			
			// RT
			if (gamepad.InputState.ButtonState & Input::RIGHT_SHOULDER)
			{
				// RB 1 (top view)
				ImGui::SetCursorScreenPos({ startCursorPos.x + 326 * g_debugImageScale , startCursorPos.y + 255 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(112 * g_debugImageScale, 39 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(249), UVFromCoordinate(610)), ImVec2(UVFromCoordinate(361), UVFromCoordinate(649)));	// Pressed tex


				// RB 1 (front view)
				ImGui::SetCursorScreenPos({ startCursorPos.x + 300 * g_debugImageScale , startCursorPos.y + 122 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(136 * g_debugImageScale, 39 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(273), UVFromCoordinate(651)), ImVec2(UVFromCoordinate(409), UVFromCoordinate(690)));	// Pressed tex
			}
			

			// Y			
			if (gamepad.InputState.ButtonState & Input::BUTTON_NORTH)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 373 * g_debugImageScale , startCursorPos.y + 301 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(42 * g_debugImageScale, 42 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(55), UVFromCoordinate(691)), ImVec2(UVFromCoordinate(98), UVFromCoordinate(734)));
			}

			// A
			if (gamepad.InputState.ButtonState & Input::BUTTON_SOUTH)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 373 * g_debugImageScale , startCursorPos.y + 373 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(42 * g_debugImageScale, 42 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(187), UVFromCoordinate(691)), ImVec2(UVFromCoordinate(229), UVFromCoordinate(734)));
			}

			// X
			if (gamepad.InputState.ButtonState & Input::BUTTON_WEST)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 337 * g_debugImageScale , startCursorPos.y + 337 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(42 * g_debugImageScale, 42 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(99), UVFromCoordinate(691)), ImVec2(UVFromCoordinate(141), UVFromCoordinate(734)));
			}

			// B
			if (gamepad.InputState.ButtonState & Input::BUTTON_EAST)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 409 * g_debugImageScale , startCursorPos.y + 337 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(42 * g_debugImageScale, 42 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(143), UVFromCoordinate(691)), ImVec2(UVFromCoordinate(185), UVFromCoordinate(734)));
			}


			// Select
			if (gamepad.InputState.ButtonState & Input::OPTIONS_LEFT)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 202 * g_debugImageScale , startCursorPos.y + 342 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(30 * g_debugImageScale, 30 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(377), UVFromCoordinate(610)), ImVec2(UVFromCoordinate(407), UVFromCoordinate(640)));
			}

			// Start
			if (gamepad.InputState.ButtonState & Input::OPTIONS_RIGHT)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 277 * g_debugImageScale , startCursorPos.y + 342 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(30 * g_debugImageScale, 30 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(377), UVFromCoordinate(610)), ImVec2(UVFromCoordinate(407), UVFromCoordinate(640)));
			}


			// DPAD up
			if (gamepad.InputState.ButtonState & Input::DPAD_UP)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 178 * g_debugImageScale , startCursorPos.y + 400 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(23 * g_debugImageScale, 27 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(230), UVFromCoordinate(697)), ImVec2(UVFromCoordinate(253), UVFromCoordinate(724)));
			}

			// DPAD down
			if (gamepad.InputState.ButtonState & Input::DPAD_DOWN)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 178 * g_debugImageScale , startCursorPos.y + 456 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(23 * g_debugImageScale, 27 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(253), UVFromCoordinate(724)), ImVec2(UVFromCoordinate(230), UVFromCoordinate(697)));
			}

			// DPAD right
			if (gamepad.InputState.ButtonState & Input::DPAD_RIGHT)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 204 * g_debugImageScale , startCursorPos.y + 430 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(27 * g_debugImageScale, 23 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(254), UVFromCoordinate(701)), ImVec2(UVFromCoordinate(281), UVFromCoordinate(724)));
			}
			
			// DPAD left
			if (gamepad.InputState.ButtonState & Input::DPAD_LEFT)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 149 * g_debugImageScale , startCursorPos.y + 430 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(27 * g_debugImageScale, 23 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(281), UVFromCoordinate(724)), ImVec2(UVFromCoordinate(254), UVFromCoordinate(701)));
			}

			// -----------------------------------------------------------------------------------------------------------------------------

			ImGui::SetCursorScreenPos({ startCursorPos.x + 514 * g_debugImageScale, startCursorPos.y + 595 * g_debugImageScale });

			#endif
		}

		void DrawDualShockDebugLayout(const Input::Gamepad& gamepad)
		{
			// TODO: Draw the image(s) of the controller and highlight / move the parts accordingly to the actual input
			//ImGui::Image((ImTextureID)m_DualSenseDBImage->GetGPUHandle(), ImVec2(m_DualSenseDBImage->GetWidth(), m_DualSenseDBImage->GetHeight()));

		}

		void DrawDualSenseDebugLayout(const Input::Gamepad& gamepad)
		{

			#ifdef BOREALIS_WIN

			ImVec2 startCursorPos = ImGui::GetCursorScreenPos();
			ImVec2 lThumPos = ImVec2(gamepad.InputState.LeftThumbstickX, gamepad.InputState.LeftThumbstickY);
			ImVec2 rThumPos = ImVec2(gamepad.InputState.RightThumbstickX, gamepad.InputState.RightThumbstickY);

			ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(491 * g_debugImageScale, 391 * g_debugImageScale), 
				ImVec2(UVFromCoordinate(533), UVFromCoordinate(202)), ImVec2(UVFromCoordinate(1024), UVFromCoordinate(593) ));

			// --------------------- Drawing individual parts of the same atlas image texture to represent button states ---------------------

			
			// L
			ImGui::SetCursorScreenPos({ (startCursorPos.x + 123 * g_debugImageScale) + m_ThumbstickMovementOffset * lThumPos.x * g_debugImageScale, 
				(startCursorPos.y + 205 * g_debugImageScale) - m_ThumbstickMovementOffset * lThumPos.y * g_debugImageScale });
			ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(60 * g_debugImageScale, 60 * g_debugImageScale), 
				ImVec2(0.408, 0.597), ImVec2(0.486, 0.675));	// Stick itself
			
			// L Button
			if (gamepad.InputState.ButtonState & Input::LEFT_THUMBSTICK)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 134 * g_debugImageScale) + m_ThumbstickMovementOffset * lThumPos.x * g_debugImageScale, 
					(startCursorPos.y + 214 * g_debugImageScale) - m_ThumbstickMovementOffset * lThumPos.y * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(39 * g_debugImageScale, 39 * g_debugImageScale), 
					ImVec2(0, UVFromCoordinate(691)), ImVec2(UVFromCoordinate(52), UVFromCoordinate(745)));	// Pressed tex
			}
			
			// R
			ImGui::SetCursorScreenPos({ (startCursorPos.x + 306 * g_debugImageScale) + m_ThumbstickMovementOffset * rThumPos.x * g_debugImageScale, 
				(startCursorPos.y + 204 * g_debugImageScale) - m_ThumbstickMovementOffset * rThumPos.y * g_debugImageScale });
			ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(60 * g_debugImageScale, 60 * g_debugImageScale), 
				ImVec2(0.408, 0.597), ImVec2(0.486, 0.675));	// Stick itself

			// R Button
			if (gamepad.InputState.ButtonState & Input::RIGHT_THUMBSTICK)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 317 * g_debugImageScale) + m_ThumbstickMovementOffset * rThumPos.x * g_debugImageScale, (startCursorPos.y + 213 * g_debugImageScale) - m_ThumbstickMovementOffset * rThumPos.y * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(39 * g_debugImageScale, 39 * g_debugImageScale), 
					ImVec2(0, UVFromCoordinate(691)), ImVec2(UVFromCoordinate(52), UVFromCoordinate(745)));	// Pressed tex
			}

			// PS Logo
			if (gamepad.InputState.ButtonState & Input::BUTTON_LOGO)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 227 * g_debugImageScale) + m_ThumbstickMovementOffset * rThumPos.x * g_debugImageScale, (startCursorPos.y + 213 * g_debugImageScale) - m_ThumbstickMovementOffset * rThumPos.y * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(37 * g_debugImageScale, 29 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(533), UVFromCoordinate(656)), ImVec2(UVFromCoordinate(570), UVFromCoordinate(685)));
			}
			
			// DPAD up
			if (gamepad.InputState.ButtonState & Input::DPAD_UP)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 61 * g_debugImageScale), (startCursorPos.y + 109 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(33 * g_debugImageScale, 42 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(640), UVFromCoordinate(645)), ImVec2(UVFromCoordinate(673), UVFromCoordinate(687)));
			}
			
			// DPAD down
			if (gamepad.InputState.ButtonState & Input::DPAD_DOWN)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 61 * g_debugImageScale), (startCursorPos.y + 164 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(33 * g_debugImageScale, 42 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(673), UVFromCoordinate(687)), ImVec2(UVFromCoordinate(640), UVFromCoordinate(645)));
			}

			// DPAD left
			if (gamepad.InputState.ButtonState & Input::DPAD_LEFT)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 29 * g_debugImageScale), (startCursorPos.y + 141 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(42 * g_debugImageScale, 33 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(588), UVFromCoordinate(653)), ImVec2(UVFromCoordinate(630), UVFromCoordinate(686)));
			}
						
			// DPAD right
			if (gamepad.InputState.ButtonState & Input::DPAD_RIGHT)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 84 * g_debugImageScale), (startCursorPos.y + 141 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(42 * g_debugImageScale, 33 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(630), UVFromCoordinate(686)), ImVec2(UVFromCoordinate(588), UVFromCoordinate(653)));
			}

			// Create
			if (gamepad.InputState.ButtonState & Input::OPTIONS_LEFT)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 109 * g_debugImageScale), (startCursorPos.y + 85 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(18 * g_debugImageScale, 28 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(684), UVFromCoordinate(658)), ImVec2(UVFromCoordinate(702), UVFromCoordinate(686)));
			}
			
			// Options
			if(gamepad.InputState.ButtonState & Input::OPTIONS_RIGHT)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 365 * g_debugImageScale), (startCursorPos.y + 85 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(18 * g_debugImageScale, 28 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(702), UVFromCoordinate(658)), ImVec2(UVFromCoordinate(684), UVFromCoordinate(686)));
			}
			
			// Triangle
			if (gamepad.InputState.ButtonState & Input::BUTTON_NORTH)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 395 * g_debugImageScale), (startCursorPos.y + 110 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(34 * g_debugImageScale, 34 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(711), UVFromCoordinate(652)), ImVec2(UVFromCoordinate(745), UVFromCoordinate(686)));
			}
			
			// Square
			if (gamepad.InputState.ButtonState & Input::BUTTON_WEST)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 364 * g_debugImageScale), (startCursorPos.y + 141 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(34 * g_debugImageScale, 34 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(826), UVFromCoordinate(652)), ImVec2(UVFromCoordinate(860), UVFromCoordinate(686)));
			}

			// Cross
			if (gamepad.InputState.ButtonState & Input::BUTTON_SOUTH)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 395 * g_debugImageScale), (startCursorPos.y + 172 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(34 * g_debugImageScale, 34 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(788), UVFromCoordinate(652)), ImVec2(UVFromCoordinate(822), UVFromCoordinate(686)));
			}

			// Circle
			if (gamepad.InputState.ButtonState & Input::BUTTON_EAST)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 426 * g_debugImageScale), (startCursorPos.y + 141 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(34 * g_debugImageScale, 34 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(750), UVFromCoordinate(652)), ImVec2(UVFromCoordinate(784), UVFromCoordinate(686)));
			}

			// L2
			ImGui::SetCursorScreenPos({ startCursorPos.x + 56 * g_debugImageScale , startCursorPos.y + 1 * g_debugImageScale });
			ImGui::ImageWithBg((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(58 * g_debugImageScale, 52 * g_debugImageScale), 
				ImVec2(UVFromCoordinate(1006), UVFromCoordinate(635)), ImVec2(UVFromCoordinate(948), UVFromCoordinate(687)), 
				ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, gamepad.InputState.LeftTrigger)); // TODO: Use trigger value to make opaque

			// R2
			ImGui::SetCursorScreenPos({ startCursorPos.x + 377 * g_debugImageScale , startCursorPos.y + 1 * g_debugImageScale });
			ImGui::ImageWithBg((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(58 * g_debugImageScale, 52 * g_debugImageScale), 
				ImVec2(UVFromCoordinate(948), UVFromCoordinate(635)), ImVec2(UVFromCoordinate(1006), UVFromCoordinate(687)), 
				ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, gamepad.InputState.RightTrigger)); // TODO: Use trigger value to make opaque

			// R1
			if (gamepad.InputState.ButtonState & Input::RIGHT_SHOULDER)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 372 * g_debugImageScale), (startCursorPos.y + 58 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(68 * g_debugImageScale, 22 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(872), UVFromCoordinate(650)), ImVec2(UVFromCoordinate(940), UVFromCoordinate(672)));
			}

			// L1
			if (gamepad.InputState.ButtonState & Input::LEFT_SHOULDER)
			{ 
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 51 * g_debugImageScale), (startCursorPos.y + 58 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(68 * g_debugImageScale, 22 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(940), UVFromCoordinate(650)), ImVec2(UVFromCoordinate(872), UVFromCoordinate(672)));
			}

			// Touchpad
			if (gamepad.InputState.ButtonState & Input::BUTTON_PAD)
			{
				ImGui::SetCursorScreenPos({ (startCursorPos.x + 135 * g_debugImageScale), (startCursorPos.y + 61 * g_debugImageScale) });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(221 * g_debugImageScale, 115 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(659), UVFromCoordinate(22)), ImVec2(UVFromCoordinate(880), UVFromCoordinate(137)));
			}
			
			// Calc touchpoint rendering positions
			float tp1X = ((startCursorPos.x + 135 * g_debugImageScale) + (gamepad.InputState.Touchpad1X / 2048.0f) * 221 * g_debugImageScale - 17); // - half width of touch point sprite
			float tp1Y = ((startCursorPos.y + 61 * g_debugImageScale) + (gamepad.InputState.Touchpad1Y / 1300.0f) * 115 * g_debugImageScale - 17); // - half height of touch point sprite
		
			float tp2X = ((startCursorPos.x + 135 * g_debugImageScale) + (gamepad.InputState.Touchpad2X / 2048.0f) * 221 * g_debugImageScale - 17); // - half width of touch point sprite
			float tp2Y = ((startCursorPos.y + 61 * g_debugImageScale) + (gamepad.InputState.Touchpad2Y / 1300.0f) * 115 * g_debugImageScale - 17); // - half height of touch point sprite

			// Use timer for smooth presentation of touchpoints

			if (m_LastTP1.x != gamepad.InputState.Touchpad1X || m_LastTP1.y != gamepad.InputState.Touchpad1Y)
				m_TouchTimer1 = 0;
			else if(m_TouchTimer1 < 60)
				++m_TouchTimer1;

			if (m_LastTP2.x != gamepad.InputState.Touchpad2X || m_LastTP2.y != gamepad.InputState.Touchpad2Y)
				m_TouchTimer2 = 0;
			else if (m_TouchTimer2 < 60)
				++m_TouchTimer2;
			

			// Touch points
			if (m_TouchTimer1 < 60)
			{
				ImGui::SetCursorScreenPos({ tp1X, tp1Y });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(34 * g_debugImageScale, 34 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(592), UVFromCoordinate(29)), ImVec2(UVFromCoordinate(626), UVFromCoordinate(63)));
			}
			if (m_TouchTimer2 < 60)
			{
				ImGui::SetCursorScreenPos({ tp2X, tp2Y });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(34 * g_debugImageScale, 34 * g_debugImageScale), 
					ImVec2(UVFromCoordinate(592), UVFromCoordinate(29)), ImVec2(UVFromCoordinate(626), UVFromCoordinate(63)));
			}

			ImGui::SetCursorScreenPos({ startCursorPos.x + 491 * g_debugImageScale, startCursorPos.y + 391 * g_debugImageScale });

			DrawAccelerometerPlot(gamepad);
			ImGui::Text("Battery level: (%i)", gamepad.BatteryChargeLevel);

			m_LastTP1 = { gamepad.InputState.Touchpad1X, gamepad.InputState.Touchpad1Y };
			m_LastTP2 = { gamepad.InputState.Touchpad2X, gamepad.InputState.Touchpad2Y };


			#endif
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

#ifdef BOREALIS_WIN
			g_debugImageScale *= 2;
			ImVec2 startCursorPos = ImGui::GetCursorScreenPos();


			ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(188 * g_debugImageScale, 325 * g_debugImageScale),
				ImVec2(UVFromCoordinate(836), UVFromCoordinate(695)), ImVec2(UVFromCoordinate(1024), UVFromCoordinate(1024)));

			// Left Mouse
			if (mouse.InputState.buttonState & Input::BUTTON_LEFT)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 10 * g_debugImageScale, startCursorPos.y + 5 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(82 * g_debugImageScale, 175 * g_debugImageScale),
					ImVec2(UVFromCoordinate(710), UVFromCoordinate(849)), ImVec2(UVFromCoordinate(792), UVFromCoordinate(1024)));
			}

			// Right Mouse
			if (mouse.InputState.buttonState & Input::BUTTON_RIGHT)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 97 * g_debugImageScale, startCursorPos.y + 5 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(82 * g_debugImageScale, 175 * g_debugImageScale),
					ImVec2(UVFromCoordinate(792), UVFromCoordinate(849)), ImVec2(UVFromCoordinate(710), UVFromCoordinate(1024)));
			}

			// Middle Mouse
			if (mouse.InputState.buttonState & Input::BUTTON_MIDDLE)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 84 * g_debugImageScale, startCursorPos.y + 34 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(20 * g_debugImageScale, 51 * g_debugImageScale),
					ImVec2(UVFromCoordinate(793), UVFromCoordinate(973)), ImVec2(UVFromCoordinate(813), UVFromCoordinate(1024)));
			}
			
			if (mouse.InputState.buttonState & Input::BUTTON_EXTRA2)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 172 * g_debugImageScale, startCursorPos.y + 107 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(14 * g_debugImageScale, 49 * g_debugImageScale),
					ImVec2(UVFromCoordinate(809), UVFromCoordinate(851)), ImVec2(UVFromCoordinate(823), UVFromCoordinate(900)));
			}
			
			if (mouse.InputState.buttonState & Input::BUTTON_EXTRA1)
			{
				ImGui::SetCursorScreenPos({ startCursorPos.x + 166 * g_debugImageScale, startCursorPos.y + 156 * g_debugImageScale });
				ImGui::Image((ImTextureID)m_DebugTexAtlas->GetGPUHandle()->ptr, ImVec2(17 * g_debugImageScale, 53 * g_debugImageScale),
					ImVec2(UVFromCoordinate(800), UVFromCoordinate(909)), ImVec2(UVFromCoordinate(817), UVFromCoordinate(962)));
			}

			ImGui::SetCursorScreenPos({ startCursorPos.x + 188 * g_debugImageScale, startCursorPos.y + 330 * g_debugImageScale });
			
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Mouse Position: (x: %.2f | y: %.2f )", mouse.InputState.PositionX, mouse.InputState.PositionY);
			ImGui::Text("Scroll Wheel: (x: %.1f | y: %.1f )", mouse.InputState.WheelX, mouse.InputState.WheelY);


			ImGui::Spacing();
			ImGui::Spacing();

			g_debugImageScale /= 2;
#endif
		}

		void DrawKeyboardDebugLayout(const Input::Keyboard& keyboard)
		{
			// TODO: Draw the image of the keyboard and highlight / move the parts accordingly to the actual input
			// In the case of the keyboard, evaluate if a list of pressed buttons is a better debug format!
		}

	public:

		void OnGui() override
		{
			// Calc scale factor for all debug graphics.
			g_debugImageScale = Math::Max(1.0f - Math::Clamp01(150.0f / ImGui::GetContentRegionAvail().x), 0.35f);
			
			ImGui::SeparatorText("Device Connections");

			//ImGui::Text("Input Devices connected: %u", pInputSystem->GetAllDevices().size());
			ImGui::Spacing();

			ImGui::Text("Mouse connected: %u / 1", pInputSystem->GetMouse().IsValid() ? 1 : 0);
			ImGui::Text("Keyboards connected: %u / 1", pInputSystem->GetKeyboard().IsValid() ? 1 : 0);
			ImGui::Text("Gamepads connected: %lu / %u", pInputSystem->GetGamepads().size(), Input::MAX_GAMEPADS());
			
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::SeparatorText("Input Devices");

			Types::uint8 deviceIdx = 0;

			if (ImGui::TreeNode("All Devices"))
			{
				std::set<Memory::RefCntAutoPtr<Input::IInputDevice>> allDevices = pInputSystem->GetAllDevices();

				for (std::set<Memory::RefCntAutoPtr<Input::IInputDevice>>::iterator it = allDevices.begin(); it != allDevices.end(); ++it)
				{
					Memory::RefCntAutoPtr<Input::IInputDevice> device = *it;

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

								Input::IInputDevice* inputDevice = device.RawPtr();
								Input::Gamepad* gamepad = dynamic_cast<Input::Gamepad*>(inputDevice);
								
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
		Memory::RefCntAutoPtr<Graphics::Texture> m_DebugTexAtlas;
		Types::int8 m_ThumbstickMovementOffset = 30; // pixels per unit in every direction
		ImVec2 m_LastTP1;
		ImVec2 m_LastTP2;
		Types::int8 m_TouchTimer1 = 0;
		Types::int8 m_TouchTimer2 = 0;
	};
}