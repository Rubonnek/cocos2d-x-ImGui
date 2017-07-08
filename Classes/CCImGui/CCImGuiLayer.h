#ifndef __IMGUILAYER_H__
#define __IMGUILAYER_H__

#include "cocos2d.h"

USING_NS_CC;

// cocos2d::Layer has been deprecated. The equivalent now is a node.
class ImGuiLayer : public cocos2d::Node
{
	public:
		virtual bool init() override;
		virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
		void onDraw();

		// implement the "static create()" method manually
		CREATE_FUNC(ImGuiLayer);

		// Add Mouse callbacks:
		EventListenerMouse* onMouseEventListener;
		void onMouseDown(EventMouse* event);
		void onMouseUp(EventMouse* event);
		void onMouseMove(EventMouse* event);
		void onMouseScroll(EventMouse* event);

		//Add keyboard callbacks
		EventListenerKeyboard* onKeyboardEventListener;
		void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
		void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	private:
		CustomCommand _command;
};

#endif // __IMGUILAYER_H__
