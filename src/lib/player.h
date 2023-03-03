#pragma once

#include "imusicplayer.h"
#include "playlist.h"

#include <atomic>
#include <mutex>
#include <vector>

namespace iplayer
{

/* Main class to simulate a music player */
class Player
{
public:
	Player(std::shared_ptr<IMusicPlayer>, Playlist&& playlist);

	void setOnMusicChanged(std::function<void()>);

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

	std::optional<std::size_t> getSelectionIndex() const;

	// playlist interface
	void push_back(TrackHeader&&);
	void insertAt(std::size_t pos, TrackHeader&& track);

	void remove(std::size_t);
	void move(std::size_t from, std::size_t to);

	void removeDuplicate();

	void info_tracks(std::ostream&);
	void info_track(std::ostream&, std::size_t);

	std::size_t getTrackCount() const { return displayedPlaylist.getTracks().size(); }
	const TrackHeader& getTrack(std::size_t n) const { return displayedPlaylist.getTracks().at(n).second; }

private:
	void previous(Playlist&, std::optional<std::size_t>&);
	void next(Playlist&, std::optional<std::size_t>&);
	void prepareRandomMode();

private:
	std::recursive_mutex mutex;
	std::function<void()> onMusicChanged;
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