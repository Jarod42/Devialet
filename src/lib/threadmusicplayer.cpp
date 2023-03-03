#include "threadmusicplayer.h"

#include <fstream>

using namespace std::literals;

namespace iplayer
{

//------------------------------------------------------------------------------
ThreadMusicPlayer::ThreadMusicPlayer(std::ostream& os) : os(os)
{
	thread = std::thread([this]() {
		while (true) {
			for (std::size_t i = 0; i != 1; ++i) { // 5
				if (stop) {
					return;
				}
				std::this_thread::sleep_for(200ms);
			}
			if (inPause) {
				continue;
			}
			std::lock_guard l{mutex};
			if (elapsedTime.count() >= content.size()) {
				elapsedTime = 0s;
				if (onMusicFinished) {
					onMusicFinished();
				}
				continue;
			}
			this->os << content[static_cast<std::size_t>(elapsedTime.count())] << "\n";
			elapsedTime += 1s;
		}
	});
}

//------------------------------------------------------------------------------
ThreadMusicPlayer::~ThreadMusicPlayer()
{
	stop = true;
	thread.join();
}

//------------------------------------------------------------------------------
bool ThreadMusicPlayer::openMusic(const std::filesystem::path& p)
{
	std::lock_guard l{mutex};

	content.clear();
	elapsedTime = 0s;
	std::ifstream file(p);
	std::string line;

	if (!std::getline(file, line)) {
		return false;
	}
	while (std::getline(file, line)) {
		content.push_back(std::move(line));
	}
	return true;
}
//------------------------------------------------------------------------------
void ThreadMusicPlayer::pause()
{
	inPause = true;
}
//------------------------------------------------------------------------------
void ThreadMusicPlayer::play()
{
	inPause = false;
}
//------------------------------------------------------------------------------
void ThreadMusicPlayer::setElapsedTime(const std::chrono::seconds& t)
{
	std::lock_guard l{mutex};
	elapsedTime = t;
}
//------------------------------------------------------------------------------
std::chrono::seconds ThreadMusicPlayer::getElapsedTime()
{
	std::lock_guard l{mutex};
	return elapsedTime;
}
//------------------------------------------------------------------------------
void ThreadMusicPlayer::setOnMusicFinished(std::function<void()> f)
{
	std::lock_guard l{mutex};
	onMusicFinished = std::move(f);
}

} // namespace iplayer
