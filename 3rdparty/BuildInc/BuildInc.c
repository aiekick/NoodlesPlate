/*
MIT License

Copyright (c) 2022-2022 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024
#define BUILD_PREFIX "Prefix"
#define BUILD_WORD "BuildNumber"
#define MINOR_WORD "MinorNumber"
#define MAJOR_WORD "MajorNumber"
#define BUILD_ID_WORD "BuildId"

static int rule_maxBuildNumber            = 1000;
static int rule_maxMinorNumber            = 10;
static char prefix_buffer[MAX_LENGTH + 1] = "";

// return 1 if a prefix was found
int ParsePrefix(const char* vPrefix) {
    if (vPrefix && strlen(vPrefix)) {
        prefix_buffer[0] = '\0';
#ifdef _MSC_VER
        int res = sscanf_s(vPrefix, "-prefix=%s", prefix_buffer, MAX_LENGTH);
#else
        int res  = sscanf(vPrefix, "-prefix=%s", prefix_buffer);
#endif
        if (res == 1) {
            return 1;
        }
    }

    return 0;
}

// return 1 if a rule was found
int ParseRule(const char* vRule) {
    if (vRule && strlen(vRule)) {
        int maxBuild, maxMinor;
#ifdef _MSC_VER
        int res = sscanf_s(vRule, "%i:%i", &maxBuild, &maxMinor);
#else
        int res  = sscanf(vRule, "%i:%i", &maxBuild, &maxMinor);
#endif
        if (res == 2) {
            rule_maxBuildNumber = maxBuild;
            rule_maxMinorNumber = maxMinor;

            return 1;
        }
    }

    printf("-- The rule is missing or wrong\n");
    printf("-- The rule must respect this format max_build_number:max_minor_number\n");
    printf("-- The default rule of 1000:10 will be applied\n");

    return 0;
}

void ParseFile(const char* vFile) {
    int BuildNumber = 0;
    int MinorNumber = 0;
    int MajorNumber = 0;

    if (vFile && strlen(vFile)) {
        int idx = 0;
        char bufLine[MAX_LENGTH + 1];

#ifdef _MSC_VER
        FILE* fp          = NULL;
        const errno_t err = fopen_s(&fp, vFile, "r");
        if (err == 0 && fp)
#else
        FILE* fp = fopen(vFile, "r");
        if (fp)
#endif

        {
            // read
            while (!feof(fp) && idx < 3) {
                fgets(bufLine, MAX_LENGTH, fp);
                if (ferror(fp)) {
                    fprintf(stderr, "-- Reading error with code %d\n", errno);
                    break;
                }

                char bufKey[MAX_LENGTH + 1];
                bufKey[0] = '\0';

                int id       = 0;
                int is_found = 0;
                if (strlen(prefix_buffer)) { // is there is a not null prefix
                    char bufcmp[MAX_LENGTH + 1];
                    bufcmp[0] = '\0';
                    snprintf(bufcmp, MAX_LENGTH, "#define %s_%%s %%i", prefix_buffer);
#ifdef _MSC_VER
                    if (sscanf_s(bufLine, bufcmp, bufKey, MAX_LENGTH, &id) == 2)
#else
                    if (sscanf(bufLine, bufcmp, bufKey, &id) == 2)
#endif
                    {
                        is_found = 1;
                    }
                } else {
#ifdef _MSC_VER
                    if (sscanf_s(bufLine, "#define %s %i", bufKey, MAX_LENGTH, &id) == 2)
#else
                    if (sscanf(bufLine, "#define %s %i", bufKey, &id) == 2)
#endif
                    {
                        is_found = 1;
                    }
                }
                if (is_found) {
                    if (strcmp(bufKey, BUILD_WORD) == 0) {
                        BuildNumber = id;
                        ++idx;
                    } else if (strcmp(bufKey, MINOR_WORD) == 0) {
                        MinorNumber = id;
                        ++idx;
                    } else if (strcmp(bufKey, MAJOR_WORD) == 0) {
                        MajorNumber = id;
                        ++idx;
                    }
                }
            }
            fclose(fp);
        }

        if (idx == 3 || // parse du fichier ok
            fp == NULL) { // fichier non existant
            // treatment
            ++BuildNumber;
            if (BuildNumber > rule_maxBuildNumber) {
                BuildNumber = 0;
                ++MinorNumber;
            }
            if (MinorNumber > rule_maxMinorNumber) {
                MinorNumber = 0;
                ++MajorNumber;
            }

            // print vars :
            printf("-------------------- BuildInc ----------------------------------------------------------------------\n");
            if (strlen(prefix_buffer)) { // is there is a not null prefix
                printf("-- Prefix : %s\n", prefix_buffer);
            }
            printf("-- Build Id : %i.%i.%i\n", MajorNumber, MinorNumber, BuildNumber);
            printf("-- In file %s\n", vFile);
            printf("----------------------------------------------------------------------------------------------------\n");

#ifdef _MSC_VER
            FILE* fp          = NULL;
            const errno_t err = fopen_s(&fp, vFile, "w");
            if (err == 0 && fp)
#else
            FILE* fp = fopen(vFile, "w");
            if (fp)
#endif
            {
                fputs("#pragma once\n\n", fp);

                // write
                // BUILD_PREFIX
                int n = snprintf(bufLine, MAX_LENGTH, "#define %s_%s \"%s\"\n", prefix_buffer, BUILD_PREFIX, prefix_buffer);
                if (n) {
                    fwrite(bufLine, n, 1, fp);
                    int n = snprintf(bufLine, MAX_LENGTH, "#define %s_%s %i\n", prefix_buffer, BUILD_WORD, BuildNumber);
                    if (n) {
                        fwrite(bufLine, n, 1, fp);
                        n = snprintf(bufLine, MAX_LENGTH, "#define %s_%s %i\n", prefix_buffer, MINOR_WORD, MinorNumber);
                        if (n) {
                            fwrite(bufLine, n, 1, fp);
                            n = snprintf(bufLine, MAX_LENGTH, "#define %s_%s %i\n", prefix_buffer, MAJOR_WORD, MajorNumber);
                            if (n) {
                                fwrite(bufLine, n, 1, fp);
                                n = snprintf(bufLine, MAX_LENGTH, "#define %s_%s \"%i.%i.%i\"\n", prefix_buffer, BUILD_ID_WORD, MajorNumber, MinorNumber, BuildNumber);
                                if (n) {
                                    fwrite(bufLine, n, 1, fp);
                                }
                            }
                        }
                    }
                }

                fclose(fp);
            }
        } else {
            printf("-------------------- BuildInc ----------------------------------------------------------------------\n");
            if (strlen(prefix_buffer)) {  // is there is a not null prefix
                printf("-- Prefix : %s\n", prefix_buffer);
            }
            printf("-- Error in parsing\n");
            printf("-- Build Id is unchanged : %i %i %i\n", MajorNumber, MinorNumber, BuildNumber);
            printf("-- In file %s\n", vFile);
            printf("----------------------------------------------------------------------------------------------------\n");
        }
    }
}

int main(int argc, char* argv[]) {  // Don't forget first integral argument 'argc'
    /*
    the build id is : MajorNumber.MinorNumber.BuildNumber
    if the rule is "1000:10" this correspond to :
    if (BuildNumber > 1000)
        ++MinorNumber;
    if (MinorNumber > 10)
        ++MajorNumber;
    */

    if (argc == 1) {
        printf("-------------------- BuildInc --------------------\n");
        printf("-- this func will increment in a c/c++ include file, 3 vars : MajorNumber, MinorNumber and BuildNumber, according to a rule\n");
        printf("-- the syntax is : BuildInc -prefix=\"prefix\" rule include_file\n");
        printf("-- the rule is 'max_build_number:max_minor_number' \n");
        printf("-- by ex with a rule of 1000:10 the corresponding pseudo code will be :\n");
        printf("-- if (BuildNumber > 1000) ++MinorNumber;\n");
        printf("-- if (MinorNumber > 10) ++MajorNumber\n");
        printf("-- the Build id will be MajorNumber.MinorNumber.BuildNumber\n");
        printf("--------------------------------------------------\n");
    }

    int idx = 1;
    if (argc > 1) {
        ParsePrefix(argv[idx++]);
        int res = ParseRule(argv[idx]);
        if (res)
            ++idx;
        if (argc > 2 || res == 0)
            ParseFile(argv[idx]);
    }

    return 0;
}
