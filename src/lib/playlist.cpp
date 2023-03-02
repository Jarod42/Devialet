#include "playlist.h"

#include <algorithm>
#include <random>

namespace iplayer
{
//------------------------------------------------------------------------------
void Playlist::push_back(Track&& track)
{
	tracks.emplace_back(counter++, std::move(track));
}

//------------------------------------------------------------------------------
void Playlist::insertAt(std::size_t pos, Track&& track)
{
	pos = std::clamp(pos, std::size_t(0), tracks.size());
	tracks.emplace(tracks.begin() + pos, counter++, std::move(track));
}

//------------------------------------------------------------------------------
void Playlist::remove(std::size_t pos)
{
	if (pos < tracks.size()) {
		tracks.erase(tracks.begin() + pos);
	}
}

//------------------------------------------------------------------------------
void Playlist::move(std::size_t from, std::size_t to)
{
	if (tracks.size() <= from || tracks.size() <= to || from == to) {
		return;
	}
	if (from < to) {
		std::rotate(tracks.begin() + from, tracks.begin() + from + 1, tracks.begin() + to + 1);
	} else {
		std::rotate(tracks.begin() + to, tracks.begin() + from, tracks.begin() + from + 1);
	}
}

//------------------------------------------------------------------------------
void Playlist::removeDuplicate()
{
	// if order is not kept, sort+unique, but for stable remove duplicate
	auto dest = tracks.begin();
	for (const auto& t : tracks) {
		if (std::find_if(tracks.begin(), dest, [&](const auto& p) { return p.second.filename == t.second.filename; }) == dest) {
			*dest = t;
			++dest;
		}
	}
	tracks.erase(dest, tracks.end());
}

//------------------------------------------------------------------------------
void Playlist::shuffle()
{
	static thread_local std::default_random_engine rng{std::random_device()()};
	std::shuffle(tracks.begin(), tracks.end(), rng);
}

//------------------------------------------------------------------------------
void Playlist::info(std::ostream& os) const
{
	os << "Nb tracks: " << tracks.size() << "\n";
	for (const auto& [id, track] : tracks) {
		os << "- " << track.title << "\n";
	}
}

} // namespace iplayer
