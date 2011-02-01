#include "support.h"
#include "../support/gutils.h"

list_t *open_files;

int retriever_init_common(struct file_system_info *fsinfo){

	int i = 0, j = 0;

	debug(2, "Received %d repos;\n", fsinfo->repo_count);
	file_mutex = calloc(fsinfo->repo_count, sizeof(pthread_mutex_t *));
	for (i = 0; i < fsinfo->repo_count; i++){
		file_mutex[i] = calloc(fsinfo->rev_count[i], sizeof(pthread_mutex_t));
		for (j = 0; j < fsinfo->rev_count[i]; j++)
			pthread_mutex_init(&file_mutex[i][j], 0);
	};
    return (open_files = single(list_t)) != NULL;

};

int retrieve_common(struct file_system_info *fsinfo, struct stats *stats, int repo){

#define retrieve_common_finish(value){								\
			unlock(file_mutex[repo][stats->rev]);			\
			gstrdel(file);									\
			gstrdel(revision);								\
            free(temp);                                     \
			return value;									\
		}

	char *file = NULL;
	char *revision = calloc(20, sizeof(char));
	struct stat *temp = single(struct stat);

	debug(2, "Received file %s from repo %d;\n", stats->path, repo);
	lock(file_mutex[repo][stats->rev]);
    node_t *node = add_file(open_files, stats->path, stats->rev);
	if (node->count > 0){
		node->count++;
		retrieve_common_finish(0);
	};
	if (create_tmp_file(stats, node) == -1)
		retrieve_common_finish(-1);
	if (gmstrcpy(&file, fsinfo->repos[repo], "/", stats->internal, 0) == -1)
		retrieve_common_finish(-1);
	sprintf(revision, "%dB", stats->rev);
	if (retrieve_rdiff(revision, file, node->tmp_path) != 0)
		retrieve_common_finish(-1);
	if (stat(node->tmp_path, temp) != 0)
		retrieve_common_finish(-1);
	node->count = 1;
	retrieve_common_finish(0);

};

int repo_number(struct file_system_info *fsinfo, char *path){

#define repo_number_finish(value) {				\
			gstrdel(repo);						\
			return value;						\
		}

	int i = 0;
	char *repo = NULL;

	debug(3, "Received file %s;\n", path);
	if (fsinfo->repo_count == 1)
		repo_number_finish(0);
	if ((repo = gpthprt(path, 0)) == NULL)
		repo_number_finish(-1);
	for (i = 0; (i < fsinfo->repo_count) && (strcmp(repo, fsinfo->repo_names[i]) != 0); i++);
	if (i == fsinfo->repo_count)
		repo_number_finish(-1);
	repo_number_finish(i);
	
};

int retrieve_rdiff(char *revision, char *file, char *tmp_path){

	int pid = 0;

	debug(2, "Received file %s from revision %s retrieved to path %s\n", file, revision, tmp_path);
    if ((pid = fork()) == -1)
		return -1;
    if (pid == 0){
    	debug(2, "%s %s %s\n", revision, file, tmp_path);
		if (execlp("rdiff-backup", "rdiff-backup", "--force", "-r", revision, file, tmp_path, NULL) == -1)
		    fail(ERR_RDIFF);
	};
    waitpid(pid, 0, 0);
    return 0;
	
};

int create_tmp_file(stats_t *stats, node_t *node){

#define create_tmp_file_error {				\
	    gstrdel(tmp_template);				\
	    if (desc != -1)						\
			close(desc);					\
	    return -1;							\
	}

    char *tmp_template = NULL;
    int desc = -1;

	debug(3, "Received file %s;\n", stats->path);
    if (gmstrcpy(&tmp_template, data_dir, "/", stats->name, "XXXXXX", 0) != 0)
		create_tmp_file_error;
    desc = mkstemp(tmp_template);
    if (desc == -1)
		create_tmp_file_error;
    close(desc);
    if (gstrcpy(&node->tmp_path, tmp_template) != 0)
		create_tmp_file_error;
    gstrdel(tmp_template);
    return 0;

};

node_t * add_file(list_t *list, char *path, int rev){
    if (list->head == NULL){
        list->head = list->tail = single(node_t);
        gstrcpy(&list->head->path, path);
        return list->head;
    }
    node_t *node = get_open_file(path);
    if (node)
        return node;
    node = single(node_t);
    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
    gstrcpy(&node->path, path);
    node->rev = rev;
    return node;
};

node_t * get_open_file(char *path){
    node_t *node = open_files->head;
    for (; node && strcmp(node->path, path) != 0; node = node->next);
    return node;
};

void delete_open_file(node_t *node){
    if (node->next)
        node->next->prev = node->prev;
    else
        open_files->tail = node->prev;
    if (node->prev)
        node->prev->next = node->next;
    else
        open_files->head = node->next;
    free(node->path);
    free(node->tmp_path);
    free(node);
};
