#include "player.h"

namespace iplayer
{

//------------------------------------------------------------------------------
Player::Player(std::shared_ptr<IMusicPlayer> musicPlayer, Playlist&& playlist) :
	musicPlayer(std::move(musicPlayer)),
	displayedPlaylist(std::move(playlist)),
	randomOrderPlaylist(displayedPlaylist)
{
	this->musicPlayer->setOnMusicFinished([=]() { next(); });
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
	if (!optIndex) {
		if (playlist.getTracks().empty()) {
			std::cerr << "empty playlist";
			return;
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
void Player::next(Playlist&playlist, std::optional<std::size_t>& optIndex)
{
	std::lock_guard l(mutex);
	const bool wasPlaying = playing;
	stop();
	if (playlist.getTracks().empty()) {
		return;
	}
	if (!optIndex) {
		if (musicPlayer->openMusic(playlist.getTracks()[0].second.filename)) {
			optIndex = 0;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	auto index = optIndex.value_or(0);
	while (index < playlist.getTracks().size() - 1)
	{
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
	randomOrderPlaylist.shuffle();
	if (currentSelectionIndex) {
		const auto& [id, track] = displayedPlaylist.getTracks()[*currentSelectionIndex];
		auto it = std::ranges::find_if(randomOrderPlaylist.getTracks(),
		                               [&](const auto& p) { return p.first == id; });
		if (it != randomOrderPlaylist.getTracks().end()) {
			randomOrderPlaylist.move(std::distance(randomOrderPlaylist.getTracks().begin(), it), 0);
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
	
	if (value&& randomModeActivated != value) {
		prepareRandomMode();
	}
	randomModeActivated = value;
}

//------------------------------------------------------------------------------
void Player::setRepeatMode(bool value)
{
	repeatModeActivated = value;
}

} // namespace iplayer
