#include "ImGuiLayer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

USING_NS_CC;

ImGuiLayer::ImGuiLayer()
	: _director(Director::getInstance())
	, _imgui_manager(ImGuiManager::getInstance())
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

void ImGuiLayer::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	_command.init(_globalZOrder, transform, flags);
	_command.func = CC_CALLBACK_0(ImGuiLayer::onDraw, this);
	renderer->addCommand(&_command);
}

void ImGuiLayer::onDraw()
{
	glUseProgram(0);
	CHECK_GL_ERROR_DEBUG(); // whenever a GL function is exectured, check for GL errors
	if (_window)
	{
		// Update timestep
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = _director->getDeltaTime();

		// Prepare ImGui for a new frame:
		ImGui_ImplGlfw_NewFrame();

		// Render the remaining ImGui windows:
		_imgui_manager->updateImGUI();

		// Render:
		ImGui::Render();
		CHECK_GL_ERROR_DEBUG(); // whenever a GL function is exectured, check for GL errors

	}
	glUseProgram(1);
	CHECK_GL_ERROR_DEBUG(); // whenever a GL function is exectured, check for GL errors
}
