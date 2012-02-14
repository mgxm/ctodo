#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <limits.h>

#ifndef TODO_FILE
#define TODO_FILE "todo.list"
#endif

#ifndef TODO_DIR
#define TODO_DIR "/.ctodo/"
#endif

#ifndef TMPFILE
#define TMPFILE "tmptodofile"
#endif

void initialize();
void generate_paths();
void create_todo_file();
void banner_help();
void new_task(char const *task);
void complete_task(char const *tasknumber);
void list_tasks();
int have_todo_file();

char userpath[PATH_MAX + strlen(TODO_DIR)];
char fullpath[PATH_MAX + strlen(TODO_DIR) + strlen(TODO_FILE)];

int main(int argc, char const *argv[])
{
	initialize();

	if(argc<=1) {
		banner_help();
       	return 0;
    }  

	if (strcmp(argv[1], "--help") == 0) {
		banner_help();
	} else if (strcmp(argv[1], "complete") == 0) {
		complete_task(argv[2]);
	} else if (strcmp(argv[1], "new") == 0) {
		new_task(argv[2]);
	} else if (strcmp(argv[1], "list") == 0) {
		list_tasks();
	} else {
		banner_help();
	}

	return 0;
}

void initialize()
{
	generate_paths();
	if(have_todo_file() == 1)
		create_todo_file();

}

void generate_paths()
{
	struct passwd *pwd;
	/* get path of the user */
	pwd = getpwuid(getuid());

	strcpy(userpath, pwd->pw_dir);
	strcat(userpath, TODO_DIR);
	strcpy(fullpath, userpath);
	strcat(fullpath, TODO_FILE);
}

void create_todo_file()
{
	FILE *fp;

	mkdir(userpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if ((fp = fopen(fullpath, "a+")) == NULL) {
		printf("Could not create the ctodo file..\n");
		exit(1);
	}

	fclose(fp);
}

int have_todo_file()
{
	int ret;

	if(access(fullpath, F_OK) == 0) {
		ret = 0;
	}
	else {
		ret = 1;
	}
	return ret;
}

void new_task(char const *task)
{
	FILE *fp;

	if ((fp = fopen(fullpath, "r+")) == NULL) {
		printf("Could open the ctodo file..\n");
		exit(1);
	} else {
		fseek(fp, 0, SEEK_END);
		fprintf(fp, "%s\n", task);
		fseek(fp, 0, SEEK_END);
		fclose(fp);
	}
}

void complete_task(char const *tasknumber)
{
	FILE *fp, *ft;
	int ch, n;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;

	if ((fp = fopen(fullpath, "r+")) == NULL) {
		printf("Could open the ctodo file..\n");
		exit(1);
	} else {

		mkstemp(TMPFILE);
		ft = fopen(TMPFILE, "r+");

        /* remove line */
		while ((read = getline(&line, &len, fp)) != -1) {
			n++;
			if(atoi(tasknumber) != n) {
				fputs(line, ft);
			}
        }

        /*copy to original */
        remove(fullpath);
        rewind(ft);

        if ((fp = fopen(fullpath, "a+")) == NULL) {
        	printf("Could open the ctodo file..\n");
			exit(1);        	
        } else {
        	/* copy to tmp */
			while ((ch = fgetc(ft)) != EOF) {
				putc(ch, fp);
        	}
        }

        remove("tmptodofile");

        fclose(ft);
		fclose(fp);
	}
}

void list_tasks()
{
	FILE *fp;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;

	if ((fp = fopen(fullpath, "r+")) == NULL) {
		printf("Could open the ctodo file..\n");
		exit(1);
	} else {
		while ((read = getline(&line, &len, fp)) != -1) {
				printf("%s", line);
		}
	}
}

void banner_help()
{
	char ctodo_usage_string[] =
	"USAGE: ctodo [<args>]\nEXAMPLE: ctodo \"Make me a sanduiche\"";

	printf("\n%s\n\n", ctodo_usage_string);
}