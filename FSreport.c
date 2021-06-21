#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <math.h>

/**
 * Student Name: Rehan Nagoor Mohideen
 * Student ID: 110592
 */

struct otherNode {
    char *string;
    char *name;
    int inode;
    int file;
    struct otherNode *next;
};

struct queueNode {
    int level;
    char *path;
    char *name;
    struct queueNode *next;
    struct otherNode *ohead;
};

/**
 * Function to insert a queueNode struct to the list/queue
 * @returns the head pointer of the updated queue
 */
struct queueNode *insertqHead(struct queueNode *qhead, struct queueNode *qNode) {
    struct queueNode *prevNode = qhead;
    struct queueNode *nextNode = qhead;
    int i = 0;

    //finding the position to insert
    while ( (nextNode != NULL && nextNode->level<qNode->level) || (nextNode != NULL && nextNode->level == qNode->level && strcmp(qNode->name, nextNode->name)>0) ) {
        if (i!=0) {
            prevNode = prevNode->next;
        }
        nextNode = nextNode->next;
        i++;
    }

    if (prevNode != NULL && i!=0) {//if its not the head node
        prevNode->next = qNode;
    } else {//it its the head node
        qhead = qNode;
    }
    qNode->next = nextNode;

    return qhead;
}

/**
 * Function to insert a otherNode struct to the list of other nodes in for each queue Node in the print order
 * @returns the head pointer of the updated list
 */
struct otherNode *insertoHead(struct queueNode *qhead, struct otherNode *ohead, struct otherNode *oNode, int isTreeStruct) {
    struct otherNode *prevNode = ohead;
    struct otherNode *nextNode = ohead;
    int i = 0;

    if (isTreeStruct) {
        if (oNode->file == 1) {//if its a file
            //finding the position to insert for a file. files are after all directories and in ascending order
            while ( (nextNode != NULL && nextNode->file==0) || (nextNode != NULL && strcmp(oNode->name, nextNode->name)>0)) {
                if (i!=0) {
                    prevNode = prevNode->next;
                }
                nextNode = nextNode->next;
                i++;
            }
        } else {//if its a directory
            //finding the position to insert for a directory. directories are before files and in ascending order
            while ( nextNode != NULL && nextNode->file==0 && strcmp(oNode->name, nextNode->name)>0 ) {
                if (i!=0) {
                    prevNode = prevNode->next;
                }
                nextNode = nextNode->next;
                i++;
            }
        }
    } else {
        while (nextNode != NULL && oNode->inode > nextNode->inode) {
            if (i!=0) {
                prevNode = prevNode->next;
            }
            nextNode = nextNode->next;
            i++;
        }
    }

    // struct otherNode *temp = ohead;
    // while (temp !=NULL) {
    //     printf("testing %s\n", temp->name);
    //     temp = temp->next;
    // }
    if (prevNode != NULL && i!=0) {//if its not a head node
        prevNode->next = oNode;
    } else {//its a head node so add it to qhead->ohead
        qhead->ohead = oNode;
        ohead = oNode;
    }
    oNode->next = nextNode;

    return ohead;
}

/**
 * Function used to create and link the structure for all the files/directories for each path
 * @returns the pointer to the head of the updated queue
 */
struct queueNode *linkStruct (struct queueNode *qhead, char *curPath, int isTreeStruct) {
    struct dirent *dirEntry;
    struct stat fileStat;
    DIR *directory = opendir(curPath);
    if (directory == NULL) {
        printf("Could not open PATH directory\n" );
        return qhead;
    }
    struct otherNode *ohead = NULL;

    while ((dirEntry = readdir(directory)) != NULL){//for each file in the directory
        char file[1000];
        strcpy(file, curPath);
        strcat(file, "/");
        strcat(file, dirEntry->d_name);
        // printf("loop %s\n", file);


        if (stat(file, &fileStat) == 0) {//if successfully obtaines the stats
            time_t t;
            if (fileStat.st_mtim.tv_sec>fileStat.st_ctim.tv_sec) {//get either the last modified or ,based on most recent
                t = fileStat.st_mtim.tv_sec;
            }else {
                t =  fileStat.st_ctim.tv_sec;
            }

            if (S_ISDIR(fileStat.st_mode) && strcmp(dirEntry->d_name, ".")!=0 && strcmp(dirEntry->d_name, "..")!=0) {//if its a directory
                //make the queue node structure and add it to the queue
                struct queueNode *qNode = malloc(sizeof(struct queueNode));
                if (qhead == NULL) {
                    qNode->level = 1;
                } else {
                    qNode->level = qhead->level+1;
                }
                qNode->next = NULL;
                qNode->path = malloc(strlen(file)+5);
                strcpy(qNode->path, file);
                qNode->name = malloc(strlen(dirEntry->d_name)+5);
                strcpy(qNode->name, dirEntry->d_name);
                qhead = insertqHead(qhead, qNode);//add it to the queue based on level and alphabetically.

                //make the other list structure an add it to the other list
                struct otherNode *tempNode = malloc(sizeof(struct otherNode));
                tempNode->file = 0;

                char inode[10];
                sprintf(inode, "%d", fileStat.st_ino);
                tempNode->inode = fileStat.st_ino;
                char size[50];
                sprintf(size, "%d", fileStat.st_size);

                if (isTreeStruct) {
                    char dola[50];
                    sprintf(dola, "%s", ctime(&fileStat.st_atim.tv_sec));
                    dola[strlen(dola)-1] = '\0';
                    char tStr[50];
                    sprintf(tStr, "%s", ctime(&t));
                    tStr[strlen(tStr)-1] = '\0';
                    char permissions[11];
                    strcpy(permissions, "-");
                    (fileStat.st_mode & S_IRUSR) ? strcat(permissions, "r") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IWUSR) ? strcat(permissions, "w") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IXUSR) ? strcat(permissions, "x") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IRGRP) ? strcat(permissions, "r") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IWGRP) ? strcat(permissions, "w") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IXGRP) ? strcat(permissions, "x") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IROTH) ? strcat(permissions, "r") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IWOTH) ? strcat(permissions, "w") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IXOTH) ? strcat(permissions, "x") : strcat(permissions, "-");

                    //adding the output string based on format type
                    tempNode->string = malloc(30 + strlen(getgrgid(fileStat.st_gid)->gr_name) + strlen(inode) + 10 + strlen(size) + strlen(dirEntry->d_name) + strlen(dola) + strlen(tStr));
                    sprintf(tempNode->string, "%s\t%s\t%s\t%s\t%s\n\t%s\t%s", getgrgid(fileStat.st_gid)->gr_name, inode, permissions, size, dirEntry->d_name, dola, tStr);
                } else {
                    char sizeBy512[50];
                    sprintf(sizeBy512, "%d", (int)ceil(((double)fileStat.st_size)/512.0));
                    char noBlocks[50];
                    sprintf(noBlocks, "%d", fileStat.st_blocks);
                    //adding the output string based on format type
                    tempNode->string = malloc(10 + strlen(inode) + strlen(size) + strlen(noBlocks) + strlen(sizeBy512) + strlen(dirEntry->d_name));
                    sprintf(tempNode->string, "%s:\t%s\t%s\t%s\t%s", inode, size, noBlocks, sizeBy512, dirEntry->d_name);
                }
                
                //adding the file name
                tempNode->name = malloc(strlen(dirEntry->d_name)+10);
                strcpy(tempNode->name, dirEntry->d_name);

                ohead = insertoHead(qhead, ohead, tempNode, isTreeStruct);//add it to the appropriate place in the othe nodes list

            } else if (S_ISREG(fileStat.st_mode)) {//if its a file
                //make the structure and add to the other list
                struct otherNode *tempNode = malloc(sizeof(struct otherNode));
                tempNode->file = 1;

                char inode[10];
                sprintf(inode, "%d", fileStat.st_ino);
                tempNode->inode = fileStat.st_ino;
                char size[50];
                sprintf(size, "%d", fileStat.st_size);

                if (isTreeStruct) {
                    char dola[50];
                    sprintf(dola, "%s", ctime(&fileStat.st_atim.tv_sec));
                    dola[strlen(dola)-1] = '\0';
                    char tStr[50];
                    sprintf(tStr, "%s", ctime(&t));
                    tStr[strlen(tStr)-1] = '\0';
                    char permissions[11];
                    strcpy(permissions, "-");
                    (fileStat.st_mode & S_IRUSR) ? strcat(permissions, "r") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IWUSR) ? strcat(permissions, "w") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IXUSR) ? strcat(permissions, "x") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IRGRP) ? strcat(permissions, "r") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IWGRP) ? strcat(permissions, "w") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IXGRP) ? strcat(permissions, "x") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IROTH) ? strcat(permissions, "r") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IWOTH) ? strcat(permissions, "w") : strcat(permissions, "-");
                    (fileStat.st_mode & S_IXOTH) ? strcat(permissions, "x") : strcat(permissions, "-");

                    //adding the output string based on format type
                    tempNode->string = malloc(30 + strlen(getgrgid(fileStat.st_gid)->gr_name) + strlen(inode) + 10 + strlen(size) + strlen(dirEntry->d_name) + strlen(dola) + strlen(tStr));
                    sprintf(tempNode->string, "%s\t%s\t%s\t%s\t%s\n\t%s\t%s", getgrgid(fileStat.st_gid)->gr_name, inode, permissions, size, dirEntry->d_name, dola, tStr);
                } else {
                    char sizeBy512[50];
                    sprintf(sizeBy512, "%d", (int)ceil(((double)fileStat.st_size)/512.0));
                    char noBlocks[50];
                    sprintf(noBlocks, "%d", fileStat.st_blocks);
                    
                    tempNode->string = malloc(10 + strlen(inode) + strlen(size) + strlen(noBlocks) + strlen(sizeBy512) + strlen(dirEntry->d_name));
                    sprintf(tempNode->string, "%s:\t%s\t%s\t%s\t%s", inode, size, noBlocks, sizeBy512, dirEntry->d_name);
                }
                
                
                //adding the file name
                tempNode->name = malloc(strlen(dirEntry->d_name)+10);
                strcpy(tempNode->name, dirEntry->d_name);

                ohead = insertoHead(qhead, ohead, tempNode, isTreeStruct);//add it to the appropriate place in the othe nodes list
            }
        }
    }
    closedir(directory);
    return qhead;
}

/**
 * Function prints out the report based on the report type
 */
void printQHeadNode (struct queueNode *qhead, int isTreeStruct) {
    //print headings
    if (isTreeStruct) {
        if (qhead->level == 1) {
            printf("Level %d: %s\n", qhead->level, qhead->path);
        } else {
            printf("Level %d: %s\n", qhead->level, qhead->name);
        }
    } else {
        if (qhead->level == 1) {
            printf("Level %d Inodes: %s\n", qhead->level, qhead->path);
        } else {
            printf("Level %d Inodes: %s\n", qhead->level, qhead->name);
        }
    }
    
    int dCheck = 0;
    int fCheck = 0;
    struct otherNode *oNode = qhead->ohead;
    //print the file/directory lines
    while (oNode != NULL) {
        if (isTreeStruct && oNode->file == 0 && dCheck == 0) {
            printf("Directories\n");
            dCheck = 1;
        }
        if (isTreeStruct && oNode->file == 1 && fCheck == 0) {
            if (dCheck == 1) {
                printf("\n");
            }
            printf("Files\n");
            fCheck = 1;
        }
        printf("%s\n", oNode->string);
        oNode = oNode->next;        
    }
    printf("\n");
}

/**
 * deletes/frees the node head node of the queue only
 * @returns the updated queue
 */
struct queueNode *deleteQHeadNode(struct queueNode *qhead) {
    if (qhead == NULL) {
        return NULL;
    }
    
    struct otherNode *oNode = qhead->ohead;
    struct otherNode *prevNode;
    
    while (oNode != NULL) {
        free(oNode->name);
        free(oNode->string);
        prevNode = oNode;
        oNode = oNode->next;
        free(prevNode);
    }
    
    struct queueNode *prevqHead = qhead;
    qhead = qhead->next;
    free(prevqHead->name);
    free(prevqHead->path);
    free(prevqHead);
    return qhead;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: [-tree or -inode] path-name\n");
        return 0;
    }

    int reportType;
    if (strcmp(argv[1], "-tree")==0) {
        reportType = 1;
        printf("File System Report: Tree Directory Structure\n\n");
    } else if (strcmp(argv[1], "-inode")==0) {
        printf("File System Report: Inodes\n\n");
        reportType = 0;
    } else {
        printf("Usage: [-tree or -inode] path-name\n");
        return 0;
    }
    
    
    DIR *directory = opendir(argv[2]);
    if (directory == NULL) {
        printf("Could not open PATH directory\n" );
        return 0;
    }

    // int i = 1;
    // printf("File System Report: Tree Directory Structure\n\n");
    // if (i == 1) {
    //     printf("level %d, %s\n", i, argv[2]);
    // }

    struct queueNode *qhead = malloc(sizeof(struct queueNode));
    qhead->level=1;
    qhead->name = NULL;
    qhead->next = NULL;
    qhead->ohead = NULL;
    qhead->path = malloc(strlen(argv[2])+5);
    strcpy(qhead->path, argv[2]);

    while (qhead != NULL) {//while there are directories to got through
        qhead = linkStruct(qhead, qhead->path, reportType);
        printQHeadNode(qhead, reportType);
        qhead = deleteQHeadNode(qhead);
    }

    closedir(directory);

    return 0;
}
