#include "Screen.h"
#include "Chip.h"

bool Screen::init(Chip& chipPtr) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return false;
	}

    this->chip = &chipPtr;

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

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);


    clear();

    setExtendedMode(false);

    updateTexture();
    draw();

	return true;
}

void Screen::clear() {
    memset(logicalPixels, 0, sizeof(logicalPixels));
}

void Screen::setExtendedMode(bool mode) {
    hiRes = mode;

    if (texture) SDL_DestroyTexture(texture);

    uint32_t tWidth = hiRes ? width : (width/2);
    uint32_t tHeight = hiRes ? height : (height / 2);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, tWidth, tHeight);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    clear();
}

void Screen::scrollDown(int n) {
    for (int y = height - 1; y >= n; y--) {
        for (int x = 0; x < width; x++) {
            uint32_t index = x + (y * width);
            uint32_t newIndex = x + ((y-n) * width);
            logicalPixels[index] = logicalPixels[newIndex];
        }
    }

    for (int y = 0; y < n; y++) {
        for (int x = 0; x < width; x++) {
            logicalPixels[x + (y * width)] = 0;
        }
    }
}

void Screen::scrollLeft() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width - 4; x++) {
            logicalPixels[x + (y * width)] = logicalPixels[(x + 4) + (y * width)];
        }
        for (int x = width - 4; x < width; x++) {
            logicalPixels[x + (y * width)] = 0;
        }
    }
}

void Screen::scrollRight() {
    for (int y = 0; y < height; y++) {
        for (int x = width - 1; x >= 4; x--) {
            logicalPixels[x + (y * width)] = logicalPixels[(x - 4) + (y * width)];
        }
        for (int x = 0; x < 4; x++) {
            logicalPixels[x + (y * width)] = 0;
        }
    }
}

bool Screen::setPixel(uint32_t x, uint32_t y, bool clipping) {
    // 1. Handle wrapping (standard CHIP-8 behavior)

    uint32_t lWidth = width / (hiRes ? 1 : 2);
    uint32_t lHeight= height / (hiRes ? 1 : 2);
    if (clipping) {
        if (x >= lWidth || y >= lHeight) return false;
    }

    x %= lWidth;
    y %= lHeight;
    uint32_t index = x + (y * lWidth);

    // 2. Check if the current pixel is "on" (White)
    // Assuming 0xFFFFFFFF is White and 0x000000FF is Black
    bool currentPixelOn = logicalPixels[index];

    // 3. XOR logic: If the pixel was on, it now turns off (and vice-versa)
    if (currentPixelOn) {
        logicalPixels[index] = false; // Turn Off
        return true; // Collision occurred (pixel flipped from on to off)
    } else {
        logicalPixels[index] = true; // Turn On
        return false; // No collision
    }
}

// Update texture after all pixel changes have been made
void Screen::updateTexture() {
    uint32_t lWidth = width / (hiRes ? 1 : 2);
    uint32_t lHeight = height / (hiRes ? 1 : 2);
    for (int i = 0; i < (lWidth * lHeight); ++i) {
        pixels[i] = logicalPixels[i] ? onColor : offColor;
    }
    SDL_UpdateTexture(texture, NULL, pixels, lWidth * sizeof(uint32_t));
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
            if (ImGui::MenuItem("Reset Interpreter", "Ctrl+R")) {
                action = Menu::ScreenAction::Reset;
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Settings")) {
                showSettings = true;
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showSettings) {
        // Set a default size for the window
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Settings", &showSettings)) { // The '&showSettings' adds a [X] close button

            ImGui::SeparatorText("Emulation Settings");

            const char* interpreters[] = { "Chip-8 (Default)", "Super-Chip" };
            static int interpreter_current = 0;

            if (ImGui::Combo("Interpreter", &interpreter_current, interpreters, IM_ARRAYSIZE(interpreters))) {
                Chip::ChipType chipType = Chip::ChipType::Chip_8;
                switch (interpreter_current) {
                    case 0: {
                        chipType = Chip::ChipType::Chip_8;
                        break;
                    }
                    case 1: {
                        chipType = Chip::ChipType::Super_Chip;
                        break;
                    }
                }

                if (chipType != chip->chipType) {
                    // Chip type changed, switch & reset
                    chip->chipType = chipType;
                    chip->reset();
                    chip->loadProgram(chip->romPath);
                }
            }

            bool useHiRes = hiRes;

            ImGui::Checkbox("Use high resolution (128x64)", &useHiRes);

            setExtendedMode(useHiRes);
            updateTexture();

            ImGui::SeparatorText("Visuals");

            float bgr = ((offColor >> 24) & 0xFF) / 255.0f;
            float bgg = ((offColor >> 16) & 0xFF) / 255.0f;
            float bgb = ((offColor >> 8) & 0xFF) / 255.0f;
            static float bgcolor[4] = { bgr, bgg, bgb, 1.0f };
            if (ImGui::ColorEdit4("Off Color", bgcolor)) {
                offColor = (static_cast<uint32_t>(bgcolor[0] * 255) << 24 |
                                     static_cast<uint32_t>(bgcolor[1] * 255) << 16 |
                                     static_cast<uint32_t>(bgcolor[2] * 255) << 8 |
                                     0x000000FF
                );

                updateTexture();
            }

            float fgr = ((onColor >> 24) & 0xFF) / 255.0f;
            float fgg = ((onColor >> 16) & 0xFF) / 255.0f;
            float fgb = ((onColor >> 8) & 0xFF) / 255.0f;
            static float fgcolor[4] = { fgr, fgg, fgb, 1.0f };
            if (ImGui::ColorEdit4("On Color", fgcolor)) {
                onColor = (static_cast<uint32_t>(fgcolor[0] * 255) << 24 |
                                    static_cast<uint32_t>(fgcolor[1] * 255) << 16 |
                                    static_cast<uint32_t>(fgcolor[2] * 255) << 8 |
                                    0x000000FF
                );

                updateTexture();
            }

            ImGui::Spacing();
            if (ImGui::Button("Close")) {
                showSettings = false;
            }
        }
        ImGui::End();
    }

    ImGui::PopStyleVar();


    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    float menuHeight = ImGui::GetFrameHeight();

    //uint8_t resolutionMultiplier = hiRes ? 1 : 2;

    SDL_FRect outRect;
    outRect.x = 0.0f;
    outRect.y = menuHeight;
    outRect.w = (float)w;
    outRect.h = (float)h - menuHeight;

    SDL_RenderTexture(renderer, texture, NULL, &outRect);

    // 4. Render ImGui on top of the SDL Texture
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    // 5. Present everything
    SDL_RenderPresent(renderer);
    return action;
}