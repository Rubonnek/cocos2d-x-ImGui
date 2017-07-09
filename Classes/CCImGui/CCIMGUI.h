#ifndef __CC_IMGUI_H__
#define __CC_IMGUI_H__

#include "cocos2d.h"
#include "imgui/imgui.h"

USING_NS_CC;

class CCIMGUI_Value
{
	public:
		int value = 0;
		bool getBool() { return (bool)value; };
		int getInt() { return (int)value; };
};

// CCIMGUI Gived access to the backend. The main dear imgui loop.
// You'll want to edit this if you want to edit "dear imgui"s properties.
// Proper class name could be: imguiBackend
class CCIMGUI
{
	//TODO: Implement destructor. There could be memory leaks here with each of
	//the CCIMGUI_Value pointers

	public:
		static CCIMGUI* getInstance();
		//-------------------------------------------------------
		GLFWwindow* getWindow() { return _window; };
		void setWindow(GLFWwindow* window) { _window = window; };
		ImVec4 getClearColor() { return _clearColor; };
		void setClearColor(ImColor color) { _clearColor = color; };
		//-------------------------------------------------------
		void init();
		void updateImGUI();
		void addImGUI(std::function<void()> imGUICall, std::string name) {_callPipelines[name] = imGUICall;};
		void removeImGUI(std::string name);
		//-------------------------------------------------------
		void setValue(bool value, std::string uid);
		void setValue(int value, std::string uid);
		CCIMGUI_Value* getValue(std::string uid);
		void removeValue(std::string uid);
		//-------------------------------------------------------
		void setShowStyleEditor(bool show) { isShowSetupStyle = show; };

		// Engine callback rewiring so that we can pass raw GLFW events to ImGui
		// Each of these functions will first call the GLViewImpl callback
		// such as onGLFWMouseCallBack, and afterwards they will call the ImGui_Impl callback
        // such as ImGui_ImplGlfw_MouseButtonCallback

		// Note: for some reason there's no need to rewire the mouse button callback.
		//static void rewireMouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/);
		static void rewireScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void rewireKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void rewireCharCallback(GLFWwindow* window, unsigned int c);

	private:
		//-------------------------------------------------------
		GLFWwindow* _window = nullptr;
		ImVec4 _clearColor = ImColor(114, 144, 154);
		//-------------------------------------------------------
		std::map<std::string, std::function<void()>> _callPipelines;
		std::map<std::string, CCIMGUI_Value*> _values;
		std::vector<std::string> _markToDelete;
		//-------------------------------------------------------
		bool isShowSetupStyle = false;
		void displaySetupStyle();
		void rewireEngineGLFWCallbacks();
};

#endif // __IMGUILAYER_H__
