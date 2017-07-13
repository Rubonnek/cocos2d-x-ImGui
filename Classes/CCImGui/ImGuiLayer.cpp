#include "ImGuiLayer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

USING_NS_CC;

ImGuiLayer::ImGuiLayer()
	: _director(Director::getInstance())
	, _imgui_manager(ImGuiManager::getInstance())
	  , _texture(new Texture2D)
{
}

ImGuiLayer::~ImGuiLayer()
{
	// We used new for _texture. Therefore we have to use delete.
	delete _texture;
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
	_imgui_manager->updateImGui();		// Inject the user-defined callbacks
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

	// Stack the user-provided ImGui windows for rendering:
	_imgui_manager->updateImGui();

	// Render the frame internally:
	ImGui::Render();

	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;

	ImDrawData* draw_data = ImGui::GetDrawData();
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Generate a cocos2d::Texture2D from the data of ImGui:
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

	// Make sure we have enough space in memory for each of the ImGui drawing commands:
	auto DrawingCommandCount = draw_data->CmdListsCount;
	_triangles_vector.resize(DrawingCommandCount);
	_triangles_command_vector.resize(DrawingCommandCount);

	for (int n = 0; n < DrawingCommandCount; n++)
	{
		// ImGui draw data we have to pass to cocos2d-x renderer:
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data; //Complete buffer -- we need to modify it.
		ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data; //Complete buffer

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

		for ( int index = 0; index < cmd_list->VtxBuffer.Size; ++index )
		{
			// First off, lets translate the vertices pointer:

			// We need to transform this ImGui ImDrawVert:
			//struct ImDrawVert
			//{
			//	ImVec2  pos; // Texture position 8 bytes
			//	ImVec2  uv; // Texture on display 8 bytes
			//	ImU32   col; // Color 4 bytes
			//};

			// Into the cocos2d-x vertex ( reorganized to match order):
			//struct CC_DLL V3F_C4B_T2F
			//{
			//    Tex2F        texCoords;           // 8 bytes
			//    Vec3     vertices;            // 12 bytes
			//    Color4B      colors;              // 4 bytes
			//};

			// Since we can restructure ImDrawVert in imconfig.h, we can reorganize the original cocos2d-x verts:

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

			// To the following:

			//struct ImDrawVert
			//{
			//	ImVec2 pos;
			//	float z;
			//	ImU32 col;
			//	ImVec2 uv;
			//};

			// Therefore we only need to convert the OpenGL coordinates to Cocos2D-X coordinates:
			// Note: manipulating the coordinates here displaces the ImGui image
			((Vec2&)vtx_buffer[index].pos) = _director->convertToUI(Vec2(vtx_buffer[index].pos.x, vtx_buffer[index].pos.y));

			// Note: the above is equivalent to this:
			//		auto vector_in_cocos2d_x_coordinates = _director->convertToUI(Vec2(vtx_buffer[index].pos.x, vtx_buffer[index].pos.y));
			//		vtx_buffer[index].pos.x = vector_in_cocos2d_x_coordinates.x;
			//		vtx_buffer[index].pos.y = vector_in_cocos2d_x_coordinates.y;

			// Note: ImGui does not initialize new variables added to the the
			// ImDrawVert struct even when these values are specified in the
			// struct. We have to force the variable we added, "z", to be 0.
			// Otherwise "z" will contain trash and will turn into rendering
			// glitches. This is NOT a memory corruption problem in ImGui.
			//CCLOG("vtx buffer z: %f", vtx_buffer[index].z);
			vtx_buffer[index].z = 0;

			// And that's it for the vertex buffer! :D
		}

		// Now that we ported the vertex buffer info to cocos2d-x , we have to
		// attach it to the TrianglesCommand::Triangles:

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
		_triangles_vector[n].verts = (V3F_C4B_T2F*) vtx_buffer; // Here we have to force-cast ImGui pointer to the Cocos2D-X pointer. The structs are of the same size and compatible by now.
		_triangles_vector[n].indices = idx_buffer;
		_triangles_vector[n].vertCount = draw_data->CmdLists[n]->VtxBuffer.Size;
		_triangles_vector[n].indexCount = draw_data->CmdLists[n]->IdxBuffer.Size;

		// Now we have to port this ImGui rendering code over to Cocos2D-X:

		//glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId); // This is done by cocos.
		//glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));  //
		//glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer); // This is done by cocos

		// Port notes: all of that is done with a cocos2d::TriangleCommand

		// and passing the right information to it, and finally passing it to
		// the renderer. Thanks to @stevetranby from github for pointing that
		// out here:
		// http://discuss.cocos2d-x.org/t/cocos2d-x-ui-is-horrible/35796/2

		// Init the triangles command:
		_triangles_command_vector[n].init(_globalZOrder,
				_texture,
				getGLProgramState(),
				BlendFunc::ALPHA_NON_PREMULTIPLIED,
				_triangles_vector[n],
				transform,
				flags);

		// Finally pass all this information to the renderer queue here:
		renderer->addCommand(&_triangles_command_vector[n]);

		// And done!
	}
}
