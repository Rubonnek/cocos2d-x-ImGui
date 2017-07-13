#include "ImGuiManager.h"
//#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_glfw.h"

USING_NS_CC;

//================================================
// style editor variables
static int hue = 140;
static float col_main_sat = 180.f / 255.f;
static float col_main_val = 161.f / 255.f;
static float col_area_sat = 124.f / 255.f;
static float col_area_val = 100.f / 255.f;
static float col_back_sat = 59.f / 255.f;
static float col_back_val = 40.f / 255.f;

ImGuiManager* ImGuiManager::getInstance()
{
	static ImGuiManager instance;

	static bool ran_once = false;
	if ( !ran_once ) 
	{
		instance.init();
		ran_once = true;
	}

	return &instance;
}

void ImGuiManager::init()
{
	////init the impl glfw
	ImGui_ImplGlfw_Init(static_cast<GLViewImpl*>(Director::getInstance()->getOpenGLView())->getWindow(), false);

	//_callPipelines["styleEditor"] = std::bind(&ImGuiManager::displaySetupStyle, this);
	_callPipelines["styleEditor"] = CC_CALLBACK_0(ImGuiManager::displaySetupStyle, this);
	ImGuiStyle& style = ImGui::GetStyle();

	ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
	ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
	ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
	ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);
	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.90f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	//Finally rewire the engine callbacks.
	rewireEngineGLFWCallbacks();
}

void ImGuiManager::updateImGUI()
{
	// Remove all the marked to delete callbacks.
	for (const auto& callbacks_to_delete : _markToDelete )
	{
		_callPipelines.erase(callbacks_to_delete);
	}
	_markToDelete.clear();

	// Now run all the IMGUI callbacks:
	for (const auto& iter : _callPipelines)
	{
		iter.second();
	}
}

void ImGuiManager::addImGuiCallback(std::function<void()> callback,const std::string& name)
{
	_callPipelines[name] = callback;
}

void ImGuiManager::removeImGuiCallback(const std::string& name)
{
	const auto& iter = _callPipelines.find(name);
	if (iter != _callPipelines.end())
		_markToDelete.push_back(name);
}

void ImGuiManager::setShowStyleEditor(bool show)
{
	isShowSetupStyle = show;
}

// Useful for identifying the cases when we don't want our ImGui clicks to affect the gameplay
bool isTouchOnImGuiWindow(Touch* touch)
{
	CCLOG("touch in ImGui widgets %s", ImGui::IsPosHoveringAnyWindow(ImVec2(touch->getLocationInView().x, touch->getLocationInView().y)) ? "yes" : "no");
	return ImGui::IsPosHoveringAnyWindow(ImVec2(touch->getLocationInView().x, touch->getLocationInView().y));
}

// Useful for identifying the cases when we don't want our ImGui clicks to affect the gameplay
bool isClickOnImGuiWindow(EventMouse* click)
{
	CCLOG("click in ImGui widgets %s", ImGui::IsPosHoveringAnyWindow(ImVec2(click->getLocationInView().x, click->getLocationInView().y)) ? "yes" : "no");
	return ImGui::IsPosHoveringAnyWindow(ImVec2(click->getLocationInView().x, click->getLocationInView().y));
}

void ImGuiManager::displaySetupStyle()
{
	if (isShowSetupStyle)
	{
		ImGui::Begin("Style Editor", &isShowSetupStyle);
		ImGui::SliderInt("master hue", &hue, 0, 255);

		float dummy = 0.f; //FIXME: What value is this supposed to be? Seems to be working well anyway as a placeholder.
		ImVec4 rgb;
		ImGui::ColorEditMode(ImGuiColorEditMode_HSV);

		ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_main_sat, col_main_val, rgb.x, rgb.y, rgb.z);
		ImGui::ColorEdit3("main", &rgb.x);
		ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_main_sat, col_main_val);

		ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_area_sat, col_area_val, rgb.x, rgb.y, rgb.z);
		ImGui::ColorEdit3("area", &rgb.x);
		ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_area_sat, col_area_val);

		ImGui::ColorConvertHSVtoRGB(hue / 255.f, col_back_sat, col_back_val, rgb.x, rgb.y, rgb.z);
		ImGui::ColorEdit3("back", &rgb.x);
		ImGui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, dummy, col_back_sat, col_back_val);

		ImGui::End();

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
		ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
		ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
		ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);
		style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 0.90f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.92f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	}
}

void ImGuiManager::rewireEngineGLFWCallbacks()
{
	GLFWwindow* window = static_cast<GLViewImpl*>(Director::getInstance()->getOpenGLView())->getWindow();
	//glfwSetMouseButtonCallback(window, ImGuiManager::rewiredMouseButtonCallback);
	glfwSetScrollCallback(window, ImGuiManager::rewiredScrollCallback);
	glfwSetKeyCallback(window, ImGuiManager::rewiredKeyCallback);
	glfwSetCharCallback(window, ImGuiManager::rewiredCharCallback);
}

//void ImGuiManager::rewireddMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//{
//	GLFWEventHandler::onGLFWMouseCallBack(window, button, action, mods);
//	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
//}

void ImGuiManager::rewiredScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	GLFWEventHandler::onGLFWMouseScrollCallback(window, xoffset, yoffset);
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void ImGuiManager::rewiredKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GLFWEventHandler::onGLFWKeyCallback(window, key, scancode, action, mods);
	ImGui_ImplGlFw_KeyCallback(window, key, scancode, action, mods);
}

void ImGuiManager::rewiredCharCallback(GLFWwindow* window, unsigned int c)
{
	GLFWEventHandler::onGLFWCharCallback(window, c);
	ImGui_ImplGlfw_CharCallback(window, c);
}
