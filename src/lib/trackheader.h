#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <iostream>

namespace iplayer
{

struct TrackHeader
{
	std::filesystem::path filename;
	std::string title;
	std::chrono::seconds duration;
	// other metadata (codec, album, ID3vx, Lyrics...)

	bool operator==(const TrackHeader&) const = default;
};

TrackHeader openTrackHeader(const std::filesystem::path&);
TrackHeader openTrackHeader(const std::filesystem::path&, std::istream&);
void infoTrack(std::ostream&, const TrackHeader&);

} // namespace iplayer
