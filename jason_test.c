#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mcheck.h>
#include <sys/stat.h>
#include <json-c/json.h>

#define READ_FILE_BUF 1024
#define ONE_LOAD 15
#define READ_FILE "test.txt"

#define KEY_CHECK_KEY "success"
#define KEY_RESULT_OBJ "result"
#define KEY_FIELDS_OBJ "fields"
#define KEY_RECORD_OBJ "records"

#define KEY_RECORD_KEY_SITE_ID "SiteId"
#define KEY_RECORD_KEY_SITE_NAME "SiteName"
#define KEY_RECORD_KEY_COUNTRY "County"
#define KEY_RECORD_KEY_ITEM_ID "ItemId"
#define KEY_RECORD_KEY_ITEM_NAME "ItemName"
#define KEY_RECORD_KEY_ITEM_ENGNAME "ItemEngName"
#define KEY_RECORD_KEY_ITEM_UNIT "ItemUnit"
#define KEY_RECORD_KEY_MON_DATA "MonitorDate"
#define KEY_RECORD_KEY_CONCENT "Concentration"

long int file_size(FILE *fp)
{
	long size;

	fseek(fp, 0, SEEK_END);

    /*Get the current position of the file pointer.*/
    size = ftell(fp);

    if (size != -1) {
		fseek(fp, 0, SEEK_SET);
        return size;
	}

    printf("open file err\n");
    return size;
}

struct json_object *find_obj(struct json_object *jobj, const char *key)
{
	struct json_object *tmp;

	json_object_object_get_ex(jobj, key, &tmp);
	return tmp;
}

int find_key_str_val(struct json_object *jobj,
	const char *key, char *val_str)
{
	json_object *tmp_obj;
	const char *tmp_str;

	tmp_obj = find_obj(jobj, key);
	if (!tmp_obj) {
		fprintf(stderr, "can't find target key name %s\n", key);
		return -1;
	}

	tmp_str = json_object_get_string(tmp_obj);
	if (!tmp_str) {
		fprintf(stderr, "can't find target keyname in the file\n");
		return -1;
	}

	strcpy(val_str, tmp_str);
	/* fprintf(stdout, "str=%s\n", val_str); */

	return 0;
}


int test(char *dist)
{
    FILE *fp = NULL;
    char *read_jason_buf;
    const char *test_my_str;
    char str_val[30];
	char *val_type_str, *type_str;
    long json_file_sz;
	int val_type, arraylen, i, ret;

	json_object *jobj;
	json_object *result_obj;
	json_object *fields_obj;
	json_object *array_obj;
	json_object *engname_obj, *sitname_obj;

    if ((fp = fopen (READ_FILE, "r")) == NULL) {
        fprintf(stderr, "Can't open file\n");
        return 1;
    }

	json_file_sz = file_size(fp);
	read_jason_buf = malloc(sizeof(char) * json_file_sz);
	fread(read_jason_buf, json_file_sz, 1, fp);
	/* fprintf(stdout, "read_jason_buf=%s\n",buf); */

    jobj = json_tokener_parse(read_jason_buf);
    if (!jobj) {
		test_my_str = json_object_to_json_string_ext(jobj,
			JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
		if (!test_my_str) {
			fprintf(stderr, "json_tokener_parse fail\n");
			goto end;
		}
		printf("jobj from str:\n---\n%s\n---\n", test_my_str);
		jobj = json_tokener_parse(test_my_str);
	} else {
	    jobj = json_tokener_parse(read_jason_buf);
	    if (!jobj) {
			fprintf(stderr, "json_tokener_parse fail\n");
	        goto end;
		}
	}

	/* 
	 * make sure json file correct
	 */
	ret = find_key_str_val(jobj, KEY_CHECK_KEY, str_val);
	if (ret < 0) {
		printf("find_key_str_val fail");
		return 1;
	}

	if (strcmp(str_val, "true") != 0) {
		printf("json file format fail (%s)\n", str_val);
		return 1;
	}
	fprintf(stdout, "str=%s\n", str_val);

	/*
	 * get array value from json file
	 */
	result_obj = find_obj(jobj, KEY_RESULT_OBJ);
	if (!result_obj) {
		fprintf(stderr, "can't find key %s\n", KEY_RESULT_OBJ);
		return 1;
	}

	fields_obj = find_obj(result_obj, KEY_RECORD_OBJ);
	if (!fields_obj) {
		fprintf(stderr, "can't find key %s\n", KEY_RECORD_OBJ);
		return 1;
	}

	arraylen = json_object_array_length(fields_obj);
	printf("arraylen %d\n", arraylen);

	for (i = 0; i < arraylen; i++) {
		array_obj = json_object_array_get_idx(fields_obj, i);

		engname_obj = find_obj(array_obj, KEY_RECORD_KEY_ITEM_ENGNAME);
		sitname_obj = find_obj(array_obj, KEY_RECORD_KEY_SITE_NAME);

		/* print out the name attribute */
		printf("sitname=%s\n", json_object_get_string(sitname_obj));
		printf("engname=%s\n", json_object_get_string(engname_obj));
	}

	/* cloase file */
	free(read_jason_buf);
	json_object_put(jobj);
    fclose(fp);
    return 0;

end:
	free(read_jason_buf);
    json_object_put(jobj);
    fclose(fp);
}

int main()
{
    char dist[16];
    test(dist);
    return 0;
}

