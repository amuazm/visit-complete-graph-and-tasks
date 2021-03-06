#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#define MYMAX numeric_limits<int>::max()

map<string, map<string, int>> distances;
map<string, vector<string>> tasks;

//trim from start
void ltrim(string& s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {return !isspace(ch); }));
}

//trim from end
void rtrim(string& s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {return !isspace(ch); }).base(), s.end());
}

//trim from both ends
void trim(string& s) {
    ltrim(s);
    rtrim(s);
}

//reads position distances from distances.txt and stores them into the distances map
void readDistances() {
    ifstream fileDistances("distances.txt");
    while (fileDistances.peek() != EOF) {
        char s[256];

        fileDistances.get(s, 256, '-');
        string s2(s);
        trim(s2);

        fileDistances.ignore(1);

        fileDistances.get(s, 256, ':');
        string s3(s);
        trim(s3);

        fileDistances.ignore(1);

        fileDistances.get(s, 256);
        int i = atoi(s);

        distances[s2][s3] = i;
    }
    fileDistances.close();
}

//reads tasks from tasks.txt and stores them into the tasks map
void readTasks() {
    ifstream fileTasks("tasks.txt");
    while (fileTasks.peek() != EOF) {
        char s[256];

        fileTasks.get(s, 256, '-');
        string s2(s);
        trim(s2);

        fileTasks.ignore(1);

        fileTasks.get(s, 256);
        string s3(s);
        trim(s3);

        tasks[s2].push_back(s3);
    }
    fileTasks.close();
}

//prints out the distances map for testing purposes
void printDistances() {
    cout << "\ndistances:\n";
    for (auto elem : distances) {
        cout << elem.first << ": ";
        for (auto elem2 : elem.second) {
            cout << elem2.first << elem2.second << " ";
        }
        cout << "\n";
    }
}

//prints out the tasks map for testing purposes
void printTasks() {
    cout << "tasks:\n";
    for (auto elem : tasks) {
        cout << elem.first << ": ";
        for (string s : elem.second) {
            cout << s << " ";
        }
        cout << "\n";
    }
}

class Robot {
public:
    vector<string> visitHistory;
    vector<string> inventory;
    string currentPos = "";
    int totalDistanceTravelled = 0;

    //require distances and tasks maps when creating object
    Robot() {
        cout << "Robot created.\n";
    }

    //sets position of robot and helps track current situation
    //at the end, immediately moves on to next position
    void setPos(string s) {

        //calculate distance travelled
        if (currentPos != "") {
            totalDistanceTravelled += distances[currentPos][s];
            cout << "\nDistance: " << distances[currentPos][s] << "\n";
        }
        cout << "Total Distance travelled: " << totalDistanceTravelled << "\n";

        currentPos = s;

        //add position to history
        visitHistory.push_back(currentPos);
        //print visit history
        cout << "Visit History: ";
        for (string s : visitHistory) {
            cout << s << " | ";
        }
        cout << "\n";

        //task protocol
        //dropoff tasks
        for (auto it = find(inventory.begin(), inventory.end(), currentPos);
            it != inventory.end();
            it = find(inventory.begin(), inventory.end(), currentPos))
        {
            cout << "Dropping off one parcel...\n";
            inventory.erase(it);
        }
        //pickup tasks
        if (tasks[currentPos].empty()) {
            cout << "Picked up no tasks.\n";
        }
        else {
            //add tasks into inventory and clear out tasks from position
            inventory.insert(inventory.end(), tasks[currentPos].begin(), tasks[currentPos].end());
            cout << "Picked up parcels: ";
            for (string s : tasks[currentPos]) {
                cout << s << " | ";
            }
            cout << "\n";
            tasks[currentPos].clear();
        }
        //print inventory
        cout << "Inventory: ";
        for (string s : inventory) {
            cout << s << " | ";
        }
        cout << "\n";

        nextPos();
    }

    //PATH FUNCTIONS
    //protocol for finding which position to go to next
    void nextPos() {
        if (gotoClosestUnvisited());
        else if (deliverClosestParcel());
        else goHOME();
    }

    //finds the closest unvisited position
    //if found - moves robot to said position and returns true
    //if not found - returns false
    bool gotoClosestUnvisited() {
        //find closest unvisited position
        pair<string, int> closestUnvisited("", MYMAX);
        for (auto elem : distances[currentPos]) { //use the current position's distances map as a reference
            if (find(visitHistory.begin(), visitHistory.end(), elem.first) == visitHistory.end()) { //check if unvisited
                if (elem.second < closestUnvisited.second) { //find the closest
                    closestUnvisited.first = elem.first;
                    closestUnvisited.second = elem.second;
                }
            }
        }
        if (closestUnvisited.first != "") {
            cout << "Going to closest unvisited position.\n";
            setPos(closestUnvisited.first);
            return true;
        }
        else {
            return false;
        }
    }

    //checks inventory's parcels' destinations and finds the closest position to deliver to
    //if found - moves robot to said position and returns true
    //if not found - returns false
    bool deliverClosestParcel() {
        pair<string, int> closestParcelDelivery("", MYMAX);
        if (inventory.empty() == false) {
            for (string parcelDestination : inventory) {
                if (distances[currentPos][parcelDestination] < closestParcelDelivery.second) {
                    closestParcelDelivery.first = parcelDestination;
                    closestParcelDelivery.second = distances[currentPos][parcelDestination];
                }
            }
        }
        if (closestParcelDelivery.first != "") {
            cout << "Delivering closest parcel.\n";
            setPos(closestParcelDelivery.first);
            return true;
        }
        else {
            return false;
        }
    }

    //moves the robot back home if not already at home
    void goHOME() {
        cout << "Jobs done!\n";
        if (currentPos != "HOME") {
            cout << "Going back HOME.\n";
            setPos("HOME");
        }
    }
};

int main()
{
    readDistances();
    readTasks();

    Robot myRobot;
    myRobot.setPos("HOME");

    //for use of executable
    for (string s = ""; s != "1";) {
        cout << "\nTo finish, please enter 1: ";
        cin >> s;
    }
}