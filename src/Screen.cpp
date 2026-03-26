#include "Screen.h"
#include <sstream>

bool Screen::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return false;
	}

	window = SDL_CreateWindow("CHIP-8 Emulator", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	renderer = SDL_CreateRenderer(window, nullptr);

	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);

	if (!window || !renderer || !texture) return false;

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	return true;
}

void Screen::clear() {
	std::fill(pixels, pixels + (width * height), offColor);
}

bool Screen::setPixel(uint32_t x, uint32_t y) {
    // 1. Handle wrapping (standard CHIP-8 behavior)
    x %= width;
    y %= height;
    uint32_t index = x + (y * width);

    // 2. Check if the current pixel is "on" (White)
    // Assuming 0xFFFFFFFF is White and 0x000000FF is Black
    bool currentPixelOn = (pixels[index] == onColor);

    // 3. XOR logic: If the pixel was on, it now turns off (and vice-versa)
    if (currentPixelOn) {
        pixels[index] = offColor; // Turn Off
        return true; // Collision occurred (pixel flipped from on to off)
    } else {
        pixels[index] = onColor; // Turn On
        return false; // No collision
    }
}

// Update texture after all pixel changes have been made
void Screen::updateTexture() {
    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(uint32_t));
}

Menu::ScreenAction Screen::draw() {
    Menu::ScreenAction action = Menu::ScreenAction::None;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 1. Start the ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    float customVerticalPadding = 10.0f; // Increase padding for menu bar

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, customVerticalPadding));

    // 2. Create the Top Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load ROM", "Ctrl+O")) {
                action = Menu::ScreenAction::OpenROM;
            }
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                action = Menu::ScreenAction::Quit;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Reset Interpreter")) {
                action = Menu::ScreenAction::Reset;
            }
            ImGui::Separator();

            std::string resLabel = "Use " + std::to_string(64 * (3 - resolutionMultiplier)) +
                "x" + std::to_string(32 * (3 - resolutionMultiplier)) +
                " Resolution";

            if (ImGui::MenuItem(resLabel.c_str())) {
                resolutionMultiplier = 3 - resolutionMultiplier;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::PopStyleVar();


    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    float menuHeight = ImGui::GetFrameHeight();

    SDL_FRect outRect;

    outRect.x = 0.0f;
    outRect.y = menuHeight;
    outRect.w = (float)w * resolutionMultiplier;
    outRect.h = (float)h * resolutionMultiplier - menuHeight;

    SDL_RenderTexture(renderer, texture, NULL, &outRect);

    // 4. Render ImGui on top of the SDL Texture
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    // 5. Present everything
    SDL_RenderPresent(renderer);
    return action;
}