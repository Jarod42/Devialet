#pragma once

#include "trackheader.h"

#include <vector>

namespace iplayer
{

class Playlist
{
public:
	void push_back(TrackHeader&&);
	void insertAt(std::size_t pos, TrackHeader&& track);

	void remove(std::size_t);
	void move(std::size_t from, std::size_t to);

	void removeDuplicate();

	const std::vector<std::pair<std::size_t, TrackHeader>>& getTracks() const { return tracks; }

	void shuffle();

	void info(std::ostream&) const;

private:
	std::size_t counter = 0; // Used for unique ID
	std::vector<std::pair<std::size_t, TrackHeader>> tracks;
};
} // namespace iplayer
