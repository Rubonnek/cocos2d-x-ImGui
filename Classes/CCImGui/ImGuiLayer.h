#ifndef __IMGUILAYER_H__
#define __IMGUILAYER_H__

#include "cocos2d.h"

USING_NS_CC;

class ImGuiLayer : public cocos2d::Node
{
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
};

#endif // __IMGUILAYER_H__
