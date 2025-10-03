#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
namespace fs = std::filesystem;
using namespace std;

class FileSystemItem {
protected:
    string path; 
public:
    FileSystemItem(string p) : path(p) {}
    virtual void showInfo() = 0;     
    virtual void deleteItem() = 0;    
    virtual void renameItem(string newName) = 0; 
    virtual uintmax_t getSize() = 0;  
};

class FileItem : public FileSystemItem {
public:
    FileItem(string p) : FileSystemItem(p) {}
    void showInfo() override { cout << "File: " << path << endl; }
    void deleteItem() override { fs::remove(path); }
    void renameItem(string newName) override {
        fs::rename(path, fs::path(path).parent_path()/newName);
        path = (fs::path(path).parent_path()/newName).string();
    }
    uintmax_t getSize() override { return fs::file_size(path); }
};

class FolderItem : public FileSystemItem {
public:
    FolderItem(string p) : FileSystemItem(p) {}
    void showInfo() override { cout << "Folder: " << path << endl; }
    void deleteItem() override { fs::remove_all(path); }
    void renameItem(string newName) override {
        fs::rename(path, fs::path(path).parent_path()/newName);
        path = (fs::path(path).parent_path()/newName).string();
    }
    uintmax_t getSize() override {
        uintmax_t size = 0;
        for(auto& p: fs::recursive_directory_iterator(path))
            if(fs::is_regular_file(p)) size += fs::file_size(p);
        return size;
    }
};

class FileManager {
public:
    void showDriveContents(string path) {
        cout << "Contents of " << path << ":" << endl;
        for(auto& p: fs::directory_iterator(path))
            cout << "  " << p.path().string() << endl;
    }
    void createFile(string path) { ofstream(path); }
    void createFolder(string path) { fs::create_directory(path); }
    void copyItem(string source, string dest) {
        if(fs::is_directory(source))
            fs::copy(source, dest, fs::copy_options::recursive);
        else
            fs::copy_file(source, dest, fs::copy_options::overwrite_existing);
    }
    void moveItem(string source, string dest) { fs::rename(source, dest); }
    void searchMask(string path, string mask) {
        cout << "Search results for '" << mask << "':" << endl;
        for(auto& p: fs::recursive_directory_iterator(path))
            if(p.path().filename().string().find(mask) != string::npos)
                cout << "  " << p.path().string() << endl;
    }
};

int main() {
    FileManager fm;

    fm.createFolder("test_folder");
    fm.createFile("test_folder/file1.txt");
    fm.createFile("test_folder/file2.txt");

    fm.showDriveContents(".");

    fm.searchMask(".", ".txt");

    fm.copyItem("test_folder", "copy_folder");
    fm.moveItem("copy_folder", "moved_folder");

    FolderItem f("test_folder");
    cout << "Folder size: " << f.getSize() << " bytes" << endl;
}
