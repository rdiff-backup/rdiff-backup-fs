#include "initialize.h"

// private:

void check_mount(){
    
    if (gpthpro(&mount) != 0)
    	fail(ERR_NO_MOUNT);
            
};

void check_repo(int index){

    char *rdiff_backup_dir = NULL;

#ifdef DEBUG
	printf("[Function: check_repo] Checking repo %s;\n", repos[index]);
#endif
	if (gmstrcpy(&rdiff_backup_dir, repos[index], "/rdiff-backup-data", 0) == -1)
		fail(-1);
	if (gpthpro(&rdiff_backup_dir) != 0)
		fail(ERR_NO_REPO);
	gstrdel(rdiff_backup_dir);
	if (gpthpro(&repos[index]) != 0)
		fail(ERR_NO_REPO);
#ifdef DEBUG
	printf("[Function: check_repo] Setting repo to %s;\n", repos[index]);
#endif
	
};

void check_repos(){
    
    int i = 0;
    
    repo_names = calloc(repo_count, sizeof(char *));
    for (i = 0; i < repo_count; i++)
		if (gpthcld(&repo_names[i], repos[i]) == -1)	
			fail(-1);
    for (i = 0; i < repo_count; i++)
		check_repo(i);
    
};

void layout_setup(){

	if (layout == LAYOUT_ALL){
		init = &all_init;
		init_multi = &all_init_multi;
		get_file = &all_get_file;
		get_children = &all_get_children;
	}
	else if (layout == LAYOUT_LAST){
		init = &versions_init;
		init_multi = &versions_init_multi;
		get_file = &versions_get_file;
		get_children = &versions_get_children;
	}
	else
		fail(-1);

};

void data_structure_setup(){

	if (structure == STRUCTURE_FULL){
		struct_build = &full_build;
		struct_build_multi = &full_build_multi;
		struct_get_children = &full_get_children;
		struct_get_file = &full_get_file;
	}
	else if (structure == STRUCTURE_NECESSARY){
		struct_build = &necessary_build;
		struct_build_multi = &necessary_build_multi;
		struct_get_children = &necessary_get_children;
		struct_get_file = &necessary_get_file;
	}
	else
		fail(-1);

};

void fuse_operations_setup(){

	operations.getattr 	= &revs_getattr;
	operations.readdir 	= &revs_readdir;
	operations.readlink = &revs_readlink;
	operations.open	   	= &revs_open;
	operations.read    	= &revs_read;
	operations.release 	= &revs_release;
	operations.destroy 	= &revs_destroy;

};

void create_tmp_dir(char **tmp_dir, char **tmp_file){
    
    char *tmpdirprefix = getenv("TMPDIR");
    if (!tmpdirprefix) 
		tmpdirprefix = DEFAULT_TMP_DIR;
    if (*tmp_dir == NULL)
    	if (gstrcpy(tmp_dir, tmpdirprefix) == -1)
    		fail(-1);
    if (gpthpro(tmp_dir) != 0)
    	fail(ERR_NO_TMP);
    int length = strlen(*tmp_dir) + strlen(TMP_DIR_NAME);
    char tmp_template[length + 1];

    strcpy(tmp_template, *tmp_dir);
   	strcpy(tmp_template + strlen(*tmp_dir), TMP_DIR_NAME);
    if (mkdtemp(tmp_template) == NULL)
		fail(-1);
    if (((*tmp_file) = gstralloc(length)) == NULL)
    	fail(-1);
    strcpy((*tmp_file), tmp_template);
#ifdef DEBUG
    printf("[Function: create_tmp_dir] Created directory %s;\n", *tmp_file);
#endif
        
};

// public:

void initialize(){

    check_mount();
    check_repos();
   	create_tmp_dir(&tmp_dir, &tmp_file);

    data_structure_setup();
	layout_setup();
    fuse_operations_setup();

    if (repo_count == 1)
		init(repos[0]);
    else if (repo_count > 1)
		init_multi(repo_count, repos);
    else
		fail(ERR_NO_REPO);

    if (cache_limit == 0)
   		retriever_init_simple(repo_count, rev_count);
   	else
   		retriever_init_limit(repo_count, rev_count);
   	
};
