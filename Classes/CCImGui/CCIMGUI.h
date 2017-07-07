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

class CCIMGUI
{
	//TODO: Implement destructor. There could be memory leaks here with each of the CCIMGUI_Value pointers

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

};

#endif // __IMGUILAYER_H__
