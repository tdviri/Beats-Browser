#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

typedef struct {
   string genre;
   string artist;
   string title;
   string trackid;
   int popularity;
} Song;

typedef struct {
   Song* songs;        
   size_t size;       
   size_t capacity; 
} SongArrayList ;

typedef struct {
   string genre;
   int count;
} GenreInfo;

typedef struct LinkListNode {
   GenreInfo* data;
   LinkListNode* next;
} LinkListNode;

vector<string> genres;
vector<string> artists;
vector<string> titles;
vector<string> trackids;
vector<int> popularity;

void readInSongs(SongArrayList *songList){
    ifstream file("songs2.5k.txt");
    if (!file.is_open()){
        cout << "Error opening file" << endl;
    }
    else {
        songList->capacity = 3000;
        songList->songs = new Song[songList->capacity];
        songList->size = 0;
        string line;
        int songCount = 0;
        while (getline(file, line)){
            songList->size++;
            if (line.substr(0, 5) == "genre"){
                songList->size--;
                continue;
            }
            if (songList->size == songList->capacity){
                songList->capacity *= 2;
                Song* newSongs = new Song[songList->capacity];
                memcpy(newSongs, songList->songs, songList->size * sizeof(Song));
                delete[] songList->songs;
                songList->songs = newSongs;
            }

            istringstream iss(line);
            getline(iss, songList->songs[songList->size - 1].genre, '\t');
            getline(iss, songList->songs[songList->size - 1].artist, '\t');
            getline(iss, songList->songs[songList->size - 1].title, '\t');
            getline(iss, songList->songs[songList->size - 1].trackid, '\t');
            iss >> songList->songs[songList->size - 1].popularity;
        }
    file.close();
}
}

void printSongList(SongArrayList* songList, int starting_index){
    if (starting_index > 0){
        for (int i = starting_index; i < starting_index + 10 && i <= songList->size; i++) {
        cout << "\t" << i << ". " << songList->songs[i - 1].genre << " - " << songList->songs[i - 1].artist << " - " << songList->songs[i - 1].title << endl;
        }
    }
    else{
        cout << "Must provide a starting index greater than 0";
    }
}

void generateUniqueGenres(LinkListNode** ptrUniqueGenresLL, SongArrayList* songList) {
    if (songList->size > 0) {
        LinkListNode* uniqueGenreHead = new LinkListNode;
        *ptrUniqueGenresLL = uniqueGenreHead;

        uniqueGenreHead->data = new GenreInfo;
        uniqueGenreHead->data->genre = songList->songs[0].genre;
        uniqueGenreHead->data->count = 1;
        uniqueGenreHead->next = nullptr;

        for (int i = 1; i < songList->size; i++) {
            string g = songList->songs[i].genre;
            bool match = false;

            LinkListNode* temp = *ptrUniqueGenresLL;
            while (temp != nullptr) {
                if (temp->data->genre == g) {
                    match = true;
                    temp->data->count++;
                    break;
                }
                if (temp->next == nullptr){
                    break;
                }
                temp = temp->next;
            }

            if (!match) {
                temp->next = new LinkListNode;
                temp->next->data = new GenreInfo;
                temp->next->data->genre = g;
                temp->next->data->count = 1;
                temp->next->next = nullptr;
            }
        }
    }
}

void printUniqueLinkList(LinkListNode* uniqueLinkList){
    LinkListNode* temp = uniqueLinkList;
    while (temp != nullptr){
        cout << temp->data->genre << ", " << temp->data->count << endl;
        temp = temp->next;
    }
}

string filterPrompt(LinkListNode* uniqueLinkList){
      LinkListNode* temp = uniqueLinkList;
        int size = 0;
        while (temp != nullptr){
            size++;
            temp = temp->next;
        }

        temp = uniqueLinkList;
        string* genres = new string[size];
        for (int i = 0; i < size; i++){
            genres[i] = temp->data->genre;
            temp = temp->next;
        }
    while (true){
        string chosenGenreStr = "";
        cout << "Do you want to filter based on genre? Enter 'yes' or 'no'" << endl;
        string input = "";
        cin >> input;
        char navKey = '\0';
        int chosenGenre = -1;
        if (input == "yes"){
            cout << "Choose a genre: " << endl;
            for (int i = 0; i < size - 1; i++){
                cout << "[" << i << "] " << genres[i] << " - ";
            }
            cout <<  "[" << size - 1 << "] " << genres[size - 1] << endl;
            cin >> chosenGenre;
            if (chosenGenre < 0 || chosenGenre > size - 1){
                cout << "Invalid option" << endl;
                continue;
            }
        }
        else if (input == "no"){
            return "";
        }
        else {
            cout << "Must enter 'yes' or 'no'" << endl;
            continue;
        }

        for (int i = 0; i < size; i++){
            if (chosenGenre == i){
                chosenGenreStr = genres[i];
            }
        }
        delete[] genres;
        return chosenGenreStr;
    }
}

void createSongVectors(string chosenGenre, SongArrayList* songList){
    for (int j = 0; j < songList->size; j++){
        if (songList->songs[j].genre == chosenGenre){
            genres.push_back(songList->songs[j].genre);
            artists.push_back(songList->songs[j].artist);
            titles.push_back(songList->songs[j].title);
            trackids.push_back(songList->songs[j].trackid);
            popularity.push_back(songList->songs[j].popularity);
        }
    }
}

void setNavigation(int startIndex, int endIndex, string chosenGenre, SongArrayList* songList, bool next){
    for (int i = 0; i < songList->size; i++){
        if (i >= startIndex && i < endIndex){
            cout << songList->songs[i].genre << ", " << songList->songs[i].artist << ", " << songList->songs[i].title << ", " << songList->songs[i].trackid << ", " << songList->songs[i].popularity << endl;
        }
    }
}

void playLoop(LinkListNode* uniqueLinkList, SongArrayList* songList){
    string chosenGenre = filterPrompt(uniqueLinkList);
    int startIndex = -10;
    int endIndex = 0;
    int startGenreIndex = -10;
    int endGenreIndex = 0;
    int loopCount = 0;
    while (true){
        char navKey = '\0';
        if (chosenGenre != ""){
            cout << "Navigation: n for next, p for previous, q to quit, s for start, e for end, r for reset filter" << endl;
            cin >> navKey;
            if (navKey != 'n' && navKey != 'p' && navKey != 'q' && navKey != 's' && navKey != 'e' && navKey != 'r'){
                cout << "Invalid option" << endl;
                continue;
            }
        }
        else {
            cout << "Navigation: n for next, p for previous, q to quit, s for start, e for end, f for filter" << endl;
            cin >> navKey;
            if (navKey != 'n' && navKey != 'p' && navKey != 'q' && navKey != 's' && navKey != 'e' && navKey != 'f'){
                cout << "Invalid option" << endl;
                continue;
            }
        }
        
        if (loopCount == 0){
            createSongVectors(chosenGenre, songList);
        }

        if (navKey == 'n') {
            if (chosenGenre != "") {
                int origEndGenreIndex = 0;
                if (startGenreIndex + 10 >= genres.size()){
                    cout << "No more songs to display." << endl;
                    continue;
                }
                if (endGenreIndex + 10 > genres.size()){
                    if (endGenreIndex == genres.size()){
                        origEndGenreIndex = endGenreIndex - 10;
                    }
                    else {
                        origEndGenreIndex = endGenreIndex;
                    }
                    
                    endGenreIndex = genres.size();
                    startGenreIndex = origEndGenreIndex;
                }
                else {
                    origEndGenreIndex = endGenreIndex;
                    endGenreIndex += 10;
                    startGenreIndex = origEndGenreIndex;
                }

                // cout << startGenreIndex << ", " << endGenreIndex << endl;
                for (int i = startGenreIndex; i < endGenreIndex; i++){
                    cout << genres[i] << ", " << artists[i] << ", " << titles[i] << ", " << trackids[i] << ", " << popularity[i] << endl;
                }
            } 
            else {
                int origEndIndex = 0;
                if (startIndex + 10 >= songList->size){
                    cout << "No more songs to display." << endl;
                    continue;
                }
                if (endIndex + 10 > songList->size){
                    origEndIndex = endIndex - 10;
                    endIndex = songList->size;
                    startIndex = origEndIndex;
                }
                else {
                    origEndIndex = endIndex;
                    endIndex += 10;
                    startIndex = origEndIndex;
                }
                
                // cout << startIndex << ", " << endIndex << endl;
                setNavigation(startIndex, endIndex, chosenGenre, songList, true);
            }
        } 
        else if (navKey == 'p') {
            if (chosenGenre != ""){
                if (startGenreIndex <= 0){
                    cout << "No previous songs." << endl;
                    continue;
                }
                if (startGenreIndex > 0 && (startGenreIndex - 10 <= 0)){
                    endGenreIndex = startGenreIndex;
                    startGenreIndex = 0;
                }
                else {
                    startGenreIndex -= 10;
                    endGenreIndex -= 10;
                }

                // cout << startGenreIndex << ", " << endGenreIndex << endl;
                for (int i = startGenreIndex; i < endGenreIndex; i++){
                    cout << genres[i] << ", " << artists[i] << ", " << titles[i] << ", " << trackids[i] << ", " << popularity[i] << endl;
                }
            }
            else {
                if (startIndex <= 0){
                    cout << "No previous songs." << endl;
                    continue;
                }
                if (startIndex > 0 && (startIndex - 10 <= 0)){
                    endIndex = startIndex;
                    startIndex = 0;
                }
                else {
                    startIndex -= 10;
                    endIndex -= 10;
                }

                // cout << startIndex << ", " << endIndex << endl;
                setNavigation(startIndex, endIndex, chosenGenre, songList, false);
            }
        } 
        else if (navKey == 's') {
            if (chosenGenre != ""){
                startGenreIndex = 0;
                endGenreIndex = 10;
                if (endGenreIndex > genres.size()){
                    endGenreIndex = genres.size();
                }

                // cout << startGenreIndex << ", " << endGenreIndex << endl;
                for (int i = startGenreIndex; i < endGenreIndex; i++){
                    cout << genres[i] << ", " << artists[i] << ", " << titles[i] << ", " << trackids[i] << ", " << popularity[i] << endl;
                }
            }
            else {
                startIndex = 0;
                endIndex = 10;

                // cout << startIndex << ", " << endIndex << endl;
                setNavigation(startIndex, endIndex, chosenGenre, songList, true);
            }
        } 
        else if (navKey == 'q') {
            break;
        } 
        else if (navKey == 'e') {
             if (chosenGenre != ""){
                startGenreIndex = genres.size() - 10;
                if (startGenreIndex < 0){
                    startGenreIndex = 0;
                }
                endGenreIndex = genres.size();

                // cout << startGenreIndex << ", " << endGenreIndex << endl;
                for (int i = startGenreIndex; i < endGenreIndex; i++){
                    cout << genres[i] << ", " << artists[i] << ", " << titles[i] << ", " << trackids[i] << ", " << popularity[i] << endl;
                }
            }
            else {
                if (songList->size <= 10){
                    startIndex = 0;
                    endIndex = songList->size;
                }
                else {
                    startIndex = songList->size - 10;
                    endIndex = songList->size;
                }
                // cout << startIndex << ", " << endIndex << endl;
                setNavigation(startIndex, endIndex, chosenGenre, songList, true);
            }
        } 
        else if (navKey == 'r') {
            chosenGenre = "";
            startIndex = -10;
            endIndex = 0;
            startGenreIndex = -10;
            endGenreIndex = 0;
            genres.resize(0);
            artists.resize(0);
            titles.resize(0);
            trackids.resize(0);
            popularity.resize(0);
            loopCount = -1;
        } 
        else if (navKey == 'f') {
            chosenGenre = filterPrompt(uniqueLinkList);
            startIndex = -10;
            endIndex = 0;
            startGenreIndex = -10;
            endGenreIndex = 0;
            createSongVectors(chosenGenre, songList);
        } 
        loopCount++;
    }
}

int main(){
    SongArrayList* songList = new SongArrayList;
    songList->songs = new Song;
    songList->capacity = 100;
    songList->size = 0;

    readInSongs(songList);
    // printSongList(songList, 1);

    LinkListNode* uniqueLinkList = new LinkListNode;
    uniqueLinkList->data = new GenreInfo;
    uniqueLinkList->next = nullptr;

    generateUniqueGenres(&uniqueLinkList, songList);
    printUniqueLinkList(uniqueLinkList);

    playLoop(uniqueLinkList, songList);

    delete[] songList;
    delete[] uniqueLinkList;

    return 0;
}