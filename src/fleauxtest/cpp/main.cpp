#include <cstring>
#include <iostream>
#include "../../fleaux/headers/gap_buffer.h"
#include "../../fleaux/headers/dictionary.h"
#include "gtest/gtest.h"

using namespace std;

namespace {

    class GapBufferTest : public ::testing::Test
    {
        protected:
            GapBufferTest()
            {
                gb0 = fl_gapBuffer_create(8);
                gb1 = fl_gapBuffer_create(32);
                fl_gapBuffer_insertString(gb0, "this is a test string", 21);
                fl_gapBuffer_insertString(gb1, "this is a test string", 21);
            }

            ~GapBufferTest()
            {
                fl_gapBuffer_destroy(gb0);
                fl_gapBuffer_destroy(gb1);
            }


            fl_gapBuffer_t *gb0;
            fl_gapBuffer_t *gb1;

    };

    TEST_F(GapBufferTest, InsertTest)
    {
        const char *testString0 = "this is a test string";
        ASSERT_EQ(memcmp(testString0, fl_gapBuffer_getData(gb0), gb0->length), 0);
        const char *testString1 = ". adding more text here";
        fl_gapBuffer_insertString(gb0, testString1, 23);
        ASSERT_EQ(memcmp("this is a test string. adding more text here", fl_gapBuffer_getData(gb0), gb0->length), 0);
        const char *testString2 = "this is a test string.";
        fl_gapBuffer_insertChar(gb1, '.');
        ASSERT_EQ(memcmp(testString2, fl_gapBuffer_getData(gb1), gb1->length), 0);
    }

    TEST_F(GapBufferTest, RemoveTest)
    {
        fl_gapBuffer_remove(gb0, 6, FL_1D_BACKWARD);
        fl_gapBuffer_insertString(gb0, "dude.", 5);
        const char *testString0 = "this is a test dude.";
        ASSERT_EQ(memcmp(testString0, fl_gapBuffer_getData(gb0), gb0->length), 0);

        fl_gapBuffer_remove(gb1, 1, FL_1D_BACKWARD);
        fl_gapBuffer_insertString(gb1, "g. And this is some more.", 25);
        const char *testString1 = "this is a test string. And this is some more.";
        ASSERT_EQ(memcmp(testString1, fl_gapBuffer_getData(gb1), gb1->length), 0);
    }

    TEST_F(GapBufferTest, MoveGapTest)
    {
        fl_gapBuffer_moveGap(gb0, 6, FL_1D_BACKWARD);
        fl_gapBuffer_insertString(gb0, "mac daddy ", 10);
        const char *testString0 = "this is a test mac daddy string";
        ASSERT_EQ(memcmp(testString0, fl_gapBuffer_getData(gb0), gb0->length), 0);

        fl_gapBuffer_moveGap(gb0, 3, FL_1D_FORWARD);
        fl_gapBuffer_insertString(gb0, "izz", 3);
        const char *testString1 = "this is a test mac daddy strizzing";
        ASSERT_EQ(memcmp(testString1, fl_gapBuffer_getData(gb0), gb0->length), 0);
    }

    class DictionaryTest : public ::testing::Test
    {
        protected:
            DictionaryTest()
            {
                dict0 = fl_dictionary_create();
            }

            ~DictionaryTest()
            {
                fl_dictionary_destroy(dict0);
            }

            fl_dictionary_t *dict0;
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
        ASSERT_EQ(fl_dictionary_set(dict0, testKey0, testValue0, 16), 0);
        ASSERT_EQ(fl_dictionary_set(dict0, testKey1, testValue1, 15), 0);
        ASSERT_EQ(fl_dictionary_set(dict0, testKey2, testValue2, 15), 0);
        ASSERT_EQ(fl_dictionary_set(dict0, testKey3, testValue3, 17), 0);
        ASSERT_STREQ(testValue1, (char *)fl_dictionary_get(dict0, testKey1));
        ASSERT_STREQ(testValue3, (char *)fl_dictionary_get(dict0, testKey3));
        ASSERT_STREQ(testValue0, (char *)fl_dictionary_get(dict0, testKey0));
        ASSERT_STREQ(testValue2, (char *)fl_dictionary_get(dict0, testKey2));
        
        fl_dictionary_remove(dict0, testKey0);
        fl_dictionary_remove(dict0, testKey2);
        ASSERT_TRUE(fl_dictionary_get(dict0, testKey0) == NULL);
        ASSERT_TRUE(fl_dictionary_get(dict0, testKey2) == NULL);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
