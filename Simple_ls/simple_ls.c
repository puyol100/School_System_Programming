/////////////////////////////////////////////////////////
// File Name       : simple_ls.c		       //  
// Date 	   : 2019/04/02			       //					    
// Os	           : Ubuntu 16.04 LTS 64bits	       //					     
// Author          : Han Sung Min		       //					    
// Student ID      : 2015722088			       //
// ----------------------------------------------------//    
// Title : System Programing Assignment #1-1 (ls)      //
// Decription: ...				       //
/////////////////////////////////////////////////////////

# include <stdio.h>
# include <stdlib.h>//head for dynamic allocation
# include <string.h>
# include <dirent.h>//setting dirent for directory function

/////////////////////////////////////////////////////////
// sort_print 					       //
// ====================================================//
// Input a  --> array pointer array		       //
// Input b  --> total count 			       //
// swap --> for pointer swappintg		       //
// Loop function1 --> in this function compare and swap//
// strcasecmp --> cmp case insensitive		       //
// Loop function2 -->function for print array 	       //
/////////////////////////////////////////////////////////
void sort_print(char ** a,int b){// sort_print func
	char *swap;//variable for point swap
	for(int i=0;i<b-1;i++){//swapping
		for(int j=i; j<b-1;j++){
		if(strcasecmp(a[i],a[j])>0){
			swap = a[i];
			a[i] = a[j];
			a[j] = swap;}
		}
	}
	for(int i=0; i<b-1; i++){//printing
		printf("%s\n",a[i]);
	}
	return;	
}
int main(int argv, char *argc[]){//function for main
	DIR *drip;//for stream
	struct dirent *dir;//for inputting directry information
	int i=0;
	int count =1;//for counting the number of total 
	char **arr;//for pointer array
	if(argc[1] == NULL){
	drip = opendir(".");//if ls working dirctory
	}
	else {//else case
	drip = opendir(argc[1]);//open directory
	if(drip == NULL){
	 printf("simple_ls: cannot access '%s' : No such directory\n",argc[1]);
	return 0;}
	else{
		if(argc[2]!=NULL){ printf("simple_ls: only one directory path can be processed\n"); return 0;}
	}	
	}//for exception
	while((dir = readdir(drip))!=NULL){
		if(dir->d_name[0]!='.'){//if directory is not hidden file
			count = count +1;//coutting the number of total
		}
	}	
	rewinddir(drip);//rewind 
	arr = (char**)malloc(sizeof(char*)*count);//allocate memory
	while((dir = readdir(drip))!=NULL){
		if(dir->d_name[0]!='.'){
		arr[i] = (dir->d_name);	//store d_name to arr[i]

		i++;		
		}
	}
	sort_print(arr,count);//func for sort and print
	free(arr);//allocation free
	closedir(drip);
	return 0;
}
