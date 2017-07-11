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

			//TODO: Translate this code block into cocos2d-x:
			//      Here we pass full vertex, textures and color arrays to OpenGL
			//glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
			//glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
			//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, col)));

			//TODO: So here we have to transform each of those arrays into cocos2d-x Triangles:
			//TODO: We have to use malloc here. We want a continuous buffer
			CC_SAFE_DELETE_ARRAY(_vertices_map[n]);
			_vertices_map[n] = new V3F_C4B_T2F[cmd_list->VtxBuffer.Size];
			for ( int index = 0; index < cmd_list->VtxBuffer.Size; index += 3 )
			{
				// First off, lets translate the vertices pointer:

				// We need to transofrm this ImGui ImDrawVert:
				//struct ImDrawVert
				//{
				//	ImVec2  pos; // Texture position
				//	ImVec2  uv; // Texture position diff
				//	ImU32   col; // Color
				//};

				// Into the cocos2d-x vertex ( reorganized to match order):
				//struct CC_DLL V3F_C4B_T2F
				//{
				//    // tex coords (2F)
				//    Tex2F        texCoords;           // 8 bytes
				//
				//    /// vertices (3F)
				//    Vec3     vertices;            // 12 bytes
				//
				//    /// colors (4B)
				//    Color4B      colors;              // 4 bytes
				//};

				// Start by transforming the vertices:
				//Tex Coords
				Vec2 vector_in_cocos2d_x_coordinates = _director->convertToUI(Vec2(vtx_buffer[index].uv.x, vtx_buffer[index].uv.y));
				_vertices_map[n][index].texCoords = Tex2F(vtx_buffer[index].uv.x, vtx_buffer[index].uv.y);

				// Texture Position Diff
				//TODO: we need to convert GL coordinates to cocos2d-x coordinates
				vector_in_cocos2d_x_coordinates = _director->convertToUI(Vec2(vtx_buffer[index].pos.x, vtx_buffer[index].pos.y));
				_vertices_map[n][index].vertices = Vec3(vector_in_cocos2d_x_coordinates.x,vector_in_cocos2d_x_coordinates.y,0);
				GLubyte r = (vtx_buffer[index].col >> (8*0)) & 0xff;
				GLubyte g = (vtx_buffer[index].col >> (8*1)) & 0xff;
				GLubyte b = (vtx_buffer[index].col >> (8*2)) & 0xff;
				GLubyte a = (vtx_buffer[index].col >> (8*3)) & 0xff;

				// Colors
				_vertices_map[n][index].colors = Color4B(r,g,b,a); 
				//CCLOG("Colors r g b a: %d %d %d %d", r, g, b ,a); // Color looks good
			}

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

			// First of we need to displace the index pointer:
			_triangles_map[n].verts = _vertices_map[n]; // Here we use the vertices pointer we just create. We can't use raw ImGui data here.
			_triangles_map[n].indices = draw_data->CmdLists[n]->IdxBuffer.Data;
			_triangles_map[n].vertCount = draw_data->CmdLists[n]->VtxBuffer.Size;
			_triangles_map[n].indexCount = draw_data->CmdLists[n]->IdxBuffer.Size;
			CCLOG("vertCount: %d",_triangles_map[n].vertCount);
			CCLOG("indexCount: %d",_triangles_map[n].indexCount);

			// The final pass to the renderer happens here:

			_triangles_command_map[n].init(_globalZOrder,
					_texture,
					getGLProgramState(),
					BlendFunc::ALPHA_NON_PREMULTIPLIED,
					_triangles_map[n], 
					transform,
					flags);

			// Pass it to the cocos2d-x renderer:
			renderer->addCommand(&_triangles_command_map[n]);
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
