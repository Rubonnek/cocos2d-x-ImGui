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

	// Render the initial frame and set the texture. There will be no need to
	// update this texture again. It will be done automatically by ImGui with
	// ImGui::Render() and will be catched by the Cocos2D-X renderer through
	// the pointer called "pixels"

	ImGui_ImplGlfw_NewFrame();			// Start generating the new frame.
	_imgui_manager->updateImGUI();		// Inject the user-defined callbacks
	ImGui::Render();					// Finishe rendering

	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;

	// Note: Cocos2D-X cannot use Alpha8 texture the way ImGui intentds to.
	// According to the Cocos2D-X documentation at CCTexture2D.h:
	// - generate 32-bit textures: Texture2D::PixelFormat::RGBA8888 (default one)
	// - generate 8-bit textures: Texture2D::PixelFormat::A8 (only use it if you use just 1 color)
	// And A8 uses one more color in ImGui.
	// Therefore we have to waste a bit more on GPU memory, and we can't do this:
	//io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
	_texture->initWithData(pixels, sizeof(pixels), Texture2D::PixelFormat::RGBA8888, width, height, Size(width,height));
	//setContentSize(Size(width, height)); // Doesn't seem necessary, but I'm leaving this here just in case.

	// We need to set up a GL program:
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP, _texture));

	// All the windows will be managed by ImGuiManager.
	// Don't inject ImGui callbacks here. Do it in the init method of the ImGuiManager singleton
	// if you want a default set of ImGui windows or calll its member function
	// addImGuiCallback to add more windows at runtime.

	return true;
}

void ImGuiLayer::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
	ImGuiIO& io = ImGui::GetIO();

	// Update ImGui timestep
	io.DeltaTime = _director->getDeltaTime();

	// Prepare ImGui for a new frame:
	ImGui_ImplGlfw_NewFrame();

	// Stach the remaining ImGui windows for rendering:
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
		//_texture->updateWithData(pixels, 0, 0, width, height); //Update the texture

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			// ImGui we have to pass to cocos2d-x renderer:
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data; //Complete buffer
			const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data; //Complete buffer

			// Port this code block into cocos2d-x compatible code:

			//#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
			//      Here we pass full vertex, textures and color arrays to OpenGL
			//glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
			//glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
			//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + OFFSETOF(ImDrawVert, col)));
			//#undef OFFSETOF


			// Port notes:

			// Since we can pass arrays over to OpenGL, here we
			// have to transform each of those arrays into cocos2d-x compatible
			// arrays. Meaning, we have to transform all these vertices over to
			// V3F_C4B_T2F which can be stored in a cocos2d::TrianglesCommand::Triangles

			CC_SAFE_DELETE_ARRAY(_vertices_map[n]);
			_vertices_map[n] = new V3F_C4B_T2F[cmd_list->VtxBuffer.Size];
			for ( int index = 0; index < cmd_list->VtxBuffer.Size; ++index) // Here we move the index 3 steps to access the next ImDrawVert on the next iteration
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
				_vertices_map[n][index].texCoords = Tex2F(vtx_buffer[index].uv.x, vtx_buffer[index].uv.y);

				// Texture position:
				// Note: the Y axis gets flipped if we don't convert to Cocos2D-X coordinates here:
				auto vector_in_cocos2d_x_coordinates = _director->convertToUI(Vec2(vtx_buffer[index].pos.x, vtx_buffer[index].pos.y));
				// Manipulating the coordinates here displaces the image
				_vertices_map[n][index].vertices = Vec3(vector_in_cocos2d_x_coordinates.x, vector_in_cocos2d_x_coordinates.y, 0);

				// Colors:
				// Extract each byte of the 32 bit color and pass it to the Cocos2d-X vert:
				//CCLOG("Size of the vertex color: %lu", sizeof(vtx_buffer[index].col));
				GLubyte r = (vtx_buffer[index].col >> (8*0)) & 0xFF;
				GLubyte g = (vtx_buffer[index].col >> (8*1)) & 0xFF;
				GLubyte b = (vtx_buffer[index].col >> (8*2)) & 0xFF;
				GLubyte a = (vtx_buffer[index].col >> (8*3)) & 0xFF;
				_vertices_map[n][index].colors = Color4B(r,g,b,a);
				//CCLOG("Colors: r g b a: %d %d %d %d", r, g, b ,a); // Color variables look good
			}

			// Now that we ported the vertices, we have to attach it to the TrianglesCommand::Triangles:
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
			_triangles_map[n].verts = _vertices_map[n]; // Here we use the vertices pointer we just create. We can't use raw ImGui data here.
			_triangles_map[n].indices = (short unsigned int*)idx_buffer; // We have to force-cast the idx_buffer pointer. Originally it had the const modifier.
			_triangles_map[n].vertCount = draw_data->CmdLists[n]->VtxBuffer.Size;
			_triangles_map[n].indexCount = draw_data->CmdLists[n]->IdxBuffer.Size;

			// Debug Count
			//CCLOG("vertCount: %d",_triangles_map[n].vertCount);
			//CCLOG("indexCount: %d",_triangles_map[n].indexCount);

			// Port this ImGui rendering code over to Cocos2D-X:
			//glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId); // This is done by cocos.
			//glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));  //
			//glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer); // This is done by cocos

			// Port notes: all of that is done with a cocos2d::TriangleCommand
			// and passing the right information to it, and finally passing it
			// to the renderer. Thanks to stevetranby for pointing that out
			// here:
			// http://discuss.cocos2d-x.org/t/cocos2d-x-ui-is-horrible/35796/2

			// Init the triangles command:
			_triangles_command_map[n].init(_globalZOrder,
					_texture,
					getGLProgramState(),
					BlendFunc::ALPHA_NON_PREMULTIPLIED,
					_triangles_map[n],
					transform,
					flags);

			// Finally final pass all this information to the renderer queue here:
			renderer->addCommand(&_triangles_command_map[n]);
		}

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
