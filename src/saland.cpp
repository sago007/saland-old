#include <iostream>
#include <boost/program_options.hpp>
#include <SDL_mixer.h>
#include "sago/SagoDataHolder.hpp"
#include "sago/SagoSpriteHolder.hpp"
#include "sago/GameStateInterface.hpp"
#include <sstream>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL/SDL_timer.h>
#include "Libs/NFont.h"
#include "saland/globals.hpp"

#include "sago/SagoMisc.hpp"
#include "sago/platform_folders.h"

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

#define GAMENAME "saland_game"

GlobalData globalData;

static void NFont_Write(SDL_Renderer* target, int x, int y, const char* text) {
	globalData.nf_standard_font.draw(target, x, y, "%s", text);
}

class TheGame : public sago::GameStateInterface {
	virtual bool IsActive() override {
		return true;
	}
	
	virtual void Draw(SDL_Renderer* target) override {
		SDL_SetRenderDrawColor(target, 0, 0, 0, 0);
		SDL_RenderClear(target);
		NFont_Write(target, 10, 10, "Hello World");
		circleRGBA(target, 
				150, 150, 75, 
				0, 0, 255, 255);
		const sago::SagoSprite& s = globalData.spriteHolder->GetSprite("male_walkcycle_E");
		s.Draw(globalData.screen, SDL_GetTicks(), 100, 100);
		SDL_RenderPresent(target);
	}
	
	virtual void ProcessInput(const SDL_Event& event, bool &processed) override {
		
	}
	
	virtual void Update() override {
		
	}
};

void RunGameState(sago::GameStateInterface& state ) {
	bool done = false;     //We are done!
	while (!done && !globalData.isShuttingDown) {
		state.Draw(globalData.screen);

		SDL_Delay(1);
		SDL_Event event;

		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				globalData.isShuttingDown = true;
				done = true;
			}

			bool processed = false;
			state.ProcessInput(event, processed);

		}

		state.Update();

		SDL_RenderPresent(globalData.screen);
		

		if (!state.IsActive()) {
			done = true;
		}
	}
}

void runGame() {
	SDL_Window* win = NULL;
	int posX = 100, posY = 100, width = 1024, height = 768;
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_OGG);
	
	win = SDL_CreateWindow("Hello World", posX, posY, width, height, 0);
	globalData.screen = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	sago::SagoDataHolder holder(globalData.screen);
	globalData.spriteHolder.reset(new sago::SagoSpriteHolder(holder));
	globalData.nf_standard_font.load(globalData.screen, holder.getFontPtr("freeserif", 30),NFont::Color(255,255,255));
	
	TheGame g;
	RunGameState(g);
	
	SDL_DestroyRenderer(globalData.screen);
	SDL_DestroyWindow(win);

	SDL_Quit();

}

static sago::PlatformFolders pf;

std::string getPathToSaveFiles() {
	return pf.getSaveGamesFolder1()+"/"+GAMENAME;
}

void OsCreateSaveFolder() {
#if defined(__unix__)
        std::string cmd = "mkdir -p '"+getPathToSaveFiles()+"/'";
        int retcode = system(cmd.c_str());
        if (retcode != 0) {
                std::cerr << "Failed to create: " << getPathToSaveFiles()+"/" << "\n";
        }
#elif defined(_WIN32)
        //Now for Windows NT/2k/xp/2k3 etc.
        std::string tempA = getPathToSaveFiles();
        CreateDirectory(tempA.c_str(),nullptr);
#endif
}


int main(int argc, const char* argv[]) {
	PHYSFS_init(argv[0]);
	PHYSFS_addToSearchPath((std::string(PHYSFS_getBaseDir())+"/data").c_str(), 1);
	std::string savepath = getPathToSaveFiles();
	OsCreateSaveFolder();
	PHYSFS_addToSearchPath(savepath.c_str(), 1);
	PHYSFS_setWriteDir(savepath.c_str());
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version")) {
		std::cout << "saland " << VERSIONNUMBER << "\n";
		return 0;
	}
	runGame();
	return 0;
}
