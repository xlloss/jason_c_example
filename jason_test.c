#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mcheck.h>
#include <json-c/json.h>

#define READ_FILE_BUF 256
#define ONE_LOAD 15
#define READ_FILE_PATH "test.txt"

#define KEY_CHECK_FILE "success"
#define KEY_TEST "district";

struct json_object *find_target(struct json_object *jobj, const char *key)
{
	struct json_object *tmp;

	json_object_object_get_ex(jobj, key, &tmp);
	return tmp;
}

int test(char *dist)
{
    FILE *fp = NULL;
    char buf[READ_FILE_BUF] = {0};
    char str[ONE_LOAD];
    const char *test_my_str;
    char *tname = KEY_TEST;
    char *key_name = KEY_CHECK_FILE;
    const char *gstr = NULL; //get string
	json_object *jobj;
	json_object *val_obj = NULL;


/*
	char *str_test = "{ \"msg-type\": [ \"0xdeadbeef\", \"irc log\" ], \
		\"msg-from\": { \"class\": \"soldier\", \"name\": \"Wixilav\" }, \
		\"msg-to\": { \"class\": \"supreme-commander\", \"name\": \"[Redacted]\" }, \
		\"msg-log\": [ \
			\"soldier: Boss there is a slight problem with the piece offering to humans\", \
			\"supreme-commander: Explain yourself soldier!\", \
			\"soldier: Well they don't seem to move anymore...\", \
			\"supreme-commander: Oh snap, I came here to see them twerk!\" \
			] \
		}";
*/

    if ((fp = fopen (READ_FILE_PATH, "r")) == NULL) {
        fprintf(stderr, "Can't open file\n");
        return 1;
    }

	fread(buf, 30, 1, fp);
	fprintf(stdout, "buf=%s\n",buf);

//    while (!feof(fp) && !ferror(fp)) {
//        if (strlen(buf) > READ_FILE_BUF - ONE_LOAD) {
//            fprintf(stdout, "buf oversize\n");
//            goto end;
//        }
//
//        if (fgets(str, ONE_LOAD, fp) != NULL) {
//            str[strlen(str) - 1] = 0x20;
//            strncat(buf, str, ONE_LOAD);
//            fprintf(stdout, "buf=%s\n", buf);
//        }
//    }



    jobj = json_tokener_parse(buf);
    if (!jobj) {
		test_my_str = json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
		if (!test_my_str) {
			fprintf(stderr, "json_tokener_parse fail\n");
			goto end;
		}
		printf("jobj from str:\n---\n%s\n---\n", test_my_str);
		jobj = json_tokener_parse(test_my_str);
	} else {
	    jobj = json_tokener_parse(buf);
	    if (!jobj) {
			fprintf(stderr, "json_tokener_parse fail\n");
	        goto end;
		}
	}

#if 1
	val_obj = find_target(jobj, key_name);
	if (!val_obj) {
		fprintf(stderr, "Can't find target keyname %s\n", key_name);
		return 1;
	}
#else
	val_obj = find_target(jobj, tname);
	if (!val_obj) {
		fprintf(stderr, "Can't find target keyname %s\n", tname);
		return 1;
	}
#endif

	gstr = json_object_get_string(val_obj);
	if (!gstr) {
		fprintf(stderr, "Can't find target keyname in the file\n");
		return 1;
	}

	strcpy(dist, gstr);

    json_object_put(jobj);
    fprintf(stdout, "dist=%s\n", dist);


/*
	val_obj = find_target(jobj, tname);
	if (!val_obj) {
		fprintf(stderr, "Can't find target keyname %s\n", tname);
		return 1;
	}

	gstr = json_object_get_string(val_obj);
	if (!gstr) {
		fprintf(stderr, "Can't find target keyname in the file\n");
		return 1;
	}

	strcpy(dist, gstr);

    json_object_put(jobj);
    fprintf(stdout, "dist=%s\n", dist);
*/

    fclose(fp);
    return 0;

end:
    json_object_put(jobj);
    fclose(fp);
}

int main()
{
    char dist[16];
    test(dist);
    return 0;
}

