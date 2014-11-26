#include <cstring>
#include <iostream>
#include "../../sdnbc/headers/gap_buffer.h"
#include "../../sdnbc/headers/dictionary.h"
#include "gtest/gtest.h"

using namespace std;

namespace {

    class GapBufferTest : public ::testing::Test
    {
        protected:
            GapBufferTest()
            {
                gb0 = sdnb_gapBuffer_create(8);
                gb1 = sdnb_gapBuffer_create(32);
                sdnb_gapBuffer_insertString(gb0, "this is a test string", 21);
                sdnb_gapBuffer_insertString(gb1, "this is a test string", 21);
            }

            ~GapBufferTest()
            {
                sdnb_gapBuffer_destroy(gb0);
                sdnb_gapBuffer_destroy(gb1);
            }


            sdnb_gapBuffer_t *gb0;
            sdnb_gapBuffer_t *gb1;

    };

    TEST_F(GapBufferTest, InsertTest)
    {
        const char *testString0 = "this is a test string";
        ASSERT_EQ(memcmp(testString0, sdnb_gapBuffer_getData(gb0, 0, gb0->length), gb0->length), 0);
        const char *testString1 = ". adding more text here";
        sdnb_gapBuffer_insertString(gb0, testString1, 23);
        ASSERT_STREQ("this is a test string. adding more text here", sdnb_gapBuffer_getData(gb0, 0, gb0->length));
        const char *testString2 = "this is a test string.";
        sdnb_gapBuffer_insertChar(gb1, '.');
        ASSERT_STREQ(testString2, sdnb_gapBuffer_getData(gb1, 0, gb1->length));
    }

    TEST_F(GapBufferTest, RemoveTest)
    {
        sdnb_gapBuffer_remove(gb0, -6);
        sdnb_gapBuffer_insertString(gb0, "dude.", 5);
        const char *testString0 = "this is a test dude.";
        ASSERT_STREQ(testString0, sdnb_gapBuffer_getData(gb0, 0, gb0->length));

        sdnb_gapBuffer_remove(gb1, -1);
        sdnb_gapBuffer_insertString(gb1, "g. And this is some more.", 25);
        const char *testString1 = "this is a test string. And this is some more.";
        ASSERT_STREQ(testString1, sdnb_gapBuffer_getData(gb1, 0, gb1->length));
    }

    TEST_F(GapBufferTest, MoveGapTest)
    {
        sdnb_gapBuffer_moveGap(gb0, -6);
        sdnb_gapBuffer_insertString(gb0, "mac daddy ", 10);
        const char *testString0 = "this is a test mac daddy string";
        ASSERT_STREQ(testString0, sdnb_gapBuffer_getData(gb0, 0, gb0->length));

        sdnb_gapBuffer_moveGap(gb0, 3);
        sdnb_gapBuffer_insertString(gb0, "izz", 3);
        const char *testString1 = "this is a test mac daddy strizzing";
        ASSERT_STREQ(testString1, sdnb_gapBuffer_getData(gb0, 0, gb0->length));
    }

    class DictionaryTest : public ::testing::Test
    {
        protected:
            DictionaryTest()
            {
                dict0 = sdnb_dictionary_create();
            }

            ~DictionaryTest()
            {
                sdnb_dictionary_destroy(dict0);
            }

            sdnb_dictionary_t *dict0;
    };

    TEST_F(DictionaryTest, SetGetRemoveTest)
    {
        char testKey0[] = "key";
        char testKey1[] = "anotherkey";
        char testKey2[] = "random";
        char testKey3[] = "@@9"; //should collide with "key"
        char testValue0[] = "test value zero";
        char testValue1[] = "test value one";
        char testValue2[] = "test value two";
        char testValue3[] = "test value three";
        //make sure inserts don't return errors
        ASSERT_EQ(sdnb_dictionary_set(dict0, testKey0, testValue0, 16), 0);
        ASSERT_EQ(sdnb_dictionary_set(dict0, testKey1, testValue1, 15), 0);
        ASSERT_EQ(sdnb_dictionary_set(dict0, testKey2, testValue2, 15), 0);
        ASSERT_EQ(sdnb_dictionary_set(dict0, testKey3, testValue3, 17), 0);
        ASSERT_STREQ(testValue1, (char *)sdnb_dictionary_get(dict0, testKey1));
        ASSERT_STREQ(testValue3, (char *)sdnb_dictionary_get(dict0, testKey3));
        ASSERT_STREQ(testValue0, (char *)sdnb_dictionary_get(dict0, testKey0));
        ASSERT_STREQ(testValue2, (char *)sdnb_dictionary_get(dict0, testKey2));
        
        sdnb_dictionary_remove(dict0, testKey0);
        sdnb_dictionary_remove(dict0, testKey2);
        ASSERT_TRUE(sdnb_dictionary_get(dict0, testKey0) == NULL);
        ASSERT_TRUE(sdnb_dictionary_get(dict0, testKey2) == NULL);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
