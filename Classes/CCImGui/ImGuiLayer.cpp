#include "ImGuiLayer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "CCIMGUI.h"

USING_NS_CC;

ImGuiLayer::ImGuiLayer()
{
}

ImGuiLayer::~ImGuiLayer()
{
	// Unregister the IMEDelegate functions with the IMEDispatcher.
	// This pointer will now become invalid. There will be a segmentation fault if we don't do this.
}



// on "init" you need to initialize your instance
bool ImGuiLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !Node::init() )
	{
		return false;
	}

	//----------------------------------------
	// init imgui cc
	CCIMGUI::getInstance()->setWindow(((GLViewImpl*)Director::getInstance()->getOpenGLView())->getWindow());
	setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR));
	//----------------------------------------

	return true;
}

void ImGuiLayer::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags)
{
	Node::visit(renderer, parentTransform, parentFlags);
	_command.init(_globalZOrder);
	_command.func = CC_CALLBACK_0(ImGuiLayer::onDraw, this);
	Director::getInstance()->getRenderer()->addCommand(&_command);
}

void ImGuiLayer::onDraw()
{

	glUseProgram(0);
	if (CCIMGUI::getInstance()->getWindow())
	{
		// Update timestep
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = Director::getInstance()->getDeltaTime();

		ImGui_ImplGlfw_NewFrame();
		CCIMGUI::getInstance()->updateImGUI();

		// Render
		ImGui::Render();
	}
	glUseProgram(1);
}
