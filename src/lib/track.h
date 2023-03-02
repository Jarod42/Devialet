#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <iostream>

namespace iplayer
{

struct Track
{
	std::filesystem::path filename;
	std::string title;
	std::chrono::seconds duration;
	// other metadata (codec, album, ID3vx, Lyrics...)

	bool operator==(const Track&) const = default;
};

Track openTrack(const std::filesystem::path&);
Track openTrack(const std::filesystem::path&, std::istream&);
void infoTrack(std::ostream&, const Track&);

} // namespace iplayer
