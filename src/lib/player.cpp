#include "player.h"

namespace iplayer
{

//------------------------------------------------------------------------------
Player::Player(std::shared_ptr<IMusicPlayer> musicPlayer, Playlist&& playlist) :
	musicPlayer(std::move(musicPlayer)),
	playlist(std::move(playlist))
{
	this->musicPlayer->setOnMusicFinished([=]() { next(); });
}

//------------------------------------------------------------------------------
void Player::play()
{
	std::lock_guard l(mutex);
	if (playing) return;

	if (!currentSelectionIndex) {
		next();
	}
	if (currentSelectionIndex) {
		playing = true;
		musicPlayer->play();
	}
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

	const bool wasPlaying = playing;
	stop();
	if (!currentSelectionIndex) {
		if (playlist.getTracks().empty()) {
			std::cerr << "empty playlist";
			return;
		}
		if (musicPlayer->openMusic(playlist.getTracks().back().filename)) {
			currentSelectionIndex = playlist.getTracks().size() - 1;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	auto index = currentSelectionIndex.value_or(playlist.getTracks().size() - 1);
	while (index != 0) {
		--index;
		if (musicPlayer->openMusic(playlist.getTracks()[index].filename)) {
			currentSelectionIndex = index;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	if (repeatModeActivated) {
		currentSelectionIndex.reset();
		playing = wasPlaying;
		previous();
	}
}

//------------------------------------------------------------------------------
void Player::next()
{
	std::lock_guard l(mutex);

	const bool wasPlaying = playing;
	stop();
	if (playlist.getTracks().empty()) {
		return;
	}
	if (!currentSelectionIndex) {
		if (musicPlayer->openMusic(playlist.getTracks()[0].filename)) {
			currentSelectionIndex = 0;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	auto index = currentSelectionIndex.value_or(0);
	while (index < playlist.getTracks().size() - 1)
	{
		++index;
		if (musicPlayer->openMusic(playlist.getTracks()[index].filename)) {
			currentSelectionIndex = index;
			if (wasPlaying) {
				play();
			}
			return;
		}
	}
	if (repeatModeActivated) {
		currentSelectionIndex.reset();
		playing = wasPlaying;
		next();
	}
}

//------------------------------------------------------------------------------
void Player::select(std::size_t n)
{
	std::lock_guard l(mutex);
	if (playlist.getTracks().empty()) {
		return;
	}
	const bool wasPlaying = playing;
	stop();
	n = std::clamp(n, std::size_t(0), playlist.getTracks().size());
	if (musicPlayer->openMusic(playlist.getTracks()[n].filename)) {
		currentSelectionIndex = n;
		if (wasPlaying) {
			play();
		}
	}
}

#if 0
//------------------------------------------------------------------------------
void Player::setRandomMode(bool value)
{
	randomModeActivated = value;
}
#endif
//------------------------------------------------------------------------------
void Player::setRepeatMode(bool value)
{
	repeatModeActivated = value;
}

} // namespace iplayer
