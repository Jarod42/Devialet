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

	bool getRandomMode() const { return randomModeActivated; }
	void setRandomMode(bool);

	bool isRepeatModeActivated() const { return repeatModeActivated; }
	void setRepeatMode(bool);

private:
	void previous(Playlist&, std::optional<std::size_t>&);
	void next(Playlist&, std::optional<std::size_t>&);
	void prepareRandomMode();

private:
	std::recursive_mutex mutex;
	std::shared_ptr<IMusicPlayer> musicPlayer;
	std::optional<std::size_t> currentSelectionIndex;
	std::optional<std::size_t> currentRandomSelectionIndex;
	bool playing = false;
	Playlist displayedPlaylist;
	Playlist randomOrderPlaylist;
	std::atomic<bool> repeatModeActivated = false;
	std::atomic<bool> randomModeActivated = false;
};

} // namespace iplayer