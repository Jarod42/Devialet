#pragma once

#include "track.h"

#include <vector>

namespace iplayer
{

class Playlist
{
public:
	void push_back(Track&&);
	void insertAt(std::size_t pos, Track&& track);

	void remove(std::size_t);
	void move(std::size_t from, std::size_t to);

	void removeDuplicate();

	const std::vector<std::pair<std::size_t, Track>>& getTracks() const { return tracks; }

	void shuffle();

	void info(std::ostream&) const;

private:
	std::size_t counter = 0; // Used for unique ID
	std::vector<std::pair<std::size_t, Track>> tracks;
};
} // namespace iplayer
