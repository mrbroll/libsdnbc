#include <cstring>
#include <iostream>
#include "../../sdnbc/headers/gap_buffer.h"
#include "../../sdnbc/headers/dictionary.h"
#include "../../sdnbc/headers/ptr.h"
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
        char *dataStr2 = (char *)malloc(gb0->length + 1);
        sdnb_gapBuffer_getData(gb0, dataStr2, 0, gb0->length);
        ASSERT_STREQ(testString0, dataStr2);
        free(dataStr2);
        const char *testString1 = ". adding more text here";
        sdnb_gapBuffer_insertString(gb0, testString1, 23);
        char *dataStr0 = (char *)malloc(gb0->length + 1);
        sdnb_gapBuffer_getData(gb0, dataStr0, 0, gb0->length);
        ASSERT_STREQ("this is a test string. adding more text here", dataStr0);
        free(dataStr0);
        const char *testString2 = "this is a test string.";
        sdnb_gapBuffer_insertChar(gb1, '.');
        char *dataStr1 = (char *)malloc(gb1->length + 1);
        sdnb_gapBuffer_getData(gb1, dataStr1, 0, gb1->length);
        ASSERT_STREQ(testString2, dataStr1);
        free(dataStr1);
    }

    TEST_F(GapBufferTest, RemoveTest)
    {
        sdnb_gapBuffer_remove(gb0, -6);
        sdnb_gapBuffer_insertString(gb0, "dude.", 5);
        const char *testString0 = "this is a test dude.";
        char *dataStr0 = (char *)malloc(gb0->length + 1);
        sdnb_gapBuffer_getData(gb0, dataStr0, 0, gb0->length);
        ASSERT_STREQ(testString0, dataStr0);
        free(dataStr0);
        sdnb_gapBuffer_remove(gb1, -1);
        sdnb_gapBuffer_insertString(gb1, "g. And this is some more.", 25);
        const char *testString1 = "this is a test string. And this is some more.";
        char *dataStr1 = (char *)malloc(gb1->length + 1);
        sdnb_gapBuffer_getData(gb1, dataStr1, 0, gb1->length);
        ASSERT_STREQ(testString1, dataStr1);
        free(dataStr1);
    }

    TEST_F(GapBufferTest, MoveGapTest)
    {
        sdnb_gapBuffer_moveGap(gb0, -6);
        sdnb_gapBuffer_insertString(gb0, "mac daddy ", 10);
        const char *testString0 = "this is a test mac daddy string";
        char *dataStr0 = (char *)malloc(gb0->length + 1);
        sdnb_gapBuffer_getData(gb0, dataStr0, 0, gb0->length);
        ASSERT_STREQ(testString0, dataStr0);
        free(dataStr0);
        sdnb_gapBuffer_moveGap(gb0, 3);
        sdnb_gapBuffer_insertString(gb0, "izz", 3);
        const char *testString1 = "this is a test mac daddy strizzing";
        char *dataStr1 = (char *)malloc(gb0->length + 1);
        sdnb_gapBuffer_getData(gb0, dataStr1, 0, gb0->length);
        ASSERT_STREQ(testString1, dataStr1);
        free(dataStr1);
    }

    TEST_F(GapBufferTest, IterTest)
    {
        //out of bounds
        char c = sdnb_gapBuffer_iterSet(gb0, 22);
        ASSERT_EQ(c, '\0');

        char str0[21] = "";
        c = sdnb_gapBuffer_iterSet(gb0, 0);
        strncat(str0, &c, 1);
        for (int i = 1; i < gb0->length; i++) {
            c = sdnb_gapBuffer_iterNext(gb0);
            strncat(str0, &c, 1);
        }
        char *dataStr = (char *)malloc(gb0->length + 1);
        sdnb_gapBuffer_getData(gb0, dataStr, 0, gb0->length);
        ASSERT_STREQ(dataStr, str0);
        free(dataStr);

        char str1[21] = "";
        const char *dataStr1 = "gnirts tset a si siht";
        c = sdnb_gapBuffer_iterSet(gb0, gb0->length - 1);
        strncat(str1, &c, 1);
        for (int i = 1; i < gb0->length; i++) {
            c = sdnb_gapBuffer_iterPrev(gb0);
            strncat(str1, &c, 1);
        }

        sdnb_gapBuffer_moveGap(gb0, -6);
        sdnb_gapBuffer_insertString(gb0, "big ", 4);
        char str3[32] = "";
        const char *dataStr3 = "this is a test big string";
        c = sdnb_gapBuffer_iterSet(gb0, 0);
        strncat(str3, &c, 1);
        for (int i = 1; i < gb0->length; i++) {
            c = sdnb_gapBuffer_iterNext(gb0);
            strncat(str3, &c, 1);
        }
        ASSERT_STREQ(dataStr3, str3);

        const char *dataStr4 = "gnirts gib tset a si siht";
        char str4[32] = "";
        c = sdnb_gapBuffer_iterSet(gb0, gb0->length - 1);
        strncat(str4, &c, 1);
        for (int i = 1; i < gb0->length; i++) {
            c = sdnb_gapBuffer_iterPrev(gb0);
            strncat(str4, &c, 1);
        }
        ASSERT_STREQ(dataStr4, str4);
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

    class PtrTest : public ::testing::Test
    {
        protected:
            PtrTest()
            {
                char testString[] = "This is test ptr data!";
                ptr0 = sdnb_ptr_create(testString, strlen(testString) + 1);
            }

            ~PtrTest()
            {
                sdnb_ptr_destroy(ptr0);
            }

            sdnb_ptr_t *ptr0;
    };

    //Just Testing to make sure that nothing breaks
    //Implementation is too simple to do any real testing
    TEST_F(PtrTest, IncrefDecrefTest)
    {
        sdnb_ptr_t *ptr1 = sdnb_ptr_incref(ptr0);
        ASSERT_STREQ((char *)ptr0->data, (char *)ptr1->data);
        sdnb_ptr_decref(ptr1); 
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
