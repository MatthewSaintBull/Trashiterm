#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <time.h>

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wformat-security"

char* get_deletiondate(){
	time_t ttime; 
	struct tm * timeinfo;
	time( &ttime);
	timeinfo = localtime(&ttime);
	char* deletiontime;

	char year[4]; 
	sprintf(year,"%d",timeinfo->tm_year+1900);
	char month[2];
	sprintf(month,"%d",timeinfo->tm_mon+1);
	char day[2];
	sprintf(day,"%d",timeinfo->tm_mday);
	char hour[2];
	sprintf(hour,"%d",timeinfo->tm_hour);
	char minutes[2];
	sprintf(minutes,"%d",timeinfo->tm_min);
	char seconds[2];
	sprintf(seconds,"%d",timeinfo->tm_sec);
	char tract = '-';
	char colon = ':';
	//generates the deletion date by the syntax of the trashinfo files
	asprintf(&deletiontime,"%s%c%s%c%s%c%s%c%s%c%s",year,tract,month,tract,day,'T',hour,colon,minutes,colon,seconds);
	return deletiontime;
}

char* get_username(){
	struct passwd *p = getpwuid(getuid()); //gets the passwd structure from the /etc/passwd file by the uid
	return p->pw_name; 

}

void gen_trashinfo(char* username, char* filename, char* cwd_n_file){
	FILE *infofile; //file for trashinfos
	char* dirtinfo = "/.local/share/Trash/info/"; //partial path of the trashinfos	
	asprintf(&dirtinfo, "%s%s%s%s%s","/home/",username,dirtinfo, filename,".trashinfo"); //generates the full trashinfos path
	infofile = fopen(dirtinfo,"w"); //trashinfo file
	char* whatwrite; //chars array which'll contains the infos of the file	
	char* deldate; //chars array which'll contains the deletion date information 
	deldate  = get_deletiondate(); //gets the deletion date
	
	//generates the full trashinfo file
	asprintf(&whatwrite, "%s%s%s%s%s","[Trash Info]\n","Path=",cwd_n_file,"\nDeletionDate=",deldate); 

	fprintf(infofile,whatwrite); //writes the trashinfo file
	fclose(infofile); //closes the file	

}

void mv_file(char* username, char* filename, char* cwd_n_file){
	char* dirfile = "/.local/share/Trash/files/"; //partial path of the files directory 
	asprintf(&dirfile,"%s%s%s%s","/home/",username,dirfile,filename);
	rename(cwd_n_file,dirfile);	
}

void check_arguments(char* arguments[]){
	if(arguments[2]!=NULL){  //check if there are more than one argument passed during the execution
		printf("you've passed a wrong number of arguments\n");
		printf("usage : trashiterm file\n");
		exit(0);
	}
}

void exists(char* filename){
	FILE *filecheck = fopen(filename, "rb");
	if(filecheck == NULL){
		printf("the file you are trying to remove doesn't exists\n");
		exit(0);
	} 
}

int main(int argc, char *argv[]) {
	check_arguments(argv);
	char* filename = argv[1]; //file passed as argument
	exists(filename);
	char* cwd_n_file;
	char cwd[1024]; //chars array which'll contains the current working directory
	getcwd(cwd, sizeof(cwd)); //get the current working directory
	asprintf(&cwd_n_file,"%s%s%s",cwd,"//",filename); //generates the new path of the file
	
	char* username = get_username(); //username of the user
	
	gen_trashinfo(username,filename,cwd_n_file);
	mv_file(username,filename,cwd_n_file);
}
