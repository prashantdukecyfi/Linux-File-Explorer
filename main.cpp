#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <iostream>
#include <bits/stdc++.h>
#include <iomanip>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <cstddef>
#include <pwd.h>
#include <grp.h>
#define putcur(x,y) printf("\033[%d;%dH",(x),(y))
using namespace std;

int rows, cols, n_rows, n_cols, c_rows;
DIR *c_dir;
string path, cur_folder, command;
vector<vector<string>> directories;
vector<string> p_vector;
vector<string> c_vector;
int p_pos;
int upper, low;
int cursor, cursor2;
int l_filename;
struct winsize window;

struct termios non_canonical_mode()
{
	struct termios old_terminal;
	tcgetattr(1, &old_terminal);
	struct termios new_terminal=old_terminal;
	new_terminal.c_lflag &= ~ICANON;
	new_terminal.c_lflag &= ~ECHO;
	new_terminal.c_cc[VMIN] = 1;
	new_terminal.c_cc[VTIME] = 0;
	tcsetattr(1, TCSANOW, &new_terminal);
	return(old_terminal);
}

void canonical_mode(struct termios term_p)
{
	tcsetattr(1, TCSANOW, &term_p);
}

bool sortcomp(const vector<string>& v1, const vector<string>& v2)
{ 
	return(v1[0]<v2[0]); 
}

bool checkdir(string p)
{
	struct stat get_dir_info;
	stat(p.c_str(), &get_dir_info);
	if(S_ISDIR(get_dir_info.st_mode))
		return(true);
	return(false);
}

bool checkfile(string p)
{
	FILE* f1;
	f1=fopen(p.c_str(), "r");
	if(f1)
	{
		fclose(f1);
		return(true);
	}
	return(false);
}

string file_size(off_t size)
{
	long long k, m, g;
	k=1024;
	m=1024*1024;
	g=1024*1024*1024;
	if(size<k)
		return(to_string(size)+" B");
	else if(size<m)
	{
		size/=k;
		return(to_string(size)+" KB");
	}
	else if(size<g)
	{
		size/=m;
		return(to_string(size)+" MB");
	}
	else 
	{
		size/=g;
		return(to_string(size)+" GB");
	}
}

string permission(mode_t mod)
{
	string perm;
	if(mod & 0400)
		perm+='r';
	else
		perm+='-';
	if(mod & 0200)
		perm+='w';
	else
		perm+='-';
	if(mod & 0100)
		perm+='x';
	else
		perm+='-';
	if(mod & 0040)
		perm+='r';
	else
		perm+='-';
	if(mod & 0020)
		perm+='w';
	else
		perm+='-';
	if(mod & 0010)
		perm+='x';
	else
		perm+='-';
	if(mod & 0004)
		perm+='r';
	else
		perm+='-';
	if(mod & 0002)
		perm+='w';
	else
		perm+='-';
	if(mod & 0001)
		perm+='x';
	else
		perm+='-';
	return(perm);
}

void list_all(string p)
{
	struct dirent* dir_info;
	c_dir=opendir(p.c_str());
	l_filename=0;
	if(c_dir!=NULL)
	{
		directories.clear();
		while(dir_info=readdir(c_dir))
		{
			struct stat get_info;
			vector<string> values;
			string abs_path=p+'/'+dir_info->d_name;
			stat(abs_path.c_str(), &get_info);
			struct passwd *pwd=getpwuid(get_info.st_uid);
			struct group *group_info=getgrgid(get_info.st_gid);
			string file=dir_info->d_name;
			string uid=string(pwd->pw_name);
			string gid=string(group_info->gr_name);
			string perms=permission(get_info.st_mode);
			string time=string(ctime(&get_info.st_mtime));
			string f_size=file_size(get_info.st_size);
			int temp=file.size();
			time.pop_back();
			if(S_ISDIR(get_info.st_mode))
				perms="d"+perms;
			else
				perms="-"+perms;
			l_filename=max(l_filename, temp);
			values.push_back(file);
			values.push_back(f_size);
			values.push_back(uid);
			values.push_back(gid);
			values.push_back(perms);
			values.push_back(time);
			directories.push_back(values);
		}
		closedir(c_dir);
	}
}

void display()
{
	int c=0;
	sort(directories.begin(), directories.end(), sortcomp);
	int n=directories.size();
	l_filename=min(l_filename, (cols/2)-1);
	for(vector<string> s: directories)
	{
		if(c==rows-2)
			break;
		c++;
		if(s[0].size()<(cols/2)-1)
			cout << left << setw(l_filename) << s[0] << " ";
		else
			cout << left << setw(l_filename) << s[0].substr(0, l_filename-2) << " ";
		cout << right << setw(7) << s[1] << " ";
		cout << s[2] << " ";
		cout << s[3] << " ";
		cout << s[4] << " ";
		cout << s[5] << endl;
	}
	if(c!=rows-2)
	{
		for(int i=c; i<rows-2; i++)
			cout << endl;
	}
	else
	{
		n_rows=n-rows-2;
	}
	cout << "Normal Mode;\tPress q to exit;\tPress : to enter Command Mode" << endl;
	cursor=0;
	putcur(1,1);
}

void work(string p)
{
	p_vector.push_back(p);
	p_pos=p_vector.size();
	cout << "\033[2J\033[1;1H";
	list_all(p);
	display();
}

void work_lr(string p)
{
	cout << "\033[2J\033[1;1H";
	list_all(p);
	display();
}

void copy_file(string file1, string file2)
{
	FILE *src;
	FILE *dest;
	char t;

	if(((src=fopen(file1.c_str(), "r"))==NULL) || ((dest=fopen(file2.c_str(), "w"))==NULL))
	{
		perror("");
		return;
	}

	struct stat src_file_info;
	stat(file1.c_str(), &src_file_info);
	chown(file2.c_str(), src_file_info.st_uid, src_file_info.st_gid);
	chmod(file2.c_str(), src_file_info.st_mode);

	while(!feof(src))
	{
		t=getc(src);
		putc(t, dest);
	}
	fclose(src);
	fclose(dest);
}

void copy_dir(string dir1, string dir2)
{
	DIR* d_temp;
	d_temp=opendir(dir1.c_str());
	if(!d_temp)
	{
		perror("");
		return;
	}
	struct dirent* d;
	d=readdir(d_temp);
	while(d)
	{
		if(string(d->d_name)=="." || string(d->d_name)=="..")
		{
			d=readdir(d_temp);
			continue;
		}
		else
		{
			string src, dest;
			src=dir1+"/"+string(d->d_name);
			dest=dir2+"/"+string(d->d_name);
			if(checkdir(src))
			{
				if(mkdir(dest.c_str(), 0755)==0)
					copy_dir(src, dest);
				else
				{
					perror("");
					return;
				}
			}
			else
				copy_file(src, dest);
			d=readdir(d_temp);
		}
	}
	closedir(d_temp);
}


void do_copy()
{
	int i;
	string tmp;
	if(c_vector.size()<3)
	{
		cout << "\n=> Invalid arguments" << endl;
		return;
	}
	else
	{
		if(checkdir(c_vector[c_vector.size()-1]))
		{
			for(i=1; i<c_vector.size()-1; i++)
			{
				int pos=c_vector[i].find_last_of("/");
				tmp=c_vector[c_vector.size()-1]+"/"+c_vector[i].substr(pos+1, c_vector[1].size());
				if(checkdir(c_vector[i]))
				{
					if(mkdir(tmp.c_str(), 0755)==0)
						copy_dir(c_vector[i], tmp);
					else
						perror("");
				}
				else
					copy_file(c_vector[i], tmp);
			}
		}
	}
	cout << "\n=> Done" << endl;
}

void delete_rec(string p)
{
	DIR* temp;
	temp=opendir(p.c_str());
	string new_path;
	struct dirent* d_info;
	if(temp)
	{
		while(d_info=readdir(temp))
		{
			if(string(d_info->d_name)=="." || string(d_info->d_name)=="..")
				continue;
			else
			{
				new_path=p+"/"+d_info->d_name;
				if(checkdir(new_path))
					delete_rec(new_path);
				else
					remove(new_path.c_str());
			}
		}
		rmdir(p.c_str());
		closedir(temp);
	}
	return;
}

void do_move()
{
	if(c_vector.size() < 3)
	{
		cout << "\n=> Invalid arguments" << endl;
		return;
	}
	do_copy();
	c_vector.pop_back();
	for(int i=1; i<c_vector.size(); i++)
	{
		if(checkdir(c_vector[i]))
			delete_rec(c_vector[i]);
		else
			remove(c_vector[i].c_str());
	}
	cout << "\n=> Done" << endl;
}

void do_rename()
{
	if(c_vector.size()!=3)
		cout << "\n=> Invalid arguments" << endl;
	else
	{
		int r=rename(c_vector[1].c_str(), c_vector[2].c_str());
		if(r==-1)
			cout << "\n=> Error" << endl;
		else
			cout << "\n=> Done" << endl;
	}
}

void do_create_file()
{
	if(c_vector.size()!=3)
		cout << "\n=> Invalid arguments" << endl;
	else
	{
		int pos=c_vector[1].find_last_of("/");
		c_vector[1]=c_vector[1].substr(pos+1, c_vector[1].size());
		if(checkdir(c_vector[2]))
		{
			FILE* f1;
			string new_path=c_vector[2]+"/"+c_vector[1];
			cout << new_path << endl;
			f1=fopen(new_path.c_str(), "w+");
			if(f1)
				cout << "\n=> Done" << endl;
			else
				cout << "\n=> Error" << endl;
			fclose(f1);
		}
		else
			cout << "\n=> Error" << endl;
	}
}

void do_create_dir()
{
	int pos=c_vector[1].find_last_of("/");
	c_vector[1]=c_vector[1].substr(pos+1, c_vector[1].size());
	if(c_vector.size()!=3)
		cout << "\n=> Invalid arguments" << endl;
	else
	{
		if(checkdir(c_vector[2]))
		{
			string new_path=c_vector[2]+"/"+c_vector[1];
			if(mkdir(new_path.c_str(), 0755)==0)
				cout << "\n=> Done" << endl;
			else
				cout << "\n=> Error" << endl;
		}
		else
			cout << "\n=> Error" << endl;
	}
}

void do_delete_file()
{
	if(c_vector.size()!=2)
		cout << "\n=> Invalid arguments" << endl;
	else
	{
		if(checkfile(c_vector[1]))
		{
			if(remove(c_vector[1].c_str())==0)
				cout << "\n=> Done" << endl;
			else
				cout << "\n=> Error" << endl;
		}
		else
			cout << "\n=>Error" << endl;
	}
}

void do_delete_dir()
{
	if(c_vector.size()!=2)
		cout << "\n=> Invalid arguments" << endl;
	else
	{
		if(checkdir(c_vector[1]))
		{
			delete_rec(c_vector[1]);
			cout << "\n=> Done" << endl;
		}
		else
			cout << "\n=>Error" << endl;
	}
}

void do_goto()
{
	if(c_vector.size()!=2)
		cout << "\n=> Invalid arguments" << endl;
	else
	{
		if(checkdir(c_vector[1]))
		{
			string p=c_vector[1]+"/";
			if(p==p_vector.back())
				cout << "\n=> Done" << endl;
			else
			{
				p_vector.push_back(p);
				cur_folder=p;
				p_pos=p_vector.size();
				cout << "\n=> Done" << endl;
			}
		}
		else
			cout << "\n=> Error" << endl;
	}
}

bool search_util(string look_in, string target)
{
	DIR* temp;
	temp=opendir(look_in.c_str());
	struct dirent* d_info;
	bool chk=false;
	if(temp)
	{
		d_info=readdir(temp);
		while(d_info)
		{
			if(string(d_info->d_name)=="." || string(d_info->d_name)=="..")
			{
				d_info=readdir(temp);
				continue;
			}
			else if(string(d_info->d_name)==target)
			{
				closedir(temp);
				return(true);
			}
			else
			{
				string t=look_in+string(d_info->d_name);
				if(checkdir(t))
				{
					t=t+"/";
					chk=search_util(t, target);
					if(chk)
						return(true);
				}
			}
			d_info=readdir(temp);
		}
		closedir(temp);
	}
	return(false);
}

void do_search()
{
	if(c_vector.size()!=2)
		cout << "\n=> Invalid arguments" << endl;
	else
	{
		int pos=c_vector[1].find_last_of("/");
		c_vector[1]=c_vector[1].substr(pos+1, c_vector[1].size());
		if(search_util(cur_folder, c_vector[1]))
			cout << "\n=> True" << endl;
		else
			cout << "\n=> False" << endl;
	}
}

void re_split(string inp)
{
	int n, i;
	string t="";
	c_vector.clear();
	n=inp.size();

	for(i=0; i<n; i++)
	{
		if(inp[i]==' ')
		{
			c_vector.push_back(t);
			t="";
		}
		else if(inp[i]=='\\')
		{
			t+="\\ ";
			i++;
		}
		else
			t+=inp[i];
	}

	for(i=1; i<c_vector.size(); i++)
	{
		string x="";
		if(c_vector[i][0]=='~' || (c_vector[i][0]=='.' && c_vector[i][1]=='/'))
		{
			x=c_vector[i];
			c_vector[i]=path+x.substr(1,x.size());
		}
		else if(c_vector[i][0]=='/')
			c_vector[i]=path+c_vector[i];
		else
			c_vector[i]=path+"/"+c_vector[i];
	}

	if(c_vector[0]=="copy")
		do_copy();
	else if(c_vector[0]=="move")
		do_move();
	else if(c_vector[0]=="rename")
		do_rename();
	else if(c_vector[0]=="create_file")
		do_create_file();
	else if(c_vector[0]=="create_dir")
		do_create_dir();
	else if(c_vector[0]=="delete_file")
		do_delete_file();
	else if(c_vector[0]=="delete_dir")
		do_delete_dir();
	else if(c_vector[0]=="goto")
		do_goto();
	else if(c_vector[0]=="search")
		do_search();
	else
		cout << "\nUnidentified command" << endl;
}

void cmd_mode()
{
	cout << "\033[2J\033[1;1H";
	for(int i=0; i<rows-3; i++)
		cout << endl;
	cout << "Command Mode;\tPress ESC to exit;\nCommands - copy, move, rename, create_file, create_dir, delete_file, delete_dir, goto, search" << endl;
	cursor=1;
	cursor2=1;
	putcur(1,1);
	char g;
	command="";
	while(1)
	{
		g=cin.get();
		if(g==27)
			break;
		else
		{
			if(g==10)
			{
				command.push_back(' ');
				if(cursor<rows-8)
				{
					if(command.size()>=1)
					{
						re_split(command);
						cursor+=2;
						cursor2=1;
						putcur(cursor, cursor2);
					}
				}
				else
				{
					cout << "\033[2J\033[1;1H";
					for(int i=0; i<rows-3; i++)
						cout << endl;
					cout << "Command Mode;\tPress ESC to exit;\nCommands - copy, move, rename, create_file, create_dir, delete_file, delete_dir, goto, search" << endl;
					putcur(1, 1);
					cout << command;
					if(command.size()>=1)
					{
						re_split(command);
						cursor=3;
						cursor2=1;
						putcur(cursor, cursor2);
					}
				}
				command.clear();
			}
			else if(g==127)
			{
				if(command.size())
					command.pop_back();
				if(cursor2>1)
					cursor2-=1;
				printf("\b \b");
				putcur(cursor, cursor2);
			}
			else
			{
				command+=g;
				cout << g;
				cursor2+=1;
				putcur(cursor,cursor2);
			}
		}
	}
}

int main()
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
	rows=window.ws_row;
	cols=window.ws_col;
	struct termios cur_term=non_canonical_mode();
	char c;
	path=get_current_dir_name();
	cur_folder=path+'/';
	p_vector.push_back(path);
	p_pos++;
	cout << "\033[2J\033[1;1H";
	list_all(path);
	display();
	while(1)
	{
		c=cin.get();
		if(c=='q')
		{
			cout << "\033[2J\033[1;1H";
			break;
		}
		else if(c=='A')
		{
			if(cursor>0)
			{
				cursor--;
				printf("\033[A");
			}
		}
		else if(c=='B')
		{
			if(cursor<directories.size()-1)
			{
				cursor++;
				printf("\033[B");
			}
		}
		else if(c=='C')
		{
			if(p_pos<p_vector.size())
			{
				cur_folder=p_vector[++p_pos-1];
				work_lr(cur_folder);
			}
		}
		else if(c=='D')
		{
			if(p_pos>1)
			{
				cur_folder=p_vector[--p_pos-1];
				work_lr(cur_folder);
			}
		}
		else if(c=='h' || c=='H')
		{
			cur_folder=path;
			work(path);
		}
		else if(c==127)
		{
			string folder_name="";
			folder_name=cur_folder;
			if(folder_name[(folder_name.size()-1)]=='/')
				folder_name.pop_back();
			int last_pos=folder_name.find_last_of("/");
			folder_name=folder_name.substr(0,last_pos);
			cur_folder=folder_name;
			work(folder_name);
		}
		else if(c==10)
		{
			if(directories[cursor][4][0]=='-')
			{
				pid_t pid=fork();
				if(pid==0)
				{
					string cmd="vi";
					string file_name=cur_folder+directories[cursor][0];
					char* args[3];
					args[0]=(char*)cmd.c_str();
					args[1]=(char*)file_name.c_str();
					args[2]=NULL;
					execvp(args[0], args);
				}
				wait(NULL);
			}
			else if(directories[cursor][4][0]=='d')
			{
				string folder_name="";
				if(directories[cursor][0]!="." && directories[cursor][0]!="..")
				{
					folder_name=cur_folder;
					if(folder_name[(folder_name.size()-1)]=='/')
						folder_name.pop_back();
					folder_name=folder_name+'/'+directories[cursor][0];
				}
				else if(directories[cursor][0]==".")
					continue;
				else
				{
					folder_name=cur_folder;
					if(folder_name[(folder_name.size()-1)]=='/')
						folder_name.pop_back();
					int last_pos=folder_name.find_last_of("/");
					folder_name=folder_name.substr(0,last_pos);
				}
				upper=low=0;
				cur_folder=folder_name;
				work(folder_name);
			}
		}
		else if(c==':')
		{
			cmd_mode();
			work_lr(cur_folder);
		}
	}
	canonical_mode(cur_term);
	return(0);
}