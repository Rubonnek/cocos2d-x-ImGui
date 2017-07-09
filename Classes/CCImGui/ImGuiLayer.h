#ifndef __IMGUILAYER_H__
#define __IMGUILAYER_H__

#include "cocos2d.h"
#include "ImGuiManager.h"

USING_NS_CC;

class ImGuiLayer : public cocos2d::Node
{
		//TODO: There should not be more than two ImGuiNodes running around
	public:
		ImGuiLayer(); //Constructor
		virtual bool init() override;
		virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
		void onDraw();

		// implement the "static create()" method manually
		CREATE_FUNC(ImGuiLayer);

	protected:
		~ImGuiLayer(); //Destructor

	private:
		CustomCommand _command;
		Director* _director;
		ImGuiManager* _imgui_backend;
		GLFWwindow* _window;
};

#endif // __IMGUILAYER_H__
