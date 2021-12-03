# Linux-File-Explorer
Implemented a simple Linux File Explorer using C++. The File Explorer has two working modes, a Normal mode where the user can browse, open, traverse through the folders and a Command mode where a user can search/copy/rename/move/create/delete files and folders.

* Location of code file is current working directory and `home` for file explorer.
* Program can be exited using `q` or `Ctrl+X`.

## Normal Mode
* Default mode of file explorer, allowing user to navigate through file system using keyboard shortcuts.
* Displays directory content similar to `ls` command output in Unix based systems. 

![normal_mode](https://github.com/prashantdukecyfi/Linux-File-Explorer/blob/main/images/1.png)

![normal_mode](https://github.com/prashantdukecyfi/Linux-File-Explorer/blob/main/images/2.png)

### Keyboard Shortcuts
1. Up: Up arrow key
2. Down: Down arrow key
3. Scroll Up: `k`
4. Scroll Down: `l`
5. Open Directory/File: `Enter`
6. Previously visited directory: Left arrow key
7. Next directory: Right arrow key
8. Go up one level: Backspace
9. Home: `h`
10. To quit press `q`.

## Command Mode
* Allows user to enter different commands at the bottom of the screen.
* Press `:` to enter command mode.

![command_mode](https://github.com/prashantdukecyfi/Linux-File-Explorer/blob/main/images/3.png)

### Commands
1. Copy: `copy <source_file(s)/directory(ies)> <destination_directory>`
2. Move: `move <source_file(s)/directory(ies)> <destination_directory>`
3. Rename: `rename <old_filename> <new_filename>`
4. Create File: `create_file <file_name> <destination_path>` <br>
    Destination path should be relative to home.
5. Create Directory: `create_dir <dir_name> <destination_path>` <br>
    Destination path should be relative to home.    
6. Delete File: `delete_file <file_path>`<br>
    Destination path should be relative to home.    
7. Delete Directory: `delete_dir <dir_path>`<br>
    Destination path should be relative to home.
8. Goto: `goto <location>` <br>
    Location is absolute path w.r.t. home.
9. Search: `search <file_name>` or `search <directory_name>`
    File/Directory name should be relative to home. 
10. Exit command mode: `Esc` key
