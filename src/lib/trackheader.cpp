#include "trackheader.h"

#include <fstream>
#include <iostream>

namespace iplayer
{

//------------------------------------------------------------------------------
TrackHeader openTrackHeader(const std::filesystem::path& path)
{
	std::ifstream file(path);

	if (!file) {
		throw std::runtime_error("File not found"); // Use dedicated exception instead
	}
	return openTrackHeader(path, file);
}

//------------------------------------------------------------------------------
TrackHeader openTrackHeader(const std::filesystem::path& path, std::istream& is)
{
	std::string line;
	if (!std::getline(is, line)) {
		throw std::runtime_error("Invalid format"); // Use dedicated exception instead
	}
	std::stringstream ss(line);
	TrackHeader res;
	res.filename = path;
	int seconds = 0;
	if (!(ss >> std::quoted(res.title) >> seconds)) {
		throw std::runtime_error("Invalid format"); // Use dedicated exception instead
	}
	res.duration = std::chrono::seconds(seconds);
	// other metadata
	return res;
}

//------------------------------------------------------------------------------
void infoTrack(std::ostream& os, const TrackHeader& track)
{
	os << "Filename: " << track.filename.filename().string() << "\n"
	   << "Title: " << track.title << "\n"
	   << "Duration: " << track.duration.count() << "s"
	   << "\n";
}

} // namespace iplayer
