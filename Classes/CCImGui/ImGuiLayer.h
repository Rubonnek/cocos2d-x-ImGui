#ifndef __IMGUILAYER_H__
#define __IMGUILAYER_H__

#include "cocos2d.h"
#include "ImGuiManager.h"

USING_NS_CC;

class ImGuiLayer : public cocos2d::Node
{
	public:
		ImGuiLayer(); //Constructor
		virtual bool init() override;
		virtual void draw(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

		// implement the "static create()" method manually
		CREATE_FUNC(ImGuiLayer);

	protected:
		~ImGuiLayer(); //Destructor

	private:
		// Utility
		Director* _director;
		ImGuiManager* _imgui_manager;

		// Necessary variables for rendering:
		Texture2D* _texture;
		std::map<int, TrianglesCommand> _triangles_command_map;
		std::map<int, TrianglesCommand::Triangles> _triangles_map;
};

#endif // __IMGUILAYER_H__
