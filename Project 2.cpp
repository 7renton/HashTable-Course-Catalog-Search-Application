//============================================================================
// Author      : Trenton Grossheim
// Version     : 1.0
// Description : Project 2 HashTable Application
//============================================================================

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

string csv_path = "CS 300 ABCU_Advising_Program_Input.csv";
unsigned int TABLE_SIZE = 199; //size of the HashTable Vector

struct Course {
    string courseId; // unique identifier
    string title;
    vector<string> prereqs;
};

//============================================================================
// Hash Table class definition
//============================================================================

class HashTable {
private:
    //structure to hold courses, and pointer for when there is collisions
    struct Node {
        Course course;
        Node *next = nullptr;

        //Constructir
        Node(){}
        // initialize with a bid constructor
        Node(Course aCourse) : Node() {
            course = aCourse;
        }
    };

    vector<Node*> vectr; //Collisions will be stored at the same index

    vector<string> selection; //used as a means to manipulate data

    unsigned int hash(string key); //private because user doesn't need access

public:
    HashTable();
    ~HashTable();
    int Load(string csvPath);
    void Insert(Course course);//this will add a course into the table
    void PrintCourse(Course course);// this will print 1 course
    void PrintCourseDetailed(Course course);// this will print 1 course detailed
    void printSelection(); //this will print all courses that are selected
    void clearSelection(); //this will set selected courses to none
    void SelectAll(); //this will select all courses
    void SelectSearch(string searchStr); //selects all courses that can contain the substring searchStr
    void SortSelection(); // sorts the selected courses alphanumerically
    Course* GetCourse(string courseId); // simple getter function
};

/**
 * Constructor
 */
HashTable::HashTable() {
    vectr.resize(TABLE_SIZE);
}

/**
 * Destructor
 */
HashTable::~HashTable() {
    // FIXME (2): Implement logic to free storage when class is destroyed
    while(vectr.size() > 0){ //loops through vector
        //manipulating variables
        Node* c = vectr.back();
        Node* n;
        while(c != nullptr){ // loops through LinkList
            n = c->next;
            delete c; // have to make sure to delete ever node
            c = n;
        }
         vectr.pop_back();
    }
}

/**
 * Hash Key Function
 * this hash function will return the asci number of the first letter of the courseId
 * plus the number suffix and take that sum over the modulus of the table size
 */
unsigned int HashTable::hash(string key){
    return ((unsigned int)key[0] + stoul(key.substr(key.size()-3))) % TABLE_SIZE;
}

/**
 * Loading a CSV File
 */
int HashTable::Load(string csvPath) {
    ifstream file(csvPath); //open file
    if(file.fail()){ //if failed to open file, error message and return
        cout << "There is no file \"" << csvPath << "\" in this directory." << endl;
        return 0;
    }
    int numLoaded = 0;//allows us to display how many courses where loaded
    string str;
    while(getline(file, str, '\n')){ //get whole line so we have more control over how we divide it
        Course course;

        course.courseId = str.substr(0, str.find(",")); //get first substring set to courseId
        str.erase(0,str.find(",")+1);

        course.title = str.substr(0, str.find(",")); //get second substring set to title
        str.erase(0,str.find(",")+1);

        while(str.size() > 0){  //keep looping until there is no more words seperated by commas
            if(str[0] != ','){ //this is to catch empty spots, or double commas like ",,"
                int i = str.find(",");
                if(i == -1){//if there is no more commas
                    course.prereqs.push_back(str); //this will make the order of the preqeqs the opposite of how they are in the csv file
                    break;                         //there is no easy built in function to add to the front, but hopefully order wont matter too much
                }                                  //can easily be changed later
                else{//else there is more commas
                    course.prereqs.push_back(str.substr(0,i));//whatever is left add
                }
            }
            str.erase(0,str.find(",")+1);//remove first comma, should be in the first position
        }
        numLoaded++;
        Insert(course);
    }
    return numLoaded;
}

/**
 * Inserting Courses Into The Table
 * If I where to further develop this project it might be a good idea
 * to either replace duplicates or manage them somehow. This could lead to
 * errors because the getCourse function only returns the first occurrence,
 * rendering all duplicates inaccessible. I recommend this once removal
 * functions are added to the project
 */
void HashTable::Insert(Course course) {
    Node* n = new Node(course); // have to deallocate this memory
    int index = hash(n->course.courseId); // index to key val
    n->next = vectr[index]; // puts the current list at the end of this
    vectr[index] = n; // sets the vector entry to the new node
}

/**
 * Printing Functions
 */
/*Print a Course~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HashTable::PrintCourse(Course course){
    cout << course.courseId << ",  " << course.title << endl;
}
/*Detailed print includes Prereqs~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HashTable::PrintCourseDetailed(Course course){
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout << "COURSE ID: " << endl << "  *  " << course.courseId << endl << endl;
    cout << "TITLE: " << endl << "  *  " <<course.title << endl << endl;
    cout << "PREREQUISITES: " << endl;
        for(int i = course.prereqs.size()-1; i >= 0; i--){
            cout << "   *  " <<course.prereqs[i] << endl;
        }
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}
/*print all Selected Courses ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HashTable::printSelection(){
    for(int i = 0; i < selection.size(); i++){ //loops through selected courses
        Course* course = GetCourse(selection[i]); //travels through the table to get the course info
        if(course != nullptr){ //if a course was found
            PrintCourse(*course);
        }
    }
}

/**
 * Selection Functions
 */
/*Clears Selection~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HashTable::clearSelection(){
    selection.clear();
}
/*Sets Selection to all courseIds in order~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HashTable::SelectAll(){
    //loads all CourseIds into selection
    for(int i = 0; i < vectr.size(); i++){ // loops through vector
        Node* n = vectr[i]; //manipuaation var
        while(n != nullptr){ // loops through link
            selection.push_back(n->course.courseId); //add to selected courses
            n = n->next;
        }
    }
}
/*Selects all in a department; CS, Math, etc.~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HashTable::SelectSearch(string searchStr){
    int thisManyFound = 0; // allows us to display how many matches where found
    for(int i = 0; i < vectr.size(); i++){ // loops through vector
        Node* n = vectr[i]; //manipuaation var
        while(n != nullptr){ // loops through link
            if(n->course.courseId.find(searchStr)!=string::npos){//if the courseId contains the substring
                selection.push_back(n->course.courseId); // add to selection
                thisManyFound++;
            }
            n = n->next; //loop increment
        }
    }
    cout << thisManyFound << " matches found for \"" << searchStr << "\"" << endl;
}
/*Sorts all that is currently selected~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HashTable::SortSelection(){
    if(selection.size()>1){ // a list less than 1 can't be sorted
        bool finishedWithoutSwap; // this is a bumble sort algorithm
        do{
            finishedWithoutSwap = true;
            for(int i = 0; i < selection.size()-1;i++){ //loop through entire list
                if(selection[i].compare(selection[i+1])>0){ //if left is bigger than right
                    string s = selection[i];                //swap them
                    selection[i] = selection[i+1];
                    selection[i+1] = s;
                    finishedWithoutSwap = false;
                }
            }
        } while(!finishedWithoutSwap); // if it goes through entire list without swapping
    }
}

/**
 * Getter Function
 */
Course* HashTable::GetCourse(string courseId){
    unsigned int index = hash(courseId); // find the index
    Node* n = vectr[index];
    while(n->course.courseId != courseId){ //loop through the list at index
        if(n->next == nullptr){ //if end of list
            cout << "ERROR: " << courseId << " is not in course catalog" << endl;
            return nullptr;
        }
        n = n->next;
    }
    return &n->course;//if one is found it will get this far, and return
}

//============================================================================
// Main
//============================================================================
int main()
{
    HashTable hTable;

    string input; //I used a 2 input variables because you can get errors if you
    int option;   //try to input a letter into an int. And switches only use ints
    while(option != 9){ //while the user doesn't exit
        option = 0;
        //Print menu
        cout << endl << "_____SELECT_OPTION_____" << endl;
        cout << "1) Import from CSV file" << endl;
        cout << "2) Display all courses" << endl;
        cout << "3) Search Courses" << endl;
        cout << "4) Get Course Details" << endl;
        cout << "9) EXIT PROGRAM" << endl;
        cout << " -> ";

        getline(cin,input);
        try {option = stoi(input);}//tries convert the users input to int
        catch(invalid_argument){option = 0;}//if it fails it will default to 0
        cout << endl;
        switch(option){
            case 1: //Load
                cout << "Enter the csv path of the file you would like to load: " << endl << " -> ";
                getline(cin,csv_path);
                cout << hTable.Load(csv_path) << " entries were loaded." << endl;
                break;
            case 2: //Display All; clears, selects all, sorts then prints
                hTable.clearSelection();
                hTable.SelectAll();
                hTable.SortSelection();
                hTable.printSelection();
                break;
            case 3:
                { // Search subString
                string searchStr;
                cout << "Enter a course ID, part of an ID or a department prefix (E.g. CSCI, MATH)" << endl << " -> ";
                getline(cin,searchStr);
                cout << endl;
                hTable.clearSelection();
                hTable.SelectSearch(searchStr);
                hTable.SortSelection();
                hTable.printSelection();
                break;
                }
            case 4:
                {//Get Course
                string courseId;
                cout << "Enter the course ID you would like more details about:" << endl << " -> ";
                getline(cin,courseId);
                cout << endl;
                hTable.PrintCourseDetailed(*hTable.GetCourse(courseId));
                break;
                }
            case 9://Exit
                break;
            default: //Anything else is an invalid input
                cout  << "\"" << input << "\" is not a valid option" << endl;
        }
    }

    return 0;
}
