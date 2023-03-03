#pragma once

#include <chrono>
#include <filesystem>
#include <functional>

namespace iplayer
{

class IMusicPlayer
{
public:
	virtual ~IMusicPlayer() = default;
	virtual bool openMusic(const std::filesystem::path&) = 0;
	virtual void pause() = 0;
	virtual void play() = 0;
	virtual void setElapsedTime(const std::chrono::seconds&) = 0;
	virtual std::chrono::seconds getElapsedTime() = 0;
	virtual void setOnMusicFinished(std::function<void()>) = 0;
};

} // namespace iplayer