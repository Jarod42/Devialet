#include "trackheader.h"

#include <doctest.h>
#include <sstream>

using namespace std::literals;

//------------------------------------------------------------------------------
TEST_CASE("open track")
{
	const iplayer::TrackHeader expected{.filename = "c:/somepath/track.txt", .title = "Some title", .duration = 42s};

	std::stringstream ss;
	ss << std::quoted(expected.title) << " " << expected.duration.count();
	
	const iplayer::TrackHeader track = iplayer::openTrackHeader(expected.filename, ss);
	CHECK_EQ(expected, track);
}

//------------------------------------------------------------------------------
TEST_CASE("info")
{
	const iplayer::TrackHeader track{.filename = "c:/somepath/track.txt", .title = "Some title", .duration = 42s};
	const std::string expected = R"(Filename: track.txt
Title: Some title
Duration: 42s
)";
	std::ostringstream ss;

	iplayer::infoTrack(ss, track);
	CHECK_EQ(expected, ss.str());
}

//------------------------------------------------------------------------------
TEST_CASE("data")
{
	// working dir is at solution/$buildsystem/
	const std::filesystem::path dataDir = "../../data";

	CHECK_NOTHROW(iplayer::openTrackHeader(dataDir / "track1"));
	CHECK_NOTHROW(iplayer::openTrackHeader(dataDir / "track2"));
	CHECK_NOTHROW(iplayer::openTrackHeader(dataDir / "track3"));
	CHECK_NOTHROW(iplayer::openTrackHeader(dataDir / "track4"));

	CHECK_THROWS(iplayer::openTrackHeader(dataDir / "not-exist"));
	CHECK_THROWS(iplayer::openTrackHeader(dataDir / "invalid1"));
	CHECK_THROWS(iplayer::openTrackHeader(dataDir / "invalid2"));
}
