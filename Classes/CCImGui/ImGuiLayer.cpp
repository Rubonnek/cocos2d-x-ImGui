#include "ImGuiLayer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "CCIMGUI.h"

USING_NS_CC;

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

	// Setup Mouse event listeners
	onMouseEventListener = EventListenerMouse::create();
	onMouseEventListener->onMouseDown = CC_CALLBACK_1(ImGuiLayer::onMouseDown, this);
	onMouseEventListener->onMouseUp = CC_CALLBACK_1(ImGuiLayer::onMouseUp, this);
	onMouseEventListener->onMouseMove = CC_CALLBACK_1(ImGuiLayer::onMouseMove, this);
	onMouseEventListener->onMouseScroll = CC_CALLBACK_1(ImGuiLayer::onMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onMouseEventListener, this);


	// Setup the callbacks for the keyboard
	onKeyboardEventListener = EventListenerKeyboard::create();
	onKeyboardEventListener->onKeyPressed = CC_CALLBACK_2(ImGuiLayer::onKeyPressed, this);
	onKeyboardEventListener->onKeyReleased = CC_CALLBACK_2(ImGuiLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onKeyboardEventListener, this);

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
	if (CCIMGUI::getInstance()->getWindow()) {
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = Director::getInstance()->getDeltaTime();

		ImGui_ImplGlfw_NewFrame();
		CCIMGUI::getInstance()->updateImGUI();
		// Rendering
		ImGui::Render();
	}
	glUseProgram(1);
}


void ImGuiLayer::onMouseDown(EventMouse* event) // A button was pressed
{
	CCLOG("onMouseDown: %f, %f", event->getLocation().x, event->getLocation().y);

	// Determine wether an Imgui Wiget was touched or not:
	//bool inImGuiWidgets = ImGui::IsPosHoveringAnyWindow(ImVec2(event->getLocationInView().x, event->getLocationInView().y));
	//CCLOG("event in ImGui widgets %s", inImGuiWidgets ? "yes" : "no");

	switch( event->getMouseButton() ) // check which button:
	{
		case EventMouse::MouseButton::BUTTON_LEFT:
			//CCLOG("LEFT mouse button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_MIDDLE:
			//CCLOG("MIDDLE mouse button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_RIGHT:
			//CCLOG("RIGHT mouse button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_4:
			//CCLOG("BUTTON_4 button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_5:
			//CCLOG("BUTTON_5 button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_6:
			//CCLOG("BUTTON_6 button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_7:
			//CCLOG("BUTTON_7 button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_8:
			//CCLOG("BUTTON_8 button pressed");
			break;
		case EventMouse::MouseButton::BUTTON_UNSET:
			//CCLOG("BUTTON_UNSET button pressed");
			break;
	}
}

void ImGuiLayer::onMouseUp(EventMouse* event) // A button was released
{
	CCLOG("onMouseUp: %f, %f", event->getLocation().x, event->getLocation().y);
	switch( event->getMouseButton() ) // check which button:
	{
		case EventMouse::MouseButton::BUTTON_LEFT:
			//CCLOG("LEFT mouse button released");
			break;
		case EventMouse::MouseButton::BUTTON_MIDDLE:
			//CCLOG("MIDDLE mouse button released");
			break;
		case EventMouse::MouseButton::BUTTON_RIGHT:
			//CCLOG("RIGHT mouse button released");
			break;
		case EventMouse::MouseButton::BUTTON_4:
			//CCLOG("BUTTON_4 button released");
			break;
		case EventMouse::MouseButton::BUTTON_5:
			//CCLOG("BUTTON_5 button released");
			break;
		case EventMouse::MouseButton::BUTTON_6:
			//CCLOG("BUTTON_6 button released");
			break;
		case EventMouse::MouseButton::BUTTON_7:
			//CCLOG("BUTTON_7 button released");
			break;
		case EventMouse::MouseButton::BUTTON_8:
			//CCLOG("BUTTON_8 button released");
			break;
		case EventMouse::MouseButton::BUTTON_UNSET:
			//CCLOG("BUTTON_UNSET button released");
			break;
	}

}

void ImGuiLayer::onMouseMove(EventMouse* event) // This callback gets called whenever the mouse moves, regardless of whether there was a button pressed or not
{
	//CCLOG("onMouseMove: %f, %f", event->getLocation().x, event->getLocation().y);
}

void ImGuiLayer::onMouseScroll(EventMouse* event) // Scroll wheel gets scrolled, this gets called.
{
	CCLOG("onMouseScroll: %f, %f", event->getLocation().x, event->getLocation().y);

	// Gotta static_cast<int> this to check 
	CCLOG("getScrollY: %f", event->getScrollY()); // negative values go up, positive down
	CCLOG("getScrollX: %f", event->getScrollX()); // negative values go right, positive to the left
}

// Implementation of the keyboard event callback function prototype
void ImGuiLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch(keyCode)
	{
		case EventKeyboard::KeyCode::KEY_W:
			break;
		case EventKeyboard::KeyCode::KEY_A:
			break;
		case EventKeyboard::KeyCode::KEY_S:
			break;
		case EventKeyboard::KeyCode::KEY_D:
			break;
		case EventKeyboard::KeyCode::KEY_SPACE:
			break;
		case EventKeyboard::KeyCode::KEY_SHIFT:
			break;
		default: 
			CCLOG("Unhandled key \"%d\" pressed", static_cast<int>(keyCode));
	}
}

void ImGuiLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch(keyCode)
	{
		case EventKeyboard::KeyCode::KEY_W:
			break;
		case EventKeyboard::KeyCode::KEY_A:
			break;
		case EventKeyboard::KeyCode::KEY_S:
			break;
		case EventKeyboard::KeyCode::KEY_D:
			break;
		case EventKeyboard::KeyCode::KEY_SPACE:
			break;
		case EventKeyboard::KeyCode::KEY_SHIFT:
			break;
		case EventKeyboard::KeyCode::KEY_ENTER:
			break;
		default: 
			CCLOG("Unhandled key \"%d\" released", static_cast<int>(keyCode));
	}
}
