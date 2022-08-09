#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

char *strdup(const char *str)
{
	char *ret = malloc((strlen(str) + 1) * sizeof(char));
	strcpy(ret, str);
	return ret;
}

typedef struct {
	char **arr;
	int count;
} StringArray;
StringArray string_array_create(void)
{
	return (StringArray) {.arr = NULL, .count = 0};
}
void string_array_destroy(StringArray *str_arr)
{
	for (int i = 0; i < str_arr->count; i++) {
		free(str_arr->arr[i]);
	}
	free(str_arr->arr);
}
void string_array_push(StringArray *str_arr, const char *str)
{
	if (str_arr->arr == NULL) {
		str_arr->arr = malloc(1 * sizeof(char *));
	} else {
		str_arr->arr = realloc(str_arr->arr, (str_arr->count + 1) * sizeof(char *));
	}
	str_arr->arr[str_arr->count] = strdup(str);

	str_arr->count++;
}

typedef enum {
	BUILD_TYPE_BINARY,
	BUILD_TYPE_LIBRARY
} BuildType;
typedef enum {
	BUILD_MODE_DEBUG,
	BUILD_MODE_RELEASE
} BuildMode;

typedef struct {
	char *compiler;
	char *standard;
	BuildType type;
	BuildMode mode;
	char *output;
	int search_depth;

	// Directories
	StringArray src_dirs;
	StringArray lib_dirs;
	StringArray include_dirs;
	char *obj_dir;

	StringArray libs;
} BuildConfig;
BuildConfig build_config_create(const char *filepath)
{
	BuildConfig config = {0};
	config.type			= BUILD_TYPE_BINARY;
	config.mode 		= BUILD_MODE_DEBUG;
	config.output		= NULL;
	config.src_dirs		= string_array_create();
	config.lib_dirs 	= string_array_create();
	config.include_dirs	= string_array_create();
	config.obj_dir		= NULL;
	config.libs			= string_array_create();

	// Clean up the file
	FILE *fp = fopen(filepath, "rb");
	char stripped[512];
	int i = 0;
	char c;
	while ((c = fgetc(fp)) != EOF) {
		while (c == '\n' || c == ' ' || c == '\t' || c == '\r') { c = fgetc(fp); }
		stripped[i++] = c;
	}
	fclose(fp);
	stripped[i] = '\0';

	// Parse the values
	typedef enum {
		PARAM_COMPILER,
		PARAM_STANDARD,
		PARAM_TYPE,
		PARAM_MODE,
		PARAM_OUTPUT,
		PARAM_SEARCH_DEPTH,
		PARAM_LIBS,
		PARAM_SRC_DIRS,
		PARAM_OBJ_DIR,
		PARAM_INCLUDE_DIRS,
		PARAM_LIB_DIRS,
	} Parameter;
	Parameter curr_param;
	int buff_i = 0;
	i = 0;
	char buffer[512];
	while (stripped[i] != '\0') {
		// = is the end of a parameter name
		// ; is the end of a value
		// , is a separator
		switch (stripped[i]) {
			case '=':
				buffer[buff_i] = '\0';
				// Check if parameter is valid
				if (strcmp(buffer, "compiler") == 0) {
					curr_param = PARAM_COMPILER;
				} else if (strcmp(buffer, "standard") == 0 || strcmp(buffer, "std") == 0) {
					curr_param = PARAM_STANDARD;
				} else if (strcmp(buffer, "type") == 0) {
					curr_param = PARAM_TYPE;
				} else if (strcmp(buffer, "mode") == 0) {
					curr_param = PARAM_MODE;
				} else if (strcmp(buffer, "output") == 0) {
					curr_param = PARAM_OUTPUT;
				} else if (strcmp(buffer, "search_depth") == 0) {
					curr_param = PARAM_SEARCH_DEPTH;
				} else if (strcmp(buffer, "libs") == 0) {
					curr_param = PARAM_LIBS;
				} else if (strcmp(buffer, "src_dirs") == 0) {
					curr_param = PARAM_SRC_DIRS;
				} else if (strcmp(buffer, "obj_dir") == 0) {
					curr_param = PARAM_OBJ_DIR;
				} else if (strcmp(buffer, "include_dirs") == 0) {
					curr_param = PARAM_INCLUDE_DIRS;
				} else if (strcmp(buffer, "lib_dirs") == 0) {
					curr_param = PARAM_LIB_DIRS;
				} else {
					printf("Error: '%s' is not a valid parameter.\n", buffer);
					exit(1);
				}
				buff_i = 0;
				break;
			case ';':
				buffer[buff_i] = '\0';
				buff_i = 0;
				switch (curr_param) {
					case PARAM_COMPILER:
						config.compiler = strdup(buffer);
						break;
					case PARAM_STANDARD:
						config.standard = strdup(buffer);
						break;
					case PARAM_TYPE:
						if (strcmp(buffer, "bin") == 0) {
							config.type = BUILD_TYPE_BINARY;
						} else if (strcmp(buffer, "lib") == 0) {
							config.type = BUILD_TYPE_LIBRARY;
						} else {
							printf("Error: '%s' is not a valid build type. bin (binary) or lib (library) are the only options available. Make sure everything is lowercase.\n", buffer);
							exit(1);
						}
						break;
					case PARAM_MODE:
						if (strcmp(buffer, "debug") == 0 || strcmp(buffer, "d") == 0) {
							config.mode = BUILD_MODE_DEBUG;
						} else if (strcmp(buffer, "release") == 0 || strcmp(buffer, "r") == 0) {
							config.mode = BUILD_MODE_RELEASE;
						} else {
							printf("Error: '%s' is not a valid build mode. release/r and debug/d are the only valid options. Make sure everything is lowercase.\n", buffer);
							exit(1);
						}
						break;
					case PARAM_OUTPUT:
						config.output = strdup(buffer);
						break;
					case PARAM_SEARCH_DEPTH:
						if (strspn(buffer, "012345789") == strlen(buffer)) {
							config.search_depth = atoi(buffer);
						} else {
							printf("Error: search_depth only accepts posetive integers, '%s' doesn't fulfill those requirements.\n", buffer);
							exit(1);
						}
						break;
					case PARAM_LIBS:
						if (buffer[0] == '\0') { break; }
						string_array_push(&config.libs, buffer);
						break;
					case PARAM_SRC_DIRS:
						string_array_push(&config.src_dirs, buffer);
						break;
					case PARAM_OBJ_DIR:
						config.obj_dir = strdup(buffer);
						break;
					case PARAM_INCLUDE_DIRS:
						if (buffer[0] == '\0') { break; }
						string_array_push(&config.include_dirs, buffer);
						break;
					case PARAM_LIB_DIRS:
						if (buffer[0] == '\0') { break; }
						string_array_push(&config.lib_dirs, buffer);
						break;
				}
				break;
			case ',':
				buffer[buff_i] = '\0';
				buff_i = 0;
				const char *param_str[] = {
					"compiler",
					"standard",
					"type",
					"mode",
					"output",
					"search_depth",
					"libs",
					"src_dirs",
					"obj_dir",
					"include_dirs",
					"lib_dirs",
				};
				switch (curr_param) {
					case PARAM_LIBS:
						if (buffer[0] == '\0') { break; }
						string_array_push(&config.libs, buffer);
						break;
					case PARAM_SRC_DIRS:
						if (buffer[0] == '\0') { break; }
						string_array_push(&config.src_dirs, buffer);
						break;
					case PARAM_INCLUDE_DIRS:
						if (buffer[0] == '\0') { break; }
						string_array_push(&config.include_dirs, buffer);
						break;
					case PARAM_LIB_DIRS:
						if (buffer[0] == '\0') { break; }
						string_array_push(&config.lib_dirs, buffer);
						break;
					default:
						printf("Error: %s is not a list type parameter.\n", param_str[curr_param]);
						exit(1);
				}
				break;
			default:
				buffer[buff_i++] = stripped[i];
				break;
		}
		i++;
	}

	return config;
}
void build_config_destroy(BuildConfig *config)
{
	if (config->output  != NULL) { free(config->output);  }
	if (config->obj_dir != NULL) { free(config->obj_dir); }
	string_array_destroy(&config->src_dirs);
	string_array_destroy(&config->lib_dirs);
	string_array_destroy(&config->include_dirs);
	string_array_destroy(&config->libs);
}

void write_makefile(BuildConfig config, const char *filepath)
{
	FILE *fp = fopen(filepath, "wb");
	if (fp == NULL) {
		printf("Error: Unable to create/write to file '%s'.\n", filepath);
		exit(1);
	}

	// Generate file contents
	char content[1024] = {0};

	fprintf(fp, "CC := %s\n", config.compiler);
	// CFLAGS
	fprintf(fp, "CFLAGS := -std=%s -MP -MD", config.standard);
	if (config.mode == BUILD_MODE_DEBUG) {
		fprintf(fp, " -g -Wextra -pedantic\n");
	} else if (config.mode == BUILD_MODE_RELEASE) {
		fprintf(fp, " -O3\n");
	}
	// LFLAGS
	fprintf(fp, "LFLAGS :=");
	for (int i = 0; i < config.lib_dirs.count; i++) {
		fprintf(fp, " -L%s", config.lib_dirs.arr[i]);
	}
	for (int i = 0; i < config.libs.count; i++) {
		fprintf(fp, " -l%s", config.libs.arr[i]);
	}
	fprintf(fp, "\n");
	// IFLAGS
	fprintf(fp, "IFLAGS :=");
	for (int i = 0; i < config.include_dirs.count; i++) {
		fprintf(fp, " -I%s", config.include_dirs.arr[i]);
	}
	fprintf(fp, "\n");

	// SRC
	fprintf(fp, "SRC := $(foreach dir,");
	for (int i = 0; i < config.src_dirs.count; i++) {
		fprintf(fp, "%s", config.src_dirs.arr[i]);
		if (i != config.src_dirs.count - 1) {
			fprintf(fp, " ");
		}
	}
	fprintf(fp, ",");
	char depth[128] = {0};
	for (int i = 0; i < config.search_depth; i++) {
		fprintf(fp, "$(wildcard $(dir)%s/*.c)", depth);
		strcat(depth, "/**");
		if (i != config.search_depth - 1) {
			fprintf(fp, " ");
		}
	}
	fprintf(fp, ")\n");
	// OBJ
	fprintf(fp, "OBJ := $(addprefix %s/,$(notdir $(SRC:%%.c=%%.o)))\n", config.obj_dir);
	// VPATH
	fprintf(fp, "VPATH := $(dir $(SRC))\n");
	// DEP
	fprintf(fp, "DEP := $(OBJ:%%.o=%%.d)\n");
	fprintf(fp, "-include $(DEP)\n");

	fprintf(fp, "\n");

	fprintf(fp, "");

	fprintf(fp, ".DEFAULT_GOAL := build\n");
	// Output
	fprintf(fp, "build: $(OBJ)\n");
	fprintf(fp, "\t@mkdir -p $(dir %s)\n", config.output);
	if (config.type == BUILD_TYPE_BINARY) {
		fprintf(fp, "\t$(CC) $(CFLAGS) $(OBJ) -o %s $(LFLAGS)\n", config.output);
	} else if (config.type == BUILD_TYPE_LIBRARY) {
		fprintf(fp, "\tar rcs -o $(dir %s)lib$(notdir %s).a $(OBJ)\n", config.output, config.output);
	}
	// Compiling
	fprintf(fp, "%s/%%.o: %%.c\n", config.obj_dir);
	fprintf(fp, "\t@mkdir -p %s\n", config.obj_dir);
	fprintf(fp, "\t$(CC) $(CFLAGS) -c $< -o $@ $(IFLAGS)\n");

	// Phony
	fprintf(fp, ".PHONY: clean\n");
	fprintf(fp, "clean:\n");
	if (config.type == BUILD_TYPE_BINARY) {
		fprintf(fp, "\trm -f $(OBJ) $(DEP) %s\n", config.output);
	} else if ( config.type == BUILD_TYPE_LIBRARY) {
		fprintf(fp, "\trm -f $(OBJ) $(DEP) $(dir %s)lib$(notdir %s).a\n", config.output, config.output);
	}

	fclose(fp);
}

int main(int argc, char *argv[])
{
	char *user_input = NULL;
	char *user_output = NULL;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'i') {
				user_input = strdup(argv[i + 1]);
			}
			if (argv[i][1] == 'o') {
				user_output = strdup(argv[i + 1]);
			}
		}
	}

	const char *default_buildchain = ".buildchain";
	const char *default_output = "Makefile";

	BuildConfig config;
	if (user_input == NULL) {
		config = build_config_create(default_buildchain);
	} else {
		config = build_config_create(user_input);
	}

	if (user_output == NULL) {
		write_makefile(config, default_output);
	} else {
		write_makefile(config, user_output);
	}

	build_config_destroy(&config);
	return 0;
}
