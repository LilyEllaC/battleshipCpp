//importing
#include <iostream>
#include <cctype>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <string>
#include <set> 
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <chrono>
#include "battleship.h"

using namespace std;

//#define DEBUG_MODE // Define this macro for debug builds

// Macro definition using a do-while(0) loop for safe multi-statement expansion
#ifdef DEBUG_MODE
    #define DEBUG_PRINT(msg) \
        do { \
            std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " -- " << msg << std::endl; \
        } while (0)
#else
    // If DEBUG_MODE is not defined, the macro expands to nothing
    #define DEBUG_PRINT(msg) do {} while (0)
#endif


//getting each boat to have info
struct boat {
    int shape;
    vector <string> positions;
    int timesHit;
};

//getting the computer to understand previous hits
struct hits{
    bool previous=false;
    vector <string> positionsHit;
    bool stillShooting=false;
    vector <int> shipsSank;
    int direction;
};


std::ostream & operator << (std::ostream & outs, const vector <string> & strvec) {
    for (auto &v :strvec){
        outs <<v << ", ";
    }
    return outs;
}
std::ostream & operator << (std::ostream & outs, const vector <int> & strvec) {
    for (auto &v :strvec){
        outs <<v << ", ";
    }
    return outs;
}
std::ostream & operator << (std::ostream & outs, const hits & hit) {
    return outs << "previous: " << hit.previous 
            << "\n positionsHit " << hit.positionsHit 
            << "\n stillShooting " << hit.stillShooting 
            << "\n shipsSank " << hit.shipsSank
            << "\n direction " << hit.direction;
}


//functions
//getting the possible locations
vector <string> getFromFile(string fileName){
    vector <string> position;       
    ifstream file(fileName);    
    string word;                   
    
    // Read each word from file and add to set
    while (file >> word) {
        position.push_back(word);             
    }
    return position;  
}

//push to file
void pushToFile(string word, string fileName){
    ofstream file;
    file.open(fileName, std::ios::app);
    file<<word<<endl;
}

//push to file (multiple words)
void pushToFile(string word1, string word2, string fileName){
    ofstream file;
    file.open(fileName, std::ios::app);
    file<<word1<<", "<<word2<<endl;
    file.close();
}

//UNIVERSAL variable:):):):):):):):):):):):):)
const vector <string> POSSIBLES=getFromFile("positions.txt");

//finding
bool isInString(vector <string> list, string word){
    for (int i=0; i<list.size(); i++){
        if (list[i]==word){
            return true;
        }
    }
    return false;
}

//finding fo ints
bool isInInt(vector <int> list, int word){
    for (int i=0; i<list.size(); i++){
        if (list[i]==word){
            return true;
        }
    }
    return false;
}

//finding position
int positionInString(vector <string> list, string word){
    for (int i=0; i<list.size(); i++){
        if (list[i]==word){
            return i;
        }
    }
    return -1;
}

//finding position
int positionInInt(vector <int> list, int word){
    for (int i=0; i<list.size(); i++){
        if (list[i]==word){
            return i;
        }
    }
    return -1;
}

//setting to lowercase letters
void lowering(string &word){
    for (int i=0; i<word.length(); i++){
        word[i]=tolower(word[i]);
    }
}

//getting names hidden so they can't find it in the code
string encoding(string word){
    //encoding
    //going through all letters
    for (int i=0; i<word.length(); i++){
        word[i]=toupper(word[i]);
        //changing
        if (word[i]<86){
            word[i]+=5;
        } else {
            word[i]-=21;
        }
    }
    return word;
}

//setting letter positions and number positions
void settingCoords(string position, char &letterCoords, int &numberCoords){
    letterCoords=position[0];
    numberCoords=stoi(position.substr(1, position.length()-1));
}

//getting the board to look right
// shots is a length 100 vector containing what to cout at each posistion
void board(vector <string> shots, vector <string> boats, hits pastHits){
    
    cout<<"\n\033[0;1m|------------------------------------------------------|";
    printf("\n|    | A  | B  | C  | D  | E  | F  | G  | H  | I  | J  |");

    for (int i=0; i<100; i+=10){
        cout<<"\n|------------------------------------------------------|\n| "<<i/10+1;
        if (i!=90){
            cout<<" ";
        }
        cout<<" | ";
        for (int j=0; j<10; j++){
            //printf("%1s%1s]\033[0;1m | ", shots[i+j].c_str(),boats[i+j].c_str());
            cout<<shots[i+j]<<boats[i+j]<<"\033[0;1m | ";
        }
        //showing which boats have been sunk
        if (i==20&&isInInt(pastHits.shipsSank, 2)){
            cout<<"     2 boat sunk";
        } else if (i==40&&isInInt(pastHits.shipsSank, 3)){
            cout<<"     3 boat sunk";
        } else if (i==60&&isInInt(pastHits.shipsSank, 4)){
            cout<<"     4 boat sunk";
        } else if (i==80&&isInInt(pastHits.shipsSank, 5)){
            cout<<"     5 boat sunk";
        }
    }
    cout<<"\n|------------------------------------------------------|\n";
    
}

//seeing if the spot the person want to put the boat is allowed
bool spotAllowed (string position1, string position2, int boatNum, vector <string> possible2s, vector <string> positions){
    bool rightDistance=false;
    if (isInString(possible2s, position2)){
        rightDistance=true;
        if (!isInString(positions, position2)){
            rightDistance=true;
        } else {
            rightDistance=false;
        }
    } 
    
    
    //retuning
    return rightDistance;

}

//filling the vectors
void settingUp (vector <string> &shots, vector <string> &compShots, vector <string> &boats, vector <string> &emptyBoats, vector <string> &compBoats){
    for (int i=0; i<100; i++){
        shots.push_back("\033[1;32m "); // green
        compShots.push_back("\033[1;32m "); // green
        boats.push_back(" ");
        emptyBoats.push_back(" ");
        compBoats.push_back(" ");
    }
}

//finding possible second positions
vector <string> possiblePosition2(string position1, int boatNum, bool checking){
    vector <string> possible2s;
    string position2;
    char letterCoord1, letterCoord2;
    int numberCoord1, numberCoord2;
    //setting my vairables to see where in the list of possibles posible 1 and 2 are
    int coordinate1;
    int coordinate2;

    //setting the positions
    settingCoords(position1, letterCoord1, numberCoord1);

    for (int i=0; i<100; i++){
        position2=POSSIBLES[i];
        //dealing with placement variables
        settingCoords(position2, letterCoord2, numberCoord2);
        coordinate1= positionInString(POSSIBLES, position1);
        coordinate2= positionInString(POSSIBLES, position2);
        // knowing the length and order of the list of possibles, we can find out where the possible 2nd positions are
        if (coordinate1-coordinate2==boatNum-1||coordinate1-coordinate2==-boatNum+1||coordinate1-coordinate2==boatNum*10-10||coordinate1-coordinate2==-boatNum*10+10){
            //stopping it from being a column over (ex A10-B1)
            if (letterCoord1==letterCoord2||numberCoord1==numberCoord2){
                possible2s.push_back(position2);
            }
        } 
    }
    //making the spots with nothing beside it still have a place so the computer can be smart
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (checking){
        //right
        if (coordinate1<10*(boatNum-1)){
            possible2s.insert(possible2s.begin(), "Z20");
        }
        //up
        if (coordinate1%10<0+boatNum-1){
            possible2s.insert(possible2s.begin()+1, "Z20");
            //down
        } else if (coordinate1%10>=10-boatNum+1){
            possible2s.insert(possible2s.begin()+2, "Z20");
        }
        //left
        if (coordinate1>100-10*(boatNum-1)){
            possible2s.insert(possible2s.begin()+3, "Z20");
        } 
    }
#ifdef DEBUG_MODE
    for (int i=0; i<possible2s.size(); i++){
        cout<<possible2s[i];
    }
#endif
    return possible2s;
}

//finding the positions between
void findBetween(string position1, string position2, int boatNum, vector <string> &newPositions){
    //variables
    char letterCoord1, letterCoord2;
    int numberCoord1, numberCoord2;

    //setting the positions
    settingCoords(position1, letterCoord1, numberCoord1);
    settingCoords(position2, letterCoord2, numberCoord2);
    newPositions.push_back(position1);
    newPositions.push_back(position2);

    for (int i=0; i<boatNum-2; i++){
        //checking if it is vertical or horizontal
        if (letterCoord1==letterCoord2){
            //going up or down
            if (numberCoord1>numberCoord2){
                newPositions.push_back(POSSIBLES[positionInString(POSSIBLES, position1)-(i+1)]);
            } else {
                newPositions.push_back(POSSIBLES[positionInString(POSSIBLES, position1)+(i+1)]);
            }
        } else {
            //horizontal
            //left or right
            if (letterCoord1>letterCoord2){
                newPositions.push_back(POSSIBLES[positionInString(POSSIBLES, position1)-(i+1)*10]);
            } else {
                newPositions.push_back(POSSIBLES[positionInString(POSSIBLES, position1)+(i+1)*10]);
            }
        }
    }
}

//adding the positions of the ships to the list
void addingPos(vector <string> &positions, string position1, string position2, int boatNum, bool &works, boat &boatStuff){
    vector <string> newPositions;
    bool goodSpot=true;

    //getting the in between positions
    findBetween(position1, position2, boatNum, newPositions);

    //making sure none are taken
    for (int i=0; i<newPositions.size(); i++){
        if (isInString(positions, newPositions[i])){
            goodSpot=false;
            works=false;
        }
    }

    //adding them to the list
    if (goodSpot){
        for (int i=0; i<newPositions.size(); i++){
            positions.push_back(newPositions[i]);
            boatStuff.positions.push_back(newPositions[i]);
        }
        works=true;
    }
}

//making the boats have images
// positions is the location of all things(boats/shots) that you want to change the image to the 'shape' value
void boatPictures(vector <string> &boatImages, vector <string> positions, string shape){
    char letterCoord;
    int numberCoord;
    DEBUG_PRINT("in boatPictures" << positions);

    for (int i=0; i<positions.size(); i++){
        //setting the positions
        if (positions[i]!="Z20"){
            if (positions[i]==""){
                //break
                cout<<"Error, please restart.";
            }
            settingCoords(positions[i], letterCoord, numberCoord);
            boatImages[(numberCoord)*10-10+letterCoord-'A']=shape;
        }
    }
}

//getting the positions
void gettingBoatPlaces(vector <string> &positions, vector <int> boats, vector <string> &boatImages, vector <string> &shots, vector <boat> &boatStuff, hits pastHits){
    int boatNum;
    string position1, position2="0";
    bool works=true;
    vector <string> position2s;
    board(shots, boatImages, pastHits);
    for (int i=0; i<4; i++){
        boatStuff.push_back(boat());
        do{
            cout<<"\nWhich boat do you want to place? (";
            // print out boat options
            for (int j=0; j<boats.size(); j++){
                cout<<boats[j];
                
                if (j!=boats.size()-1){
                    cout<<", ";
                }
            }
            cout<<"): ";
            cin>>boatNum;
            cin.clear();
            //making sure that the boat is a possible length
            while (!isInInt(boats, boatNum)){
                //stops the game from breaking
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    cin>>ws;
                }
                if (!isInInt(boats, boatNum)){
                    cout<<"That isn't an option. Please enter a possible boat: ";
                    cin>>boatNum;
                }
            }
            boatStuff[i].shape=boatNum;
            boatStuff[i].timesHit=0;


            //getting positions
            do {
                cout<<"What is the starting coordinate for your "<<boatNum<<" boat? ";
                cin>>position1;
                position1[0]=toupper(position1[0]);
                while (!isInString(POSSIBLES, position1)||isInString(positions, position1)){
                    cout<<"That isn't a possible answer. \nPlease use format 'A1', and boats cannot be on top of each other: ";
                    cin>>position1;
                    position1[0]=toupper(position1[0]);
                }

                //second spot
                cout<<"What is the ending coordinate for your "<<boatNum<<" boat? (enter -1 to change your starting position): ";
                cin>>position2;
                position2[0]=toupper(position2[0]);
                //making sure it is possible
                while ((!isInString(POSSIBLES, position2)||isInString(positions, position2))&&position2!="-1"){
                    cout<<"That isn't a possible answer. \nPlease use format 'A1': ";
                    cin>>position2;
                    position2[0]=toupper(position2[0]);
                }
                position2s=possiblePosition2(position1, boatNum, false);
                while (!spotAllowed(position1, position2, boatNum, position2s, positions)){
                    cout<<"\nThat spot is not the right disatnce away from your starting position. \nYour options are: ";
                    for (int j=0; j<position2s.size(); j++){
                        if (!isInString(positions, position2s[j])&&position2s[j]!="Z20"){
                            cout<<position2s[j]<<" ";
                        }
                    }
                    cout<<"\nPlease enter your ending position: ";
                    cin>>position2;
                    position2[0]=toupper(position2[0]);
                } 
                //allowing to change starting position
            } while (position2=="-1");
            
            //adding/checking positions
            addingPos(positions, position1, position2, boatNum, works, boatStuff[i]);
            if (!works){
                cout<<"\nSorry, your boat couldn't be in one of the places you wanted it to be. \nPlease try again.";
            }
        } while (!works);
        //getting a record
        pushToFile(position1, position2, "boatPlacements.txt");
        //getting rid of the placed boats
        boats.erase(boats.begin()+positionInInt(boats, boatNum));
        //adding images to list etc, etc
        system("clear");
        boatPictures(boatImages, positions, "$");
        board(shots, boatImages, pastHits);
    }
}

//getting the actual boat positions
void getBoatPos(string &position1, vector <string> &positions, int boatNum, string &position2){
    // getting position1
    while (isInString(positions, position1)){
        position1 = POSSIBLES[rand() % 100];
    }

    // getting the possibles
    vector <string> possible2s = possiblePosition2(position1, boatNum, false);
    while (possible2s[0] == "Z20" && possible2s[1] == "Z20" && possible2s[2] == "Z20" && possible2s[3] == "Z20"){
        while (isInString(positions, position1)){
            position1 = POSSIBLES[rand() % 100];
        }
        possible2s = possiblePosition2(position1, boatNum, false);
    }
    // second spot
    position2 = possible2s[rand() % possible2s.size()];
    int stop = 0;
    while ((!spotAllowed(position1, position2, boatNum, possiblePosition2(position1, boatNum, false), positions)||position2 == "Z20")&&stop<possible2s.size()){
        position2 = possible2s[stop];
        stop++;
    }
} 

//computer positions
void computerBoats(vector <string> &positions, vector <int> boats, vector <string> &boatImages, vector <string> shots, vector <boat> &boatStuff, hits pastHits){
    int boatNum;
    string position1, position2;
    bool works;
    for (int i=0; i<4; i++){
        boatStuff.push_back(boat());
        boatStuff[i].timesHit=0;
        //setting up the boats
        boatNum=i+2;
        boatStuff[i].shape=boatNum;

        //getting rid of the used boats
        boats.erase(boats.begin()+positionInInt(boats, boatNum));

        do{
            position1 = POSSIBLES[rand() % 100];
            getBoatPos(position1, positions, boatNum, position2);
            //adding positions to list
            addingPos(positions, position1, position2, boatNum, works, boatStuff[i]);            

        } while (!works);
        boatPictures(boatImages, positions, "$");
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    board(shots, boatImages, pastHits);
}

//checking if all possible 2 boat spots are full
bool allSpotsFull(string shot, vector <string> placesShot, vector <string> positions, string position1, string position2){
    int numOptions=0;
    for (int i=0; i<100; i++){
        position1 = POSSIBLES[i];
        getBoatPos(position1, positions, 2, position2);
        if (!isInString(placesShot, position1)){
            numOptions++;
        }
    }
    if (numOptions>2){
        return false;
    } else {
        cout<<"You got me!";
        return true;
    }
}

// computer 2 boat
void boat2(string shot, vector <string> placesShot, vector <string> &boatPositions, vector <boat> &boatStuff, hits pastHits, vector <string> &boatVisuals){
    string position1, position2; 
    string initialPosition1=boatPositions[0], initialPosition2=boatPositions[1];
    if (isInString(boatStuff[0].positions, shot)){
        cout<<"Moving";
        do {
            position1 = POSSIBLES[rand() % 100];
            getBoatPos(position1, boatPositions, 2, position2);
            //while the position1 has been shot, position 2 has been shot, 
        } while ((isInString(placesShot, position1)||isInString(placesShot, position2))&&!allSpotsFull(shot, placesShot, boatPositions, position1, position2));
        if (allSpotsFull(shot, placesShot, boatPositions, position1, position2)){
            position1=initialPosition1;
            position2=initialPosition2;
        }
        boatPositions[0]=position1;
        boatPositions[1]=position2;
        boatStuff[0].positions[0]=position1;
        boatStuff[0].positions[1]=position2;
        int check1=positionInString(POSSIBLES, initialPosition1), check2=positionInString(POSSIBLES, initialPosition2), check3=positionInString(POSSIBLES, position1), check4=positionInString(POSSIBLES, position2);
        boatPictures(boatVisuals, {initialPosition1, initialPosition2}, " ");
        boatPictures(boatVisuals, {position1, position2}, "$");
        cout<<"done";
    }
}

//making the computer be smart
void computerSmart(vector <string> placesShot, hits &pastHits, string &shot){
    vector <string> possibleInitialBoatSpots=possiblePosition2(pastHits.positionsHit[0],2, true);
    vector <string> possibleNextBoatSpots=possiblePosition2(pastHits.positionsHit[pastHits.positionsHit.size()-1],2, true);
    if (pastHits.positionsHit.size()==1&&!isInString(placesShot, possibleInitialBoatSpots[0])&&possibleInitialBoatSpots[0]!="Z20"){
        shot=possibleInitialBoatSpots[0];
        pastHits.direction=0;
        //previous not a hit, current not a shot, not an error
    } else if (!isInString(pastHits.positionsHit, possibleInitialBoatSpots[0])&&!isInString(placesShot, possibleInitialBoatSpots[1])&&possibleInitialBoatSpots[1]!="Z20"){
        shot=possibleInitialBoatSpots[1];
        pastHits.direction=1;
        //preivious not hits, current not a shot, not an error
    } else if (!isInString(pastHits.positionsHit, possibleInitialBoatSpots[0])&&!isInString(pastHits.positionsHit, possibleInitialBoatSpots[1])&&!isInString(placesShot, possibleInitialBoatSpots[2])&&possibleInitialBoatSpots[2]!="Z20"){
        shot=possibleInitialBoatSpots[2];
        pastHits.direction=2;
    }  else if (!isInString(pastHits.positionsHit, possibleInitialBoatSpots[0])&&!isInString(pastHits.positionsHit, possibleInitialBoatSpots[1])&&!isInString(pastHits.positionsHit, possibleInitialBoatSpots[2])&&!isInString(placesShot, possibleInitialBoatSpots[3])&&possibleInitialBoatSpots[3]!="Z20"){
        shot=possibleInitialBoatSpots[3];
        pastHits.direction=3;

    //if previous was a hit (=second hit)
    } else //if (pastHits.previous)
    {   //swapping direction if they miss
        if (!pastHits.previous){
            //possibleNextBoatSpots=possibleInitialBoatSpots;
            if (pastHits.direction==0){
                pastHits.direction=3;
            } else if (pastHits.direction==1){
                pastHits.direction=2;
            } else if (pastHits.direction==2){
                pastHits.direction=1;
            } else if (pastHits.direction==3){
                pastHits.direction=0;
            }
            shot=possibleInitialBoatSpots[pastHits.direction];
        } else{
            shot=possibleNextBoatSpots[pastHits.direction];
        }

        //stopping them from having already shot there
        if (isInString(placesShot, shot)){
            if (pastHits.direction==0){
                pastHits.direction=3;
            } else if (pastHits.direction==1){
                pastHits.direction=2;
            } else if (pastHits.direction==2){
                pastHits.direction=1;
            } else if (pastHits.direction==3){
                pastHits.direction=0;
            }
            shot=possibleInitialBoatSpots[pastHits.direction];
        } 

        //error checking
        while (shot=="Z20"||isInString(placesShot, shot)){
            pastHits.direction=rand()%4;
            shot=possibleInitialBoatSpots[pastHits.direction];
        }
    }
    //error checking
    if (shot==""){
        cout<<"Error. Sorry :(";
    }
}

//doing stuff when the computer has hit more boats than it has sunk
void findExtraHit(hits &pastHits, int shipSize){
    string lastHit=pastHits.positionsHit[pastHits.positionsHit.size()-1];
    string boatEndPos;
    vector <string> boatPos;
    int lastHitPos, hitPos, direction;
    //seeing which came before/after
    lastHitPos=positionInString(POSSIBLES, lastHit);
    hitPos=positionInString(POSSIBLES, pastHits.positionsHit[0]);
    //getting the direction that the boats are in
    if (hitPos<lastHitPos){
        //is up
        if (lastHitPos-hitPos<10){
            direction=1;
        //is left
        } else {
            direction=0;
        }
    //if previous are down or right
    } else {
        //is down
        if (hitPos-lastHitPos<10){
            direction=2;
        //is right
        } else {
            direction=3;
        }
    }
    //getting the spots to erase
    vector <string> errorChecking=possiblePosition2(lastHit, shipSize, true);
    boatEndPos=possiblePosition2(lastHit, shipSize, true)[direction];
    findBetween(lastHit, boatEndPos, shipSize, boatPos);
    //erasing
    for (int i=0; i<boatPos.size(); i++){
        if (positionInString(pastHits.positionsHit, boatPos[i])!=-1){
            pastHits.positionsHit.erase(pastHits.positionsHit.begin()+positionInString(pastHits.positionsHit, boatPos[i]));
        }
    }
}

//to see if it has hit other on sunken boats
void reportShipSank(hits &pastHits, int shipSize){
    DEBUG_PRINT("ship sank" << shipSize <<pastHits );
    //int boatVolume=0;
    pastHits.shipsSank.push_back(shipSize);
    /*
    for (int j=0; j<pastHits.shipsSank.size(); j++){
        boatVolume=pastHits.shipsSank[j];
    }*/

    //checking that the hits are equal to the size of the ship
    if (pastHits.positionsHit.size()==shipSize){
        pastHits.positionsHit.clear();
        pastHits.stillShooting=false;
    } else {
        findExtraHit(pastHits, shipSize);
    }
    DEBUG_PRINT("ship sank end" << shipSize <<pastHits );

}

//checking if the shot hits
string isHit(vector <string> boatPositions, vector <string> &shots, string shot, vector <boat> &boatStuff, int &shipsSunk, hits &pastHits){
    shots.push_back(shot);
    if (isInString(boatPositions, shot)){
        cout<<"\033[0;91mHIT!";
        pastHits.previous=true;
        pastHits.positionsHit.push_back(shot);
        pastHits.stillShooting=true;
        for (int i=0; i<boatStuff.size(); i++){
            if (isInString(boatStuff[i].positions, shot)){
                boatStuff[i].timesHit+=1;
                if (boatStuff[i].timesHit==boatStuff[i].shape){
                    cout<<" You sunk my "<<boatStuff[i].shape<<" long battleship! :(";

                    //adding that a ship has been sank
                    reportShipSank(pastHits, boatStuff[i].shape);
                    shipsSunk+=1;
                }
            }
        }

        //returning the right thing
        return "\033[0;91mX";
    } else {
        cout<<" Miss";
        pastHits.previous=false;
        return "\033[0;93mO";
    }
   
}

//easy mode
void easy(vector <string> positions, string &shot, vector <string> order, string mode){
    while (isInString(positions, shot)&&mode=="easy"){
        shot=order[rand()%50];
    }
}

//getting shots
void shooting(vector <string> &positions, vector <string> &shots, string person, vector <string> &placesShot, vector <string> &boats, vector <boat> &boatStuff, int &shipsSunk, hits &pastHits, int &orderNum, vector <string> order, string mode, vector <string> &boatVisuals){
    string shot;
    bool isUser=false;
    //seeing who it is
    if (person=="user"){
        isUser=true;
    }
    //getting shots
    if (isUser){
        cout<<"\033[0;95mPlease enter a coordinate to shoot: ";
        cin>>shot;
        shot[0]=toupper(shot[0]);
        while (!isInString(POSSIBLES, shot)){
            cout<<"Please enter an actual coordinate to shoot (format is A1 or a1): ";
            cin>>shot;
            shot[0]=toupper(shot[0]);
        }
    } else {
        //making the computer be smart
        if (pastHits.stillShooting==true){
            computerSmart(placesShot, pastHits, shot);
            easy(positions, shot, order, mode);
        } else {
            //choosing where to shoot
            shot=order[rand()%50];
            orderNum++;
            easy(positions, shot, order, mode);
            if (shot==""){
                cout<<"?";
            }
        }
    }
    //making sure they haven't already shot there
    while (isInString(placesShot, shot)){
        if (isUser){
            do {
                cout<<"Please enter a NEW coordinate to shoot: ";
                cin>>shot;
                shot[0]=toupper(shot[0]);
            } while (!isInString(POSSIBLES, shot));
        } else {
            shot=order[rand()%50];
            orderNum++;
            easy(positions, shot, order, mode);
            //more error checking (this error was annoying)
            if (shot==""){
                cout<<"?";
            }
        }
    }
    //doing user stuff
    if (isUser){
        system("clear");
        if (mode=="hard"){
            boat2(shot, placesShot, positions, boatStuff, pastHits, boatVisuals);
        }
        pushToFile(shot, "shots.txt");
    }
    placesShot.push_back(shot);
    //checking for errors
    if (shot==""){
        cout<<"?";
    }
    string wasHit=isHit(positions, shots, shot, boatStuff, shipsSunk, pastHits);
    boatPictures(shots, {shot}, wasHit);
    board(shots, boats, pastHits);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

//normal shooting stuff
void normalShooting(vector <string> positions, vector <string> &shots, vector <string> &placesShot, vector <string> &boats, vector <boat> &boatStuff, int &shipsSunk, hits &pastHits, int &orderNum, vector <string> order){
    string shot;
    int go=rand()%4;
    //making the computer be smart
    if (pastHits.stillShooting==true){
        computerSmart(placesShot, pastHits, shot);
    } else if (go!=0||isInString(positions, placesShot[placesShot.size()-1])){
        shot=order[rand()%100];
    } else {
        //choosing where to shoot
        shot=positions[orderNum];
        orderNum++;
    }
    //making sure they haven't already shot there
    while (isInString(placesShot, shot)){
        if (go!=0||isInString(positions, placesShot[placesShot.size()-1])){
            shot=order[rand()%100];
        } else {
            //choosing where to shoot
            shot=positions[orderNum];
            orderNum++;
        }
    }
    placesShot.push_back(shot);
    string wasHit=isHit(positions, shots, shot, boatStuff, shipsSunk, pastHits);
    boatPictures(shots, {shot}, wasHit);
    board(shots, boats, pastHits);
}

//testing
void randomUserBoat (vector <string> &positions, vector <int> &boats, vector <string> &boatImages, vector <string> shots, vector <boat> &boatStuff, hits pastHits){
    computerBoats(positions, boats, boatImages, shots, boatStuff, pastHits);
}

//getting answer
int preference(string name){
    string animal, dessertFlavour, dessertType, comment, suggestion;
    float rating;
    cout<<"\nThank you for agreeing to participate in this survey. \nYou can enter 0 to quit at any time.";

    //rating
    do {
        cout<<"\nPlease rate the game from 1-10 with 10 being perfect and 1 being never want to play again: ";
        cin>>rating;
    } while ((rating<1||rating>10)&&rating!=0);
    //quitting
    if (rating==0){
        cout<<"Are you sure you want to quit (enter 0 to quit)?";
        cin>>rating;
        if (rating==0){
            return 0;
        }
    }
    //putting to file
    pushToFile(name, to_string(rating), "rating.txt");

    //comments
    cout<<"If you have any comments for improvements please enter them here and I will try to take them into consideration (enter 1 to skip):\n";
    cin>>ws;
    getline(cin, comment);
    
    //quitting
    if (comment=="0"){
        cout<<"Are you sure you want to quit (enter 0 to quit)?";
        cin>>comment;
        if (comment=="0"){
            return 0;
        }
    } 
    //pushing to file
    if (comment!="1"){
        pushToFile(name, comment, "comments.txt");
    }

    //suggestions
    cout<<"If you have any suggestions for improvements please enter them here and I will try to take them into consideration (enter 1 to skip):\n";
    cin>>ws;
    getline(cin, suggestion);
    
    //quitting
    if (suggestion=="0"){
        cout<<"Are you sure you want to quit (enter 0 to quit)?";
        cin>>suggestion;
        if (suggestion=="0"){
            return 0;
        }
    } 
    //pushing to file
    if (suggestion!="1"){
        pushToFile(name, suggestion, "suggestions.txt");
    }

    //moving to fun questions
    cout<<"\nThank you for answering the game related questions. \nNow the questions are just random questions becuse of curiosity.";

    //dogs vs cats
    do {
        cout<<"\nDo you prefer dogs, cats, or other? (if other enter 1): ";
        cin>>animal;
        lowering(animal);
    } while (animal.find("dogs")==-1&&animal.find("cats")==-1&&animal!="0"&&animal!="1");
    //getting other
    if (animal=="1"){
        cout<<"What pet do you prefer? ";
        cin>>ws;
        getline(cin,animal);
    } 
    //putting name in the right file
    if (animal=="0"){
        cout<<"Are you sure you want to quit (enter 0 to quit)?";
        cin>>animal;
        if (animal=="0"){
            return 0;
        }
    } 
    pushToFile(name, animal, "animal.txt");
    

    //dessert flavours
    do {
        cout<<"\nDo you prefer chocolate, vanilla or other? (if other enter 1): ";
        cin>>dessertFlavour;
        lowering(dessertFlavour);
    } while (dessertFlavour.find("chocolate")==-1&&dessertFlavour.find("vanilla")==-1&&dessertFlavour!="1"&&dessertFlavour!="0");
    //getting other
    if (dessertFlavour=="1"){
        cout<<"What flavour do you like? ";
        cin>>dessertFlavour;
    } 
    //quitting 
    if (dessertFlavour=="0"){
        cout<<"Are you sure you want to quit (enter 0 to quit)?";
        cin>>dessertFlavour;
        if (dessertFlavour=="0"){
            return 0;
        }
    }
    //adding to file
    pushToFile(name, dessertFlavour, "dessertFlavours.txt");

    //types of dessert
    do {
        cout<<"\nDo you prefer cupcakes, cookies, or other? (if other enter 1): ";
        cin>>dessertType;
        lowering(dessertType);
    } while (dessertType.find("cupcakes")==-1&&dessertType.find("cookies")==-1&&dessertType!="1"&&dessertType!="0");
    //getting other
    if (dessertType=="1"){
        cout<<"What type of dessert do you like? ";
        cin>>dessertType;
    } 
    //quitting 
    if (dessertType=="0"){
        cout<<"Are you sure you want to quit (enter 0 to quit)?";
        cin>>dessertType;
        if (dessertType=="0"){
            return 0;
        }
    }
    //adding to file
    pushToFile(name, dessertType, "dessertTypes.txt");

    //finishing
    cout<<"\nThank you so much for participating in this survey. ";
    return 0;
}

//playing
void play(int &myWins, int &compWins, const vector <string> &order, double &avgShots, bool &playAgain, string &name, int winStreak){
    //variables
    string location, wantToPlace, wantToReplay, newName, mode="normal";
    int compBattleshipsSunk=0;
    int userBattleshipsSunk=0;
    int orderNum=rand()%50, number=rand()%50;
    vector <string> userBoats, userPositions, computerPositions, userShots, computerShots, userPlacesShot, computerPlacesShot={""}, emptyBoats, computerBoatVisuals;
    vector <string> nameList=getFromFile("names.txt");
    vector <int> boats={2,3,4,5};
    vector <boat> userBoatStuff, computerBoatStuff;
    hits computerPastHits, empty;

    //intro
    cout<<"Hello! Welcome to Battleship!";
    cout<<"\nPlease enter your name: ";
    getline(cin, name);
    lowering(name);
    name[0]=toupper(name[0]);
    newName=encoding(name);

    //getting ready
    settingUp(userShots, computerShots, userBoats, emptyBoats, computerBoatVisuals);
    board(userShots, userBoats, computerPastHits);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //getting the user's boats
    while (wantToPlace!="c"&&wantToPlace!="r"){
        cout<<"\n"<<name<<", would you like to choose your boat placement or have the computer randomly generate them? (c for choosing, r for randomly): ";
        cin>>wantToPlace;
        //wantToPlace="r";
    }
    system("clear");
    while (wantToPlace=="r"){
        userPositions.clear();
        userBoatStuff.clear();
        userShots.clear();
        userBoats.clear();
        settingUp(userShots, computerShots, userBoats, emptyBoats, computerBoatVisuals);
        randomUserBoat(userPositions, boats, userBoats, userShots, userBoatStuff, empty);
        cout<<"\nIf you like this position enter y, otherwise enter 'r' to randoize it: ";
        cin>>wantToPlace;
        system("clear");
        //wantToPlace="y";
    }
    if (wantToPlace=="c"){
        gettingBoatPlaces(userPositions, boats, userBoats, userShots, userBoatStuff, computerPastHits);
    }
    //getting the comuter boats
    computerBoats(computerPositions, boats, computerBoatVisuals, computerShots, computerBoatStuff, computerPastHits);
    system("clear");

    //changing modes
    for (int names=0; names<nameList.size(); names++){
        if (newName.find(nameList[names])!=-1){
            mode="hard";
            cout<<"Hard mode 😁\n";
        }
    }
    if (name.find("Lily")!=-1){
        mode="easy";
        cout<<"easy";
    }

    //shooting
    cout<<"Board to shoot at: ";
    board(userShots, emptyBoats, empty);
    cout<<"\nYour board: ";
    board(computerShots, userBoats, computerPastHits);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //actually shooting
    while (compBattleshipsSunk!=4&&userBattleshipsSunk!=4){
        shooting(computerPositions, userShots, "user", userPlacesShot, emptyBoats, computerBoatStuff, compBattleshipsSunk, empty, number, order, mode, computerBoatVisuals);
        cout<<"\nComputer's guess: ";
        //stuff
        if (mode=="hard"){
            orderNum=0;
            normalShooting(userPositions, computerShots, computerPlacesShot, userBoats, userBoatStuff, userBattleshipsSunk, computerPastHits, orderNum, order);
        } else {
            shooting(userPositions, computerShots, "computer", computerPlacesShot, userBoats, userBoatStuff, userBattleshipsSunk, computerPastHits, orderNum, order, "normal", userBoats);  
        }
    }
    avgShots+=userPlacesShot.size();
    //showing the winner
    if (compBattleshipsSunk==4){
        cout<<"\n\nYou won!\n";
        myWins++;
        winStreak++;
    } else {
        cout<<"\n\nYou lost :(";
        compWins++;
        winStreak=0;
        cout<<"\nThe computer's boats were in these positions: ";
        board(userShots, computerBoatVisuals, empty);
        board(computerShots, userBoats, computerPastHits);
        cout<<"\n";
    }
    usleep(5000000);

    //win streak
    cout<<"\nYour win streak is "<<winStreak;

    //cout<<"\nDo you want to play again "<<name<<"? ";
    //cin>>wantToReplay;
    wantToReplay="n";
    if (tolower(wantToReplay[0])=='y'||wantToReplay=="sure"){
        playAgain=true;
    } else {
        playAgain=false;
    }
}

//main
int main(){
    //time
    auto t1=std::chrono::high_resolution_clock::now();

    //variables
    int myWins=0, compWins=0, numGames=0, winStreak=0;
    bool playAgain=true;
    double avgShots;
    string name, survey;
    vector <string> order=getFromFile("shootingOrder.txt");
    cout<<"Please enter your name: ";
    cin>>name;
    preference(name);
    do {
        srand(time(0));
        play(myWins, compWins, order, avgShots, playAgain, name, winStreak);
        numGames++;
        if (numGames==200){
            playAgain=false;
        }
    } while (playAgain);
    //end time
    auto t2=std::chrono::high_resolution_clock::now();
    auto elapsed_time=std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    int secondsPassed=elapsed_time/1000000;
    int minutesPassed=secondsPassed/60;
    secondsPassed=secondsPassed-minutesPassed*60;

    //showing the stats
    cout<<"\n\nIt took "<<minutesPassed<<"m"<<secondsPassed<<"s to run "<<numGames<<" games.";
    cout<<"\nYou won: "<<myWins<<" games";
    cout<<"\nThe computer won: "<<compWins<<" games";
    cout<<"\nIt took an average of "<<avgShots/numGames<<" shots for somebody to win\n\n";

    //survey
    cout<<"\nThank you for playing battleship, would you like to complete a survey on both your experience and some random topics (y/n)? ";
    cin>>survey;

    if (tolower(survey[0])=='y'){
        preference(name);
    } 
    cout<<"\nHave a good day!";

    //ending
    return 0;
}
