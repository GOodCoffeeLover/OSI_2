#include<unistd.h>
#include <fcntl.h>
#include<stdlib.h>
#include<sys/wait.h>
//STDIN_FILENO AND STDOUT_FILENO
int main(){
	int pipes[2];
	int nob[2];
	if(pipe(pipes)<-1){
		write(STDOUT_FILENO, "We are in troubles\n", sizeof(char)*19);
		return -1;
	}
	if(pipe(nob)<-1){
		write(STDOUT_FILENO, "We are in troubles\n", sizeof(char)*19);
		return -4;
	}
	pid_t child= fork();
	if(child< -1){
		write(STDOUT_FILENO, "We are in troubles\n", sizeof(char)*19);
		return -2;
	}
	if( child ){
		char s;
		long long size=0;
		close(pipes[0]);
		while(read(STDIN_FILENO, &s, sizeof(char))>0){
			write(pipes[1], &s, sizeof(char));
			size+=1;
			if(s=='\n'){ 
				write(nob[1],&size, sizeof(size)); 
				size=0;
			}			
		}
		size=0;
		write(nob[1], &size, sizeof(size));
		int k; waitpid(child,&k, 0 );
		close(pipes[1]);
	}else{
		off_t lenght=0;
		close(pipes[1]);
		char s, s2;
		char* name=malloc(sizeof(char));
		int size=1, file=0;
		long long bites=1;
		int i=0;
		int swtch1=1, swtch=0;
		read(nob[0], &bites, sizeof(bites));
		while(bites>0){
			for(i=0; i<bites; ++i){
				read(pipes[0], &s, sizeof(char));
				if(swtch1){
					switch(swtch){
						case 0: {
							if(!(s>='a'&& s<='z' || s>='A'&& s<='Z'|| s=='\'')) break; 
							if(s=='\''){
								swtch=1;
							}else{
								name[size-1]=s;
								name=realloc(name, sizeof(char)*(++size));
								swtch=-1;
							}
							break;
						}
						case -1: {
							if((s>='a'&&s<='z'||s>='A' && s<='Z'|| s>='0' && s<='9' || s=='.')){
								name[size-1]=s;
								name=realloc(name, sizeof(char)*(++size));
							}else{
								name[size-1]='\0';
								swtch=2;
							}
							break;
						}
						case 1:{
							if(s!='\''){
								name[size-1]=s;
								name=realloc(name, sizeof(char)*(++size));
							}else{
								name[size-1]='\0';
								swtch=2;
								read(pipes[0], &s, sizeof(char));
								++i;
							}
							break;
						}	
						case 2:{
							file=open(name, O_WRONLY | O_CREAT, S_IRWXU);
							if(file==-1){
								write(STDOUT_FILENO, "We are in troubles\n", sizeof(char)*19);
								return -3;
							}
							s2=s;
							swtch1=0;
							break;
						}
					}
				}else{
					if(!(s2==' ' && s==' ')){ 
						write(file, &s2, sizeof(char));
						++lenght;
					}
					s2=s;
				}
			}
			read(nob[0], &bites, sizeof(bites));
		}
		write(file, &s2, sizeof(char));
		++lenght;
		if(ftruncate(file, lenght)!=0)
			write(STDOUT_FILENO, "Error\n", sizeof(char)*6);
		close(file);
		free(name);
		return 0;
		close(pipes[0]);
	}
	return 0;
}

