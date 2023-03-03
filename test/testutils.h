#pragma once

#include "playlist.h"
#include "trackheader.h"

#include <vector>

//------------------------------------------------------------------------------
inline iplayer::TrackHeader makeTrack(std::size_t n)
{
	return {.filename = "file" + std::to_string(n),
	        .title = "Title" + std::to_string(n),
	        .duration{std::chrono::seconds(n)}};
}

//------------------------------------------------------------------------------
inline iplayer::Playlist buildPlaylist(const std::vector<std::size_t>& v)
{
	iplayer::Playlist res;
	for (std::size_t n : v) {
		res.push_back(makeTrack(n));
	}
	return res;
}
