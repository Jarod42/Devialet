#include "playlist.h"

#include <doctest.h>

namespace
{

//------------------------------------------------------------------------------
iplayer::Track makeTrack(std::size_t n)
{
	return {.filename = "file" + std::to_string(n),
	        .title = "Title" + std::to_string(n),
	        .duration{std::chrono::seconds(n)}};
}

//------------------------------------------------------------------------------
iplayer::Playlist buildPlaylist(const std::vector<std::size_t>& v)
{
	iplayer::Playlist res;
	for (std::size_t n : v) {
		res.push_back(makeTrack(n));
	}
	return res;
}

//------------------------------------------------------------------------------
std::vector<int> getOrder(const iplayer::Playlist& p)
{
	std::vector<int> res(p.getTracks().size());

	std::transform(
		p.getTracks().begin(), p.getTracks().end(), res.begin(), [](const auto& track) -> int {
			return static_cast<int>(track.duration.count());
		});
	return res;
}

} // namespace

//------------------------------------------------------------------------------
TEST_CASE("insertAt")
{
	auto playlist = buildPlaylist({});
	REQUIRE_EQ(std::vector<int>{}, getOrder(playlist));

	playlist.insertAt(2, makeTrack(3)); // out of range
	CHECK_EQ(std::vector{3}, getOrder(playlist));

	playlist.insertAt(0, makeTrack(0));
	CHECK_EQ(std::vector{0, 3}, getOrder(playlist));

	playlist.insertAt(1, makeTrack(1));
	CHECK_EQ(std::vector{0, 1, 3}, getOrder(playlist));

	playlist.insertAt(3, makeTrack(4));
	CHECK_EQ(std::vector{0, 1, 3, 4}, getOrder(playlist));
}


//------------------------------------------------------------------------------
TEST_CASE("remove")
{
	auto playlist = buildPlaylist({0, 1, 2, 3, 4});
	REQUIRE_EQ(std::vector{0, 1, 2, 3, 4}, getOrder(playlist));

	playlist.remove(2);
	CHECK_EQ(std::vector{0, 1, 3, 4}, getOrder(playlist));

	playlist.remove(3);
	CHECK_EQ(std::vector{0, 1, 3}, getOrder(playlist));

	playlist.remove(3); // out of ranges
	CHECK_EQ(std::vector{0, 1, 3}, getOrder(playlist));

	playlist.remove(0); // out of ranges
	CHECK_EQ(std::vector{1, 3}, getOrder(playlist));
}

//------------------------------------------------------------------------------
TEST_CASE("move")
{
	auto playlist = buildPlaylist({0, 1, 2, 3, 4});
	REQUIRE_EQ(std::vector{0, 1, 2, 3, 4}, getOrder(playlist));

	playlist.move(3, 1);
	CHECK_EQ(std::vector{0, 3, 1, 2, 4}, getOrder(playlist));

	playlist.move(1, 3);
	CHECK_EQ(std::vector{0, 1, 2, 3, 4}, getOrder(playlist));
}

//------------------------------------------------------------------------------
TEST_CASE("removeDuplicate")
{
	auto playlist = buildPlaylist({0, 1, 2, 1, 3, 3, 4});
	playlist.removeDuplicate();
	CHECK_EQ(std::vector{0, 1, 2, 3, 4}, getOrder(playlist));
}

//------------------------------------------------------------------------------
TEST_CASE("info")
{
	const auto playlist = buildPlaylist({1, 2, 3});
	const std::string expected = R"(Nb tracks: 3
- Title1
- Title2
- Title3
)";
	std::stringstream ss;

	playlist.info(ss);

	CHECK_EQ(expected, ss.str());
}
