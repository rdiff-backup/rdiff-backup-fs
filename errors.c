#include "headers.h"
#include "errors.h"
#include "externs.h"

void fail(int error){

    printf("Error: ");
    switch (error){
		case (ERR_PARAMETRES):
	    	printf("Wrong parametres; try " PROGRAM_NAME " <mount point path> <repository path> or see man " PROGRAM_NAME ";\n");
	    	break;
		case (ERR_NO_MOUNT_OPT):
		    printf("Mount point was not declared;\n");
		    break;
		case (ERR_NO_REPO_OPT):
	    	printf("No repository was given\n");
	    	break;
		case (ERR_UNKNOWN_OPTION):
		    printf("No such option; see man " PROGRAM_NAME "\n");
		    break;
		case (ERR_NO_MOUNT):
	    	printf("No such mount directory\n");
		    break;
		case (ERR_NO_REPO):
	    	printf("No such repository\n");
	    	break;
		case (ERR_NO_TMP):
		    printf("No such directory for temporary files directory;\n");
	    	break;
		case (ERR_RDIFF):
		    printf("Rdiff failed to recover file;\n");
	    	break;
	    case (ERR_FULL_ONLY):
	    	printf("Disk-only filesystem cannot be used, probably because of chosen layout;\n");
	    	break;
        case (ERR_REPO_READ):
            printf("Failed to read repository;\n");
            break;
		default:
		    printf("Unknown error\n");
	    	break;
    }
    clean_up();
    exit(-1);
    
};

void clean_up(){

    if (data_dir != NULL)
    	if (execlp("rm", "rm", "-r", "-f", data_dir, NULL) == -1)
    	    fail(-1);

}
