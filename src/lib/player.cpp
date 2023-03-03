#include "player.h"

#include <algorithm>
#include <cassert>
#include <random>

namespace
{
//------------------------------------------------------------------------------
std::size_t rand_in(std::size_t lower, std::size_t upper)
{
	static thread_local std::default_random_engine rnd{std::random_device()()};
	return std::uniform_int_distribution<std::size_t>{lower, upper}(rnd);
}

//------------------------------------------------------------------------------
std::optional<std::size_t> pos_by_id(const iplayer::Playlist& playlist, std::size_t id)
{
	auto it =
		std::ranges::find_if(playlist.getTracks(), [&](const auto& p) { return p.first == id; });

	if (it != playlist.getTracks().end()) {
		return std::distance(playlist.getTracks().begin(), it);
	} else {
		return std::nullopt;
	}
}

} // namespace
namespace iplayer
{

//------------------------------------------------------------------------------
Player::Player(std::shared_ptr<IMusicPlayer> musicPlayer, Playlist&& playlist) :
	musicPlayer(std::move(musicPlayer)),
	displayedPlaylist(std::move(playlist)),
	randomOrderPlaylist(displayedPlaylist)
{
	this->musicPlayer->setOnMusicFinished([this]() {
		next();
		if (onMusicChanged) {
			onMusicChanged();
		}
	});
}

//------------------------------------------------------------------------------
void Player::setOnMusicChanged(std::function<void()> f)
{
	std::lock_guard l(mutex);
	onMusicChanged = f;
}

//------------------------------------------------------------------------------
void Player::play()
{
	std::lock_guard l(mutex);
	if (playing) return;

	auto next_and_play = [this](auto& optIndex) {
		if (!optIndex) {
			next(); // might change optIndex
		}
		if (optIndex) {
			playing = true;
			musicPlayer->play();
		}
	};
	next_and_play(randomModeActivated ? currentRandomSelectionIndex : currentSelectionIndex);
}

//------------------------------------------------------------------------------
void Player::pause()
{
	std::lock_guard l(mutex);
	if (!playing) return;
	playing = false;
	musicPlayer->pause();
}

//------------------------------------------------------------------------------
void Player::stop()
{
	using namespace std::literals;
	std::lock_guard l(mutex);
	if (playing) {
		musicPlayer->pause();
		playing = false;
	}
	musicPlayer->setElapsedTime(0s);
}

//------------------------------------------------------------------------------
void Player::previous()
{
	std::lock_guard l(mutex);

	if (randomModeActivated) {
		previous(randomOrderPlaylist, currentRandomSelectionIndex);
	} else {
		previous(displayedPlaylist, currentSelectionIndex);
	}
}

//------------------------------------------------------------------------------
void Player::previous(Playlist& playlist, std::optional<std::size_t>& optIndex)
{
	std::lock_guard l(mutex);

	const bool wasPlaying = playing;
	stop();
	if (playlist.getTracks().empty()) {
		return;
	}
	if (!optIndex) {
		if (randomModeActivated) {
			prepareRandomMode();
		}
		if (musicPlayer->openMusic(playlist.getTracks().back().second.filename)) {
			optIndex = playlist.getTracks().size() - 1;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	auto index = optIndex.value_or(playlist.getTracks().size() - 1);
	while (index != 0) {
		--index;
		if (musicPlayer->openMusic(playlist.getTracks()[index].second.filename)) {
			optIndex = index;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	if (repeatModeActivated) {
		optIndex.reset();
		playing = wasPlaying;
		previous(playlist, optIndex);
	}
}

//------------------------------------------------------------------------------
void Player::next()
{
	std::lock_guard l(mutex);

	if (randomModeActivated) {
		next(randomOrderPlaylist, currentRandomSelectionIndex);
	} else {
		next(displayedPlaylist, currentSelectionIndex);
	}
}

//------------------------------------------------------------------------------
void Player::next(Playlist& playlist, std::optional<std::size_t>& optIndex)
{
	std::lock_guard l(mutex);
	const bool wasPlaying = playing;
	stop();
	if (playlist.getTracks().empty()) {
		return;
	}
	if (!optIndex) {
		if (randomModeActivated) {
			prepareRandomMode();
		}
		if (musicPlayer->openMusic(playlist.getTracks()[0].second.filename)) {
			optIndex = 0;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	auto index = optIndex.value_or(0);
	while (index < playlist.getTracks().size() - 1) {
		++index;
		if (musicPlayer->openMusic(playlist.getTracks()[index].second.filename)) {
			optIndex = index;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	if (repeatModeActivated) {
		optIndex.reset();
		playing = wasPlaying;
		next(playlist, optIndex);
	}
}

//------------------------------------------------------------------------------
void Player::select(std::size_t n)
{
	std::lock_guard l(mutex);
	if (displayedPlaylist.getTracks().empty()) {
		return;
	}
	const bool wasPlaying = playing;
	stop();
	n = std::clamp(n, std::size_t(0), displayedPlaylist.getTracks().size());
	if (musicPlayer->openMusic(displayedPlaylist.getTracks()[n].second.filename)) {
		currentSelectionIndex = n;
		if (randomModeActivated) {
			prepareRandomMode();
		}
		if (wasPlaying) {
			play();
		}
	}
}

//------------------------------------------------------------------------------
void Player::prepareRandomMode()
{
	std::lock_guard l(mutex);
	randomOrderPlaylist.shuffle();
	if (currentSelectionIndex) {
		const auto& [id, track] = displayedPlaylist.getTracks()[*currentSelectionIndex];
		auto pos = pos_by_id(randomOrderPlaylist, id);
		if (pos) {
			randomOrderPlaylist.move(*pos, 0);
			currentRandomSelectionIndex = 0;
		}
	} else {
		currentRandomSelectionIndex.reset();
	}
}

//------------------------------------------------------------------------------
void Player::setRandomMode(bool value)
{
	std::lock_guard l(mutex);

	if (value && randomModeActivated != value) {
		prepareRandomMode();
	}
	randomModeActivated = value;
}

//------------------------------------------------------------------------------
void Player::setRepeatMode(bool value)
{
	repeatModeActivated = value;
}

//------------------------------------------------------------------------------
void Player::push_back(TrackHeader&& track)
{
	std::lock_guard l(mutex);
	displayedPlaylist.push_back(TrackHeader(track));
	randomOrderPlaylist.push_back(std::move(track));
	if (randomModeActivated && currentRandomSelectionIndex) {
		randomOrderPlaylist.move(
			randomOrderPlaylist.getTracks().size() - 1,
			rand_in(*currentSelectionIndex + 1, randomOrderPlaylist.getTracks().size() - 1));
	}
}
//------------------------------------------------------------------------------
void Player::insertAt(std::size_t pos, TrackHeader&& track)
{
	std::lock_guard l(mutex);
	displayedPlaylist.insertAt(pos, TrackHeader(track));
	if (currentSelectionIndex && pos <= currentSelectionIndex) {
		++*currentSelectionIndex;
	}
	randomOrderPlaylist.push_back(std::move(track));
	if (randomModeActivated && currentRandomSelectionIndex) {
		randomOrderPlaylist.move(
			randomOrderPlaylist.getTracks().size() - 1,
			rand_in(*currentSelectionIndex + 1, randomOrderPlaylist.getTracks().size() - 1));
	}
}
//------------------------------------------------------------------------------
void Player::remove(std::size_t pos)
{
	std::lock_guard l(mutex);

	pos = std::clamp(pos, std::size_t(0), displayedPlaylist.getTracks().size());
	auto id = displayedPlaylist.getTracks()[pos].first;
	displayedPlaylist.remove(pos);
	if (currentSelectionIndex && pos <= *currentSelectionIndex) {
		--*currentSelectionIndex;
	}

	auto optPos = pos_by_id(randomOrderPlaylist, id);
	assert(optPos);
	pos = *optPos;
	randomOrderPlaylist.remove(pos);
	if (currentRandomSelectionIndex && pos <= *currentRandomSelectionIndex) {
		--*currentRandomSelectionIndex;
	}
}

//------------------------------------------------------------------------------
void Player::move(std::size_t from, std::size_t to)
{
	std::lock_guard l(mutex);
	displayedPlaylist.move(from, to);

	if (currentSelectionIndex) {
		if (*currentSelectionIndex == from) {
			*currentSelectionIndex = to;
		} else if (from < *currentSelectionIndex && *currentSelectionIndex <= to) {
			--*currentSelectionIndex;
		} else if (to <= *currentSelectionIndex && *currentSelectionIndex < from) {
			++*currentSelectionIndex;
		}
	}
}
//------------------------------------------------------------------------------
void Player::removeDuplicate()
{
	std::lock_guard l(mutex);

	std::optional<std::size_t> id;
	if (currentSelectionIndex) {
		id = displayedPlaylist.getTracks()[*currentSelectionIndex].first;
	} else if (currentRandomSelectionIndex) {
		id = randomOrderPlaylist.getTracks()[*currentRandomSelectionIndex].first;
	}

	displayedPlaylist.removeDuplicate();
	randomOrderPlaylist = displayedPlaylist; // ensure ID are still identical.
	if (id) {
		currentSelectionIndex = pos_by_id(displayedPlaylist, *id);
		if (randomModeActivated) {
			randomOrderPlaylist.shuffle();
		}
		currentRandomSelectionIndex = pos_by_id(randomOrderPlaylist, *id);
	}
}

//------------------------------------------------------------------------------
void Player::info_tracks(std::ostream& os)
{
	std::lock_guard l(mutex);
	displayedPlaylist.info(os);
}

//------------------------------------------------------------------------------
void Player::info_track(std::ostream& os, std::size_t pos)
{
	std::lock_guard l(mutex);
	if (pos < displayedPlaylist.getTracks().size()) {
		infoTrack(os, displayedPlaylist.getTracks()[pos].second);
	}
}

//------------------------------------------------------------------------------
std::optional<std::size_t> Player::getSelectionIndex() const
{
	if (randomModeActivated) {
		if (!currentRandomSelectionIndex) {
			return std::nullopt;
		}
		const auto& [id, track] = randomOrderPlaylist.getTracks()[*currentRandomSelectionIndex];
		return pos_by_id(displayedPlaylist, id);
	} else {
		return currentSelectionIndex;
	}
}

} // namespace iplayer
