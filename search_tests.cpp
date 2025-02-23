#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/search.h"

using namespace std;
using namespace testing;

TEST(FakeTest, PleaseDeleteOnceYouWriteSome) {
  // If no tests exist, GoogleTest fails to compile with a fairly confusing
  // message. We have this empty test that does nothing to allow an empty
  // autograder submission to compile and regularly fail. Once you write your
  // own tests for the project, you can delete this one.
  EXPECT_THAT(1, Eq(1));
}

TEST(CleanToken, PrefixAndSuffixCleaning) {
  ASSERT_THAT(cleanToken(".HELLO."), StrEq("hello"));
  ASSERT_THAT(cleanToken("...heLlo..."), StrEq("hello"));
  ASSERT_THAT(cleanToken(".............hellO.\"!?"), StrEq("hello"));
}

TEST(CleanToken, keepMiddlePunct) {
  ASSERT_THAT(cleanToken("HE..LLO"), StrEq("he..llo"));
  ASSERT_THAT(cleanToken("...heLl.o"), StrEq("hell.o"));
  ASSERT_THAT(cleanToken("heLl.o...."), StrEq("hell.o"));
  ASSERT_THAT(cleanToken("..h.ellO.\"!?"), StrEq("h.ello"));
}

TEST(GatherTokens, Trailspaces) {
  string text = "This is a test for trailing spaces             ";
  set<string> expected = {"this", "is",       "a",     "test",
                          "for",  "trailing", "spaces"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(GatherTokens, Leadspaces) {
  string text = "          This is a test for leading spaces";
  set<string> expected = {"this", "is",      "a",     "test",
                          "for",  "leading", "spaces"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(GatherTokens, Gapspaces) {
  string text = "This   is   a   test   for        large   gaps";
  set<string> expected = {"this", "is", "a", "test", "for", "large", "gaps"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(BuildIndex, noFile) {
  string filename = "blank.txt";
  map<string, set<string>> studentIndex;
  int studentNumProcessed = buildIndex(filename, studentIndex);

  string indexTestFeedback =
      "buildIndex(\"" + filename +
      "\", ...) index should be empty when file is missing\n";
  EXPECT_TRUE(studentIndex.empty()) << indexTestFeedback;

  string retTestFeedback = "buildIndex(\"" + filename +
                           "\", ...) should return 0 when file is missing\n";
  EXPECT_THAT(studentNumProcessed, Eq(0)) << retTestFeedback;
}

TEST(BuildIndex, TinyTxt) {
  string filename = "data/tiny.txt";

  map<string, set<string>> expectedIndex = {
      // www.shoppinglist.com
      {"eggs", {"www.shoppinglist.com"}},
      {"milk", {"www.shoppinglist.com"}},
      {"fish", {"www.shoppinglist.com", "www.dr.seuss.net"}},
      {"bread", {"www.shoppinglist.com"}},
      {"cheese", {"www.shoppinglist.com"}},

      // www.rainbow.org
      {"red", {"www.rainbow.org", "www.dr.seuss.net"}},
      {"gre-en", {"www.rainbow.org"}},
      {"orange", {"www.rainbow.org"}},
      {"yellow", {"www.rainbow.org"}},
      {"blue", {"www.rainbow.org", "www.dr.seuss.net"}},
      {"indigo", {"www.rainbow.org"}},
      {"violet", {"www.rainbow.org"}},

      // www.dr.seuss.net
      {"one", {"www.dr.seuss.net"}},
      {"two", {"www.dr.seuss.net"}},

      // www.bigbadwolf.com
      {"i'm", {"www.bigbadwolf.com"}},
      {"not", {"www.bigbadwolf.com"}},
      {"trying", {"www.bigbadwolf.com"}},
      {"to", {"www.bigbadwolf.com"}},
      {"eat", {"www.bigbadwolf.com"}},
      {"you", {"www.bigbadwolf.com"}}};

  map<string, set<string>> studentIndex;
  int studentNumProcessed = buildIndex(filename, studentIndex);

  string indexTestFeedback =
      "buildIndex(\"" + filename + "\", ...) index incorrect\n";
  EXPECT_THAT(studentIndex, ContainerEq(expectedIndex)) << indexTestFeedback;

  string retTestFeedback =
      "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
  EXPECT_THAT(studentNumProcessed, Eq(4)) << retTestFeedback;
}

map<string, set<string>> INDEX = {
    {"hello", {"example.com", "uic.edu"}},
    {"there", {"example.com"}},
    {"according", {"uic.edu"}},
    {"to", {"uic.edu"}},
    {"all", {"example.com", "uic.edu", "random.org"}},
    {"known", {"uic.edu"}},
    {"laws", {"random.org"}},
    {"of", {"random.org"}},
    {"aviation", {"random.org"}},
    {"a", {"uic.edu", "random.org"}},
};

TEST(FindQueryMatches, FirstTermNotInIndex) {
  // should return an empty set
  EXPECT_THAT(findQueryMatches(INDEX, "bad"), ContainerEq(set<string>{}));

  // first term not in set followed by "hello" should return empty
  EXPECT_THAT(findQueryMatches(INDEX, "bad +hello"),
              ContainerEq(set<string>{}));
}

TEST(FindQueryMatches, LaterTermUnmodifiedNotInIndex) {
  // just hello in set
  EXPECT_THAT(findQueryMatches(INDEX, "hello bad"),
              ContainerEq(set<string>{"example.com", "uic.edu"}));
}

TEST(FindQueryMatches, LaterTermPlusNotInIndex) {
  // +bad should cause set to be empty
  EXPECT_THAT(findQueryMatches(INDEX, "hello +bad"),
              ContainerEq(set<string>{}));
}

TEST(FindQueryMatches, LaterTermMinusNotInIndex) {
  // -bad should do nothing and set is just hello
  EXPECT_THAT(findQueryMatches(INDEX, "hello -bad"),
              ContainerEq(set<string>{"example.com", "uic.edu"}));
}
