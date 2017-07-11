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

	// Render the initial frame and set the texture:
	ImGui_ImplGlfw_NewFrame();
	_imgui_manager->updateImGUI();
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
	setContentSize(Size(width, height));
	_texture->initWithData(pixels, sizeof(pixels), Texture2D::PixelFormat::RGBA8888, width, height, Size(width, height));



	// All the windows will be managed by ImGui
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP, _texture));

	return true;
}

void ImGuiLayer::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	ImGuiIO& io = ImGui::GetIO();

	// Update ImGui timestep
	io.DeltaTime = _director->getDeltaTime();

	// Prepare ImGui for a new frame:
	ImGui_ImplGlfw_NewFrame();

	// Render the remaining ImGui windows:
	_imgui_manager->updateImGUI();

	// Render the frame internally:
	ImGui::Render();

	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;

	ImDrawData* draw_data = ImGui::GetDrawData();
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

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
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
		//CCLOG("Width: %d, Height: %d", width, height);
		_texture->updateWithData(pixels, 0, 0, width, height); //Update the texture

		#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data; //Complete buffer
			const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data; //Complete buffer

			//TODO: Translate this into cocos2d-x:

			//glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
			//glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
			//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, col)));

			//Update the vertices:

			// Transofrm this ImDrawVert:
			//struct ImDrawVert
			//{
			//	ImVec2  pos; // Texture position
			//	ImVec2  uv; // Texture position diff
			//	ImU32   col; // Color
			//};

			// Into the cocos2d-x vertex:
			//struct CC_DLL V3F_C4B_T2F
			//{
			//    /// vertices (3F)
			//    Vec3     vertices;            // 12 bytes
			//
			//    /// colors (4B)
			//    Color4B      colors;              // 4 bytes
			//
			//    // tex coords (2F)
			//    Tex2F        texCoords;           // 8 bytes
			//};

			// We need to transform the ImGui vertex buffer into a compatible buffer for cocos2d-x:


			_vertices_map[n].vertices = Vec3(vtx_buffer->uv.x,vtx_buffer->uv.y,0);

			//Extract the color from the ImGui buffer:
			GLubyte r = (vtx_buffer->col >> (8*0)) & 0xff;
			GLubyte g = (vtx_buffer->col >> (8*1)) & 0xff;
			GLubyte b = (vtx_buffer->col >> (8*2)) & 0xff;
			GLubyte a = (vtx_buffer->col >> (8*3)) & 0xff;
			_vertices_map[n].colors = Color4B(r,g,b,a); 
			//CCLOG("Colors r g b a: %d %d %d %d", r, g, b ,a); // Color looks good

			_vertices_map[n].texCoords = Tex2F(vtx_buffer->pos.x, vtx_buffer->pos.y); //FIXME this will draw everything black

			// Attach it to the TrianglesCommand::Triangles :
			//struct Triangles
			//{
			//	/**Vertex data pointer.*/
			//	V3F_C4B_T2F* verts;
			//	/**Index data pointer.*/
			//	unsigned short* indices;
			//	/**The number of vertices.*/
			//	int vertCount;
			//	/**The number of indices.*/
			//	int indexCount;
			//};

			// Update the triangles information:
			//CCLOG("Number of commands is: %d", n);
			_triangles_map[n].verts = &(_vertices_map[n]);
			_triangles_map[n].indices = (unsigned short*)idx_buffer;
			_triangles_map[n].vertCount = cmd_list->VtxBuffer.Size / 3;
			_triangles_map[n].indexCount = cmd_list->IdxBuffer.Size;
			CCLOG("vertCount: %d",_triangles_map[n].vertCount);
			CCLOG("indexCount: %d",_triangles_map[n].indexCount);

			// Attach the TrianglesCommand::Triangles to the TrianglesCommand
			//_triangles_command_map[n].setTriangles(_triangles_map[n]);


			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{

				//TODO: Translate this code block into cocos2d-x
				//      Where weare looping over the indices of the triangles we are drawing.
				//	glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				//	glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
				//  ===> void glDrawElements(GLsizei count,  GLenum type,  const GLvoid * indices);
				//  void glDrawElements(GLenum mode,  GLsizei count,  GLenum type,  const GLvoid * indices);
				//  idx_buffer += pcmd->ElemCount;

				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

				// Initialize the triangle command:
				//void TrianglesCommand::init(float globalOrder, Texture2D* texture, GLProgramState* glProgramState, BlendFunc blendType, const Triangles& triangles, const Mat4& mv, uint32_t flags)

				_triangles_command_map[cmd_i].init(_globalZOrder,
						_texture,
						getGLProgramState(),
						BlendFunc::ALPHA_NON_PREMULTIPLIED,
						_triangles_map[n], 
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
