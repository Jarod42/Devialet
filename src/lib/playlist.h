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

	const std::vector<Track>& getTracks() const { return tracks; }

	void info(std::ostream&) const;

private:
	std::vector<Track> tracks;
};
} // namespace iplayer
