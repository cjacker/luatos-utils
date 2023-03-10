// a tool to generate script.bin for LuatOS,
// script.bin is in luadb format, refer to 
// upstream spec : https://wiki.luatos.com/develop/contribute/luadb.html

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>

struct Node {
    char *filename;
    struct Node *next;
};

typedef struct Node Node;

Node* add_node(Node *head, char *filename) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node->filename = (char*)malloc(strlen(filename) + 1);
    strcpy(new_node->filename, filename);
    new_node->next = head;
    return new_node;
}

void help() {
	printf("luatos-mkscriptbin - a tool to generate script.bin for LuatOS\n");
	printf("Usage : luatos-mkscriptbin <dist dir>\n");
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		help();
		exit(0);
	}

    DIR *dir;
    struct dirent *ent;
	Node *head = NULL;
    char *disk_path = argv[1]; // disk path
    uint16_t file_count = 0;

    dir = opendir(disk_path);
    if (dir == NULL) {
        fprintf(stderr, "Open %s dir failed.", disk_path);
        return 1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) { // if regular file
            //printf("%s\n", ent->d_name);
			head = add_node(head, ent->d_name);
            file_count++;
        }
    }

    //printf("file count : %d\n", file_count);
    closedir(dir);

	if(file_count == 0) {
		fprintf(stderr, "Error : no file in %d dir", disk_path);
		exit(1);
	}

    FILE *fp;
    char filename[] = "script.bin";

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("open target file './script.bin' failed.");
        return 1;
    }
	// magic number
    uint8_t magic[] = {0x01, 0x04, 0x5A, 0xA5, 0X5A, 0xA5};
    int num_magic = sizeof(magic) / sizeof(uint8_t);

	fwrite(magic, sizeof(uint8_t), num_magic, fp);

	// version
	uint8_t version[] = {0x02, 0x02, 0x02, 0x00};
	//uint8_t version[] = {0x02, 0x02, 0x00, 0x02};
	int num_version = sizeof(version) / sizeof(uint8_t);
	fwrite(version, sizeof(uint8_t), num_version, fp);

	// head length, fixed length: 18, aka, 0x12
	uint8_t headlen[] = {0x03, 0x04, 0x12, 0x00, 0x00, 0x00};
	int num_headlen = sizeof(headlen) / sizeof(uint8_t);
	fwrite(headlen, sizeof(uint8_t), num_headlen, fp);	

	// file count, reverse order.
	uint8_t filecount[4];
	filecount[0] = 0x04;
	filecount[1] = 0x02;
	filecount[2] = (uint8_t)file_count;;
	filecount[3] = (uint8_t)(file_count>>8);
	int num_filecount = sizeof(filecount) / sizeof(uint8_t); 	
	fwrite(filecount, sizeof(uint8_t), num_filecount, fp);

	// CRC, not used.
	uint8_t crc[] = {0xFE, 0x02, 0xFF, 0xFF};
	int num_crc = sizeof(crc) / sizeof(uint8_t);
	fwrite(crc, sizeof(uint8_t), num_crc, fp);

 	Node *current = head;
    while (current != NULL) {
    	// magic number
    	uint8_t magic[] = {0x01, 0x04, 0x5A, 0xA5, 0X5A, 0xA5};
    	int num_magic = sizeof(magic) / sizeof(uint8_t);

    	fwrite(magic, sizeof(uint8_t), num_magic, fp);

		// filename length
		uint8_t filenamelen[] = {0x02, strlen(current->filename)};
		int num_filenamelen = sizeof(filenamelen) / sizeof(uint8_t);
    	fwrite(filenamelen, sizeof(uint8_t), num_filenamelen, fp);
		
		// filename
		fwrite(current->filename, sizeof(uint8_t), strlen(current->filename), fp);

		// open file to get file size and contents

		char *filename = malloc(strlen(disk_path) + strlen(current->filename) + 1);
		sprintf(filename, "%s/%s", disk_path, current->filename);
	
    	FILE *file;
    	long size;
	
    	file = fopen(filename, "rb"); 
    	if (file == NULL) {
        	fprintf(stderr, "open file failed : %s\n", filename);
        	return 1;
    	}

    	fseek(file, 0, SEEK_END); // seek to end
    	size = ftell(file); 

		// reset cursor	
		fseek(file, 0, SEEK_SET);
		char *content = malloc(size + 1);
		fread(content, size, 1, file);
		fclose(file);
			
		// file size, reverse order
		uint8_t filesize[6];
		filesize[0] = 0x03;
		filesize[1] = 0x04;
		filesize[2] = (uint8_t)(size);
		filesize[3] = (uint8_t)(size >>8);
		filesize[4] = (uint8_t)(size >>16);
		filesize[5] = (uint8_t)(size >>24);
		fwrite(filesize, sizeof(uint8_t), 6, fp);
	
		// crc, not used.
		uint8_t crc[] = {0xFE, 0x02, 0xFF, 0xFF};
		int num_crc = sizeof(crc) / sizeof(uint8_t);
		fwrite(crc, sizeof(uint8_t), num_crc, fp);

		// file contents
		fwrite(content, sizeof(uint8_t), size, fp);
		free(content);

		// continue to next.	
        current = current->next;
    }

    // free list. 
    current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp->filename);
        free(temp);
    }	

    fclose(fp);
	printf("script.bin generated.\n");
    return 0;
}
