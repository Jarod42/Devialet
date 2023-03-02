#pragma once

#include "playlist.h"

#include <functional>
#include <mutex>
#include <optional>
#include <vector>

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

/* Main class to simulate a music player */
class Player
{
public:
	Player(std::shared_ptr<IMusicPlayer>, Playlist&& playlist);

	void play();
	void pause();
	void stop();

	void previous();
	void next();

	void select(std::size_t);
#if 0
	bool getRandomMode() const { return randomModeActivated; }
	void setRandomMode(bool);
#endif
	bool isRepeatModeActivated() const { return repeatModeActivated; }
	void setRepeatMode(bool);

private:
	std::recursive_mutex mutex;
	std::shared_ptr<IMusicPlayer> musicPlayer;
	std::optional<std::size_t> currentSelectionIndex;
	bool playing = false;
	Playlist playlist;
	std::atomic<bool> repeatModeActivated = false;
#if 0
	std::atomic<bool> randomModeActivated = false;
#endif
};

} // namespace iplayer