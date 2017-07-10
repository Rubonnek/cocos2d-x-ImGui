#include "ImGuiLayer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

USING_NS_CC;

ImGuiLayer::ImGuiLayer()
	: _director(Director::getInstance())
	, _imgui_backend(ImGuiManager::getInstance())
	  , _window(static_cast<GLViewImpl*>(_director->getOpenGLView())->getWindow())
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

	// All the windows will be managed by ImGui

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
	if (_window)
	{
		// Update timestep
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = _director->getDeltaTime();

		// Prepare ImGui for a new frame:
		ImGui_ImplGlfw_NewFrame();

		// Render the remaining ImGui windows:
		_imgui_backend->updateImGUI();

		// Render:
		ImGui::Render();
	}
	glUseProgram(1);
}
