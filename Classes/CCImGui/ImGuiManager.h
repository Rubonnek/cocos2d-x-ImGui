#ifndef __CC_IMGUI_H__
#define __CC_IMGUI_H__

#include "cocos2d.h"
#include "imgui/imgui.h"

USING_NS_CC;

// ImGuiManager Gives access to the ImGui backend. The main dear imgui loop that will be persistent across layers.
// You'll want to edit this if you want to edit initial "dear imgui"s properties.

class ImGuiManager
{
	public:
		static ImGuiManager* getInstance();
		void init();
		void updateImGUI();
		void addImGUI(std::function<void()> imGUICall, std::string name) {_callPipelines[name] = imGUICall;};
		void removeImGUI(std::string name);
		void setShowStyleEditor(bool show) { isShowSetupStyle = show; };

		// Engine callback rewiring so that we can pass raw GLFW events to ImGui
		// Each of these functions will first call the GLViewImpl callback
		// such as onGLFWMouseCallBack, and afterwards they will call the ImGui_Impl callback
        // such as ImGui_ImplGlfw_MouseButtonCallback

		// Note: for some reason there's no need to rewire the mouse button callback.
		// static void rewireMouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/);
		static void rewireScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void rewireKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void rewireCharCallback(GLFWwindow* window, unsigned int c);

	private:

		// Map for storing each of the ImGui functions that can be changed at runtime.
		std::map<std::string, std::function<void()>> _callPipelines;
		std::vector<std::string> _markToDelete;

		bool isShowSetupStyle = true;
		void displaySetupStyle();
		void rewireEngineGLFWCallbacks();
};

#endif // __IMGUILAYER_H__
