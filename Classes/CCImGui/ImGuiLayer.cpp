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
	setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE));

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
	GL::useProgram(0); // Disable GLSL shaders. Only vertex information will be processed for this node.
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
	//GL::useProgram(GL::VERTEX_ATTRIB_FLAG_POSITION);
	CHECK_GL_ERROR_DEBUG(); // whenever a GL function is exectured, check for GL errors

	/** 
	 * Invalidates the default StateBlock.
	 *
	 * Only call it if you are calling GL calls directly. Invoke this function
	 * at the end of your custom draw call.
	 * This function restores the default render state its defaults values.
	 * Since this function might call GL calls, it must be called in a GL context is present.
	 *
	 * @param stateBits Bitwise-OR of the states that needs to be invalidated
	 */
	RenderState::StateBlock::invalidate(RenderState::StateBlock::RS_DEPTH_TEST |
			RenderState::StateBlock::RS_DEPTH_TEST |
			RenderState::StateBlock::RS_CULL_FACE |
			RenderState::StateBlock::RS_BLEND);
	CHECK_GL_ERROR_DEBUG(); // whenever a GL function is exectured, check for GL errors
}
