#include "ImGuiLayer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

USING_NS_CC;

ImGuiLayer::ImGuiLayer()
	: _director(Director::getInstance())
	, _imgui_manager(ImGuiManager::getInstance())
	  , _texture(new Texture2D)
{
	_texture->retain();
}

ImGuiLayer::~ImGuiLayer()
{
	_texture->release();
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
	// Update timestep
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = _director->getDeltaTime();

	// Prepare ImGui for a new frame:
	ImGui_ImplGlfw_NewFrame();

	// Render the remaining ImGui windows:
	_imgui_manager->updateImGUI();

	// Render the frame internally:
	ImGui::Render();

	// Get the data for drawing ImGui:
	ImDrawData* draw_data = ImGui::GetDrawData();

//#if CC_USE_CULLING
//    // Don't calculate the culling if the transform was not updated
//    auto visitingCamera = Camera::getVisitingCamera();
//    auto defaultCamera = Camera::getDefaultCamera();
//    if (visitingCamera == nullptr) {
//        _insideBounds = true;
//    }
//    else if (visitingCamera == defaultCamera) {
//        _insideBounds = ((flags & FLAGS_TRANSFORM_DIRTY) || visitingCamera->isViewProjectionUpdated()) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;
//    }
//    else
//    {
//        // XXX: this always return true since
//        _insideBounds = renderer->checkVisibility(transform, _contentSize);
//    }
//
//    if(_insideBounds)
//#endif
    {
		// Generate a cocos2d::Texture2D from the data of ImGui:
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
		//CCLOG("Width: %d, Height: %d", width, height);
		_texture->initWithData(pixels, sizeof(pixels), Texture2D::PixelFormat::A8, width, height, Size::ZERO);

		#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
			const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

			//_texture->updateWithData(pixels, sizeof(pixels), Texture2D::PixelFormat::RGBA8888, width, height, Size::ZERO);
			// We need to transform the ImGui vertex buffer into a compatible buffer for cocos2d-x:
			_vertices_map[n].vertices = Vec3(vtx_buffer->uv.x,vtx_buffer->uv.y,0);

			//Extract the color from the ImGui buffer:
			GLubyte a = (vtx_buffer->col >> (8*1)) & 0xff;
			GLubyte b = (vtx_buffer->col >> (8*2)) & 0xff;
			GLubyte g = (vtx_buffer->col >> (8*3)) & 0xff;
			GLubyte r = (vtx_buffer->col >> (8*4)) & 0xff;
			_vertices_map[n].colors = Color4B(r,g,b,a); //FIXME this will draw everything black

			_vertices_map[n].texCoords = Tex2F(vtx_buffer->pos.x, vtx_buffer->pos.y); //FIXME this will draw everything black

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

				// Update the triangles information:
				_triangles_map[cmd_i].verts = &(_vertices_map[n]);
				_triangles_map[cmd_i].indices = (short unsigned int*)idx_buffer;
				_triangles_map[cmd_i].vertCount = cmd_list->VtxBuffer.Size;
				_triangles_map[cmd_i].indexCount = cmd_list->IdxBuffer.Size;

				// Initialize the triangle command:
				_triangles_command_map[cmd_i].init(_globalZOrder,
												   _texture,
												   getGLProgramState(),
												   BlendFunc::ALPHA_NON_PREMULTIPLIED,
												   _triangles_map[cmd_i],
												   transform,
												   flags);

				// Pass it to the cocos2d-x renderer:
				renderer->addCommand(&_triangles_command_map[cmd_i]);

				idx_buffer += pcmd->ElemCount;
			}
		}
		#undef OFFSETOF

		//#if CC_SPRITE_DEBUG_DRAW
		//        _debugDrawNode->clear();
		//        auto count = _polyInfo.triangles.indexCount/3;
		//        auto indices = _polyInfo.triangles.indices;
		//        auto verts = _polyInfo.triangles.verts;
		//        for(ssize_t i = 0; i < count; i++)
		//        {
		//            //draw 3 lines
		//            Vec3 from =verts[indices[i*3]].vertices;
		//            Vec3 to = verts[indices[i*3+1]].vertices;
		//            _debugDrawNode->drawLine(Vec2(from.x, from.y), Vec2(to.x,to.y), Color4F::WHITE);
		//
		//            from =verts[indices[i*3+1]].vertices;
		//            to = verts[indices[i*3+2]].vertices;
		//            _debugDrawNode->drawLine(Vec2(from.x, from.y), Vec2(to.x,to.y), Color4F::WHITE);
		//
		//            from =verts[indices[i*3+2]].vertices;
		//            to = verts[indices[i*3]].vertices;
		//            _debugDrawNode->drawLine(Vec2(from.x, from.y), Vec2(to.x,to.y), Color4F::WHITE);
		//        }
		//#endif //CC_SPRITE_DEBUG_DRAW
    }


	// Set the draw call and vertex count
	//CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, draw_data->TotalVtxCount);



}
