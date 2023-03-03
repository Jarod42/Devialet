#pragma once

#include "imusicplayer.h"

#include <thread>


namespace iplayer
{

class ThreadMusicPlayer : public IMusicPlayer
{
public:
	explicit ThreadMusicPlayer(std::ostream& os);
	~ThreadMusicPlayer();

	ThreadMusicPlayer(const ThreadMusicPlayer&) = delete;
	ThreadMusicPlayer& operator=(const ThreadMusicPlayer&) = delete;

	bool openMusic(const std::filesystem::path&) override;
	void pause() override;
	void play() override;
	void setElapsedTime(const std::chrono::seconds&) override;
	std::chrono::seconds getElapsedTime() override;
	void setOnMusicFinished(std::function<void()>) override;

private:
	std::thread thread;
	std::ostream& os;
	std::atomic<bool> stop = false;
	std::function<void()> onMusicFinished;
	std::atomic<bool> inPause = true;
	std::recursive_mutex mutex;
	std::vector<std::string> content;
	std::chrono::seconds elapsedTime{};
};

} // namespace iplayer
