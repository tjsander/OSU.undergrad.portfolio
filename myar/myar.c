/*
# Travis J. Sanders
# sandetra@onid.oregonstate.edu
# CS344-400
# Homework #3

-q "Quickly" append named files
-x Extract named members
-t Print concise table of contents “ar t”
-v Print verbose table of contents “ar tv”
-d Delete named files from archive
-A Quickly append all "regular" files in current dir

Extra credit:
-w For a given timeout (seconds) add MODIFIED files to archive.

Latham Fell		Jul 2, 2014+1 		Google+
Here, I'll save you untold misery:
-When read() pulls chars from a file into a buffer string, 
it does not add a null char on the end. You need to do that yourself.﻿

Made extensive use of http://www.cplusplus.com/reference/ctime/
for C documentation

http://linux.die.net/man/3/open
*/

#define _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <utime.h>

#include <dirent.h> 	//-A

#include <getopt.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/stat.h>

#include <ar.h>

#define BLOCKSIZE 1

int simple_step(int in_fd, char* filename, int skip);
void complex_step(int in_fd);
int is_ar(int in_fd);
void simpler_step(int in_fd);
void ar_extract(int in_fd, char** argv, int argc);

/**************************
	Appends filein to in_fd.
	in_fd should be validated as an ar beforehand.
	Takes:		Open file reference, name of file to add
	Returns:	0 if successful.
**************************/
int ar_add(int in_fd, char* filein)
{
	int i;
	struct stat sb;
	struct ar_hdr *header = (struct ar_hdr *) malloc(sizeof(struct ar_hdr));
	if (stat(filein, &sb) == 0) {
		int in2_fd;
		in2_fd = open(filein, O_RDONLY);			//7-file_reverse.mp4
		if (in2_fd == -1) {
			perror("Error");
			exit(EXIT_FAILURE);
		}

		snprintf(header->ar_name, 16, "%s/", filein);
		snprintf(header->ar_date, 12, "%d", (int) sb.st_mtime);
		snprintf(header->ar_uid, 6, "%d", (int) sb.st_uid);
		snprintf(header->ar_gid, 6, "%d", (int) sb.st_gid);
		snprintf(header->ar_mode, 8, "%lo", (unsigned long) sb.st_mode);
		snprintf(header->ar_size, 10, "%d", (int) sb.st_size);
		strcpy(header->ar_fmag, ARFMAG);

		char* header_str;
		header_str = malloc(60*sizeof(char));
		strcat(header_str, header->ar_name);
		for (i=strlen(header->ar_name); i<16; i++) strcat(header_str, " ");
		strcat(header_str, header->ar_date);
		for (i=strlen(header->ar_date); i<12; i++) strcat(header_str, " ");
		strcat(header_str, header->ar_uid);
		for (i=strlen(header->ar_uid); i<6; i++) strcat(header_str, " ");
		strcat(header_str, header->ar_gid);
		for (i=strlen(header->ar_gid); i<6; i++) strcat(header_str, " ");
		strcat(header_str, header->ar_mode);
		for (i=strlen(header->ar_mode); i<8; i++) strcat(header_str, " ");
		strcat(header_str, header->ar_size);
		for (i=strlen(header->ar_size); i<10; i++) strcat(header_str, " ");
		strcat(header_str, header->ar_fmag);

		// printf("%s", header_str);
		write(in_fd, header_str, 60);
		char buff[BLOCKSIZE];
		i = 0;
		while (read(in2_fd, buff, BLOCKSIZE) == 1) {
			write(in_fd, buff, 1);
			i++;
		}
		if (i % 2 == 1) write(in_fd, "\n", 1);
		free(header_str);
		if (close(in2_fd) != 0) printf("Close failed!\n");
	} else {
		printf("file '%s' not found\n", filein);
		return 1;
		// exit(EXIT_FAILURE); //DESIGN: Keep going even if one file has problems?
	}
	free(header);
	return 0;
}

/**************************
	Replicates ar -xo functionality
	Takes:	file reference, argument array, number of args
	Pre:	in_fd must be a proper myar.
			in_fd must point to the first character of a file header
	Returns:	1 if named file is in archive.
				0 if not.
			If 1: Advances pointer to the beginning of the 
			header for the file found.
			If 0: Pointer will land at end of file.
**************************/
void ar_extract(int in_fd, char** argv, int argc)
{
	if (!is_ar(in_fd)) {
		fprintf(stderr, "myar: %s: File format not recognized\n", optarg);
		exit(EXIT_FAILURE);
	}
	char filename[16];
	int idx, id2;
	while ((idx = simple_step(in_fd, filename, 0)) != -1) {
		// printf("idx==%d\n", idx);
		int i=3;
		// idx += 58;
		id2 = idx;
		while (i < argc) {
			if (strcmp(filename, argv[i]) == 0) {
				char buff[1];
				// printf("***EXTRACTING [%s] from the archive!!***\n", filename);
				argv[i] = "";
				i = argc;
				//3.6.4. The -x and -d commands operate on the first file name 
				lseek(in_fd, 16, 1);		//skip filename
				struct utimbuf* oldtime = malloc(sizeof(struct utimbuf));
				char newbuff[10];
				read(in_fd, newbuff, 10);
				oldtime->actime = (time_t) atoi(newbuff);
				oldtime->modtime = oldtime->actime;
				lseek(in_fd, 17, 1);		//skip to perms
				char permsbuff[3];
				read(in_fd, permsbuff, 3);
				lseek(in_fd, 14, SEEK_CUR);
				idx -= 60;
				int out_fd = open(filename, O_CREAT | O_WRONLY, 0777);	//7-file_reverse.mp4
				fchmod(out_fd, strtol(permsbuff, (char **)NULL, 8));	//Set permissions
				while (idx > 0) {
					read(in_fd, buff, 1);
					// printf("%c", buff[0]);
					write(out_fd, buff, 1);
					idx--;
				}
				if (close(out_fd) != 0) printf("Close failed!\n");
				utime(filename, oldtime);
				free(oldtime);
			} 
			i++;
		}
		lseek(in_fd, (id2 % 2) + idx, SEEK_CUR); // IDX should be 0 if file was found
	}
}

/**************************
	Replicates ar -xo functionality
	Exctracts all files from a given archive
	Preserves file modification date.
	Takes:	Open file reference
**************************/
void ar_extract_all(int in_fd)
{
	if (!is_ar(in_fd)) {
		fprintf(stderr, "myar: %s: File format not recognized\n", optarg);
		exit(EXIT_FAILURE);
	}
	char filename[16];
	int idx, id2;
	while ((idx = simple_step(in_fd, filename, 0)) != -1) {
		// printf("idx==%d\n", idx);
		int i=3;
		// idx += 58;
		id2 = idx;
		char buff[1];
		// printf("EXTRACTING [%s] from the archive!!\n", filename);
		lseek(in_fd, 16, 1);		//skip filename
		struct utimbuf* oldtime = malloc(sizeof(struct utimbuf));
		char newbuff[10];
		read(in_fd, newbuff, 10);
		oldtime->actime = (time_t) atoi(newbuff);
		oldtime->modtime = oldtime->actime;
		lseek(in_fd, 16, 1);		//skip to perms

		char permsbuff[4];
		read(in_fd, permsbuff, 4);
		lseek(in_fd, 14, SEEK_CUR);
		idx -= 60;
		// printf("PERMSBUFF= %d\n", atoi(permsbuff));

		int out_fd = open(filename, O_CREAT | O_WRONLY, 0777);	//7-file_reverse.mp4
		fchmod(out_fd, strtol(permsbuff, (char **)NULL, 8));

		while (idx > 0) {
			read(in_fd, buff, 1);
			// printf("%c", buff[0]);
			write(out_fd, buff, 1);
			idx--;
		}
		if (close(out_fd) != 0) printf("Close failed!\n");
		utime(filename, oldtime);
		free(oldtime);
		i++;
		lseek(in_fd, (id2 % 2) + idx, SEEK_CUR); // IDX should be 0 if file was found
	}
}

/**************************
	Replicates ar d functionality
	Deletes named files from the archive.
	Takes:	Open file reference, command line references argv and argc
**************************/
void ar_delete(int in_fd, char** argv, int argc)
{
	if (!is_ar(in_fd)) {
		fprintf(stderr, "myar: %s: File format not recognized\n", optarg);
		exit(EXIT_FAILURE);
	}
	if (unlink(argv[2]) != 0) exit(EXIT_FAILURE);

	int out_fd = open(argv[2], O_CREAT | O_WRONLY, 0666);	//7-file_reverse.mp4
	if (out_fd == -1) exit(EXIT_FAILURE);
	write(out_fd, "!<arch>\n", 8);
	// printf("EXPECTED OUTPUT:\n!<arch>");
	char filename[16];
	int idx;
	while ((idx = simple_step(in_fd, filename, 0)) != -1) {
		idx += idx % 2;
		int i=3;
		while (i < argc) {
			if (strcmp(filename, argv[i]) == 0) {
				// printf("DELETING [%s] from the archive!!\n", filename);
				argv[i] = "";
				i = argc;
				lseek(in_fd, idx, SEEK_CUR); // IDX should be 0 if file was found
				idx = 0;
			}
			i++;
		}
		char buff[BLOCKSIZE];

		while (idx > 0) {
			read(in_fd, buff, BLOCKSIZE);
			// printf("%c", buff[0]);
			write(out_fd, buff, BLOCKSIZE);
			idx--;
		}
	}
	close(out_fd);
}

/**************************
	Boolean helper function for -t and -v
	Takes:		An int reference to open() file.
	Returns:	1 if file is a valid archive. 
				0 if not.
		Advances pointer to the end of ARMAG magic string
**************************/
int is_ar(int in_fd)
{
	char buffer[9];
	read(in_fd, buffer, 8);
	buffer[8] = '\0';
	if(strcmp(ARMAG, buffer) == 0) return 1;
	else {
		return 0;
	}
}

/**************************
	Following code copied from TLPI p.296
	Return ls(1)-style string for file permissions mask 
**************************/
#define STR_SIZE sizeof("rwxrwxrwx")
#define FP_SPECIAL 1
char * filePermStr(mode_t perm, int flags)
{
	static char str[STR_SIZE];
	snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
	(perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-', 
	(perm & S_IXUSR) ?
		(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
		(((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'), 
	(perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-', 
	(perm & S_IXGRP) ?
		(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
		(((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'), 
	(perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-', 
	(perm & S_IXOTH) ?
		(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') : 
		(((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));
	return str; 
}

/**************************
	Helper function for -t
	Prints simple filenames for a given open ar file.
**************************/
void simpler_step(int in_fd)
{
	char printme[16];
	if(is_ar(in_fd)) {
		while (simple_step(in_fd, printme, 1) != -1) {
			printf("%s\n", printme);
		}
	} else {
		printf("myar: %s: File format not recognized\n", optarg);
		return;
	}
}

/**************************
	Helper function for -t
	Steps through one file in in_fd ar file. Sets "filename"
	If skip is passed as 1, in_fd will be advanced to the beginning
		of the next file header.
	Returns:	0 if successful. 
				-1 if at End of File.
**************************/
int simple_step(int in_fd, char* filename, int skip)
{
	char file_buff[16];
	char size_buff[10];
	char buff[BLOCKSIZE];
	int i;
	if (read(in_fd, buff, BLOCKSIZE) == 0) return -1;	//EOF check
	i = 0;
	while (*buff != '/') {
		file_buff[i] = *buff;
		read(in_fd, buff, 1);
		i++;
		if (i > 15) {
			printf("ERROR: Filename in archive longer than 15\n");
			printf("BLARGHHHH:%s\n", file_buff);
			exit(EXIT_FAILURE);
		}
	}
	file_buff[i] = '\0';
	if (filename != 0) strcpy(filename, file_buff);	//null check to avoid segfault
	lseek(in_fd, 47 - i, SEEK_CUR);
	read(in_fd, size_buff, 10);
	size_buff[10] = '\0';
	i = (int) strtol(size_buff, (char **)NULL, 10);
	if (skip == 1) {
		i += 2 + i % 2;
		lseek(in_fd, i, SEEK_CUR);
	}
	else {
		i += 60;
		lseek(in_fd, -58, SEEK_CUR);
		// lseek(in_fd, 2, SEEK_CUR);
	}
	return i;
}

/**************************
	Helper function for -v
	Validates the ar file and executes complex_step
**************************/
void verbose_step(int in_fd)
{
	if(is_ar(in_fd)) {
		complex_step(in_fd);
	} else {
		printf("myar: %s: File format not recognized\n", optarg);
		return;
	}
}

/**************************
	Helper function for -v
	Prints verbose file information from a given open ar file
	Pre: File offset must be at the first character of a file header
		in a valid ar file.
	Recursive implementation. (This could probably have been done in a way
	that would have made the function useful in other functions.)
**************************/
void complex_step(int in_fd)
{
	int i, j;
	struct ar_hdr *header = (struct ar_hdr *) malloc(sizeof(struct ar_hdr));
	read(in_fd, header->ar_name, 16);
	i=0;
	while(header->ar_name[i] != '/') i++;
	header->ar_name[i] = '\0';
	read(in_fd, header->ar_date, 12);
	header->ar_date[11] = '\0';
	read(in_fd, header->ar_uid, 6);
	header->ar_uid[5] = '/';
	read(in_fd, header->ar_gid, 6);
	header->ar_gid[5] = '\0';
	read(in_fd, header->ar_mode, 8);
	header->ar_mode[7] = '\0';
	read(in_fd, header->ar_size, 10);
	header->ar_size[9] = '\0';
	char size_str[6];
	j = 5;
	for (i=8; i >= 0; i--) {
		if (header->ar_size[i] != ' ') {
			size_str[j] = header->ar_size[i];
			j--;
		}
	}
	for (;j >= 0; j--) size_str[j] = ' ';
	size_str[6] = '\0';
	read(in_fd, header->ar_fmag, 2);

	printf("%s", filePermStr((mode_t)strtol(header->ar_mode, (char **)NULL, 8), 0));

	printf(" %s ", header->ar_uid);
	printf("%s ", size_str);

	// time_t rawtime = 1404500269;		//http://www.cplusplus.com/reference/ctime/ctime/
	time_t rawtime = (time_t) strtol(header->ar_date, (char **)NULL, 10);
	struct tm * timeinfo;
	char date_buffer[20];
	timeinfo = localtime (&rawtime);
	strftime(date_buffer, 20, "%b %e %R %Y ", timeinfo);
  	printf ("%s", date_buffer);

	printf("%s\n", header->ar_name);
	free(header);
	i = (int) strtol(header->ar_size, (char **)NULL, 10);
	if (i % 2 == 1) i += 1;
	lseek(in_fd, i, SEEK_CUR);			//skim through the file to the next header
	if (read(in_fd, (char **)NULL, 1) == 0) return;
	complex_step(in_fd);				//recursively call self until EOF
}

int main (int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <args> <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int c, n;		//for loops
	int in_fd;
	if ((c = getopt (argc, argv, "q:x:t:v:d:A:w:r:")) != -1) {
		
		switch(c) {
			case 'q':
				in_fd = open(optarg, O_RDWR);			//7-file_reverse.mp4  | O_APPEND
				if (in_fd == -1) {
					printf("myar: creating %s\n", optarg);
					in_fd = open(optarg, O_CREAT | O_WRONLY, 0666);			//7-file_reverse.mp4
					if (write(in_fd, ARMAG, 8) != 8) {
						perror("Error writing to file...");
						exit(EXIT_FAILURE);
					}
					if (in_fd == -1) {
						perror("Error creating file...");
						exit(EXIT_FAILURE);
					}
				} else {
					if (!is_ar(in_fd)) {
						fprintf(stderr, "myar: %s: File format not recognized\n", optarg);
						exit(EXIT_FAILURE);
					}
					lseek(in_fd, 0, SEEK_END);
				}
				n = 3;		//argv[n]
				while (n < argc) {
					ar_add(in_fd, argv[n]);
					n++;
				}
				if (close(in_fd) != 0) printf("Close failed!\n");
				break;
			case 'x':
				in_fd = open(optarg, O_RDONLY);
				if (in_fd == -1) {
					perror("xError");
					exit(EXIT_FAILURE);
				}
				if (argc < 4) {
					ar_extract_all(in_fd); //EXTRACT ALL
				} else {
					ar_extract(in_fd, argv, argc);		//CHANGEME
				}
				if (close(in_fd) != 0) printf("Close failed!\n");
				break;
			case 't':
				in_fd = open(optarg, O_RDONLY);
				if (in_fd == -1) {
					perror("tError");
					exit(EXIT_FAILURE);
				}
				simpler_step(in_fd);
				break;
			case 'v':
				in_fd = open(optarg, O_RDONLY);
				if (in_fd == -1) {
					perror("vError");
					exit(EXIT_FAILURE);
				}
				verbose_step(in_fd);
				if (close(in_fd) != 0) printf("Close failed!\n");
				break;
			case 'd':
				in_fd = open(optarg, O_RDONLY);
				if (in_fd == -1) {
					perror("xError");
					exit(EXIT_FAILURE);
				}
				if (argc < 4) {
					// * DO NOTHING * //
				} else {
					// printf("Deleting %s from archive\n", argv[i]);
					ar_delete(in_fd, argv, argc);
				}
				if (close(in_fd) != 0) printf("Close failed!\n");
				break;
			case 'A':
				in_fd = open(optarg, O_RDWR);			//7-file_reverse.mp4  | O_APPEND
				if (in_fd == -1) {
					printf("myar: creating %s\n", optarg);
					in_fd = open(optarg, O_CREAT | O_RDWR, 0666);	//7-file_reverse.mp4
					if (write(in_fd, ARMAG, 8) != 8) {
						perror("Error writing to file...");
						exit(EXIT_FAILURE);
					}
					if (in_fd == -1) {
						perror("Error creating file...");
						exit(EXIT_FAILURE);
					}
				} else {
					if (!is_ar(in_fd)) {
						fprintf(stderr, "myar: %s: File format not recognized\n", optarg);
						exit(EXIT_FAILURE);
					}
					lseek(in_fd, 0, SEEK_END);
				}
				//	Following code modified from 
				//  http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1046380353&id=1044780608
				DIR *d;
				struct dirent *dir;
				d = opendir(".");
				if (d) {
					while ((dir = readdir(d)) != NULL) {
						if (dir->d_type == DT_REG 
							&& (strcmp(dir->d_name, optarg) != 0)
							&& (strcmp(dir->d_name, "myar") != 0)		 //for testing purposes
							&& (strcmp(dir->d_name, "sig_demo") != 0)	 //excludes these binary files
							) {
						    // printf("%s\n", dir->d_name);
						    ar_add(in_fd, dir->d_name);
						}
					}
					closedir(d);
				}
				if (close(in_fd) != 0) printf("Close failed!\n");
				break;
			case 'w':
				printf("w! I do nothing! 25 points of extra credit?\n");
				break;
			case 'r':
				printf("r! I also do nothing and am worth 25 ex cred!\n");
				break;
			default:
				printf("Exiting program.\n");
				break;
		}
		
	} else {
		printf("Did you forget to use '-' before your argument?\n");
	}
	return 0;
}