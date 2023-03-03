#include "shell.h"
#include "threadmusicplayer.h"

int main(int argc, char* argv[])
{
	auto musicPlayer = std::make_shared<iplayer::ThreadMusicPlayer>(std::cout);
	auto player = std::make_shared<iplayer::Player>(musicPlayer, iplayer::Playlist{});
	iplayer::Shell shell{player, std::cin, std::cout};
	shell.run();
}
