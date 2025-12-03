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
vector <string> getPositions(){
    vector <string> position;       
    ifstream file("positions.txt");    
    string word;                   
    
    // Read each word from file and add to set
    while (file >> word) {
        position.push_back(word);             
    }
    return position;  
}

//UNIVERSAL variable:):):):):):):):):):):):):)
const vector <string> POSSIBLES=getPositions();


//getting the order to shoot
vector <string> getOrder(){
    vector <string> order;       
    ifstream file("shootingOrder.txt");    
    string word;                   
    
    // Read each word from file and add to set
    while (file >> word) {
        order.push_back(word);             
    }
    return order;  
}

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

//setting letter positions and number positions
void settingCoords(string position, char &letterCoords, int &numberCoords){
    letterCoords=position[0];
    numberCoords=stoi(position.substr(1, position.length()-1));
}

//getting the board to look right
// shots is a length 100 vector containing what to cout at each posistion
void board(vector <string> shots, vector <string> boats){
    
    cout<<"\n|------------------------------------------------------|";
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
    }
    cout<<"\n|------------------------------------------------------|";
    
}

//seeing if the spot the person want to put the boat is allowed
bool spotAllowed (string position1, string position2, int boatNum, vector <string> possible2s){
    bool rightDistance=false;
    if (isInString(possible2s, position2)){
        rightDistance=true;
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
        //if (possibles[i]!="Z20"){
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
        //}
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
    vector <string> possibles=getPositions();

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
                newPositions.push_back(possibles[positionInString(possibles, position1)-(i+1)]);
            } else {
                newPositions.push_back(possibles[positionInString(possibles, position1)+(i+1)]);
            }
        } else {
            //horizontal
            //left or right
            if (letterCoord1>letterCoord2){
                newPositions.push_back(possibles[positionInString(possibles, position1)-(i+1)*10]);
            } else {
                newPositions.push_back(possibles[positionInString(possibles, position1)+(i+1)*10]);
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
void gettingBoatPlaces(vector <string> &positions, vector <int> boats, vector <string> &boatImages, vector <string> &shots, vector <boat> &boatStuff){
    int boatNum;
    string position1, position2="0";
    bool works=true;
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
            int i=0;
            while (!isInInt(boats, boatNum)){
                //stops the game from breaking
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                }
                cin>>ws;
                cout<<"That isn't an option. Please enter a possible boat: ";
                cin>>boatNum;
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
                cout<<"What is the ending coordinate for your "<<boatNum<<" boat? (enter -1 to change your starting positio)";
                cin>>position2;
                position2[0]=toupper(position2[0]);
            } while (position2=="-1");

            //making sure it is possible
            while (!isInString(POSSIBLES, position1)||isInString(positions, position2)){
                cout<<"That isn't a possible answer. \nPlease use format 'A1': ";
                cin>>position2;
                position2[0]=toupper(position2[0]);
            }
            while (!spotAllowed(position1, position2, boatNum, possiblePosition2(position1, boatNum, false))){
                cout<<"That spot is not the right disatnce away from your starting position. \nYour options are: ";
                for (int j=0; j<4; j++){
                    cout<<possiblePosition2(position1, boatNum, false)[j]<<" ";
                }
                cout<<"Please enter your ending position: ";
                cin>>position2;
                position2[0]=toupper(position2[0]);
            } 
            //adding/checking positions
            addingPos(positions, position1, position2, boatNum, works, boatStuff[i]);
        } while (!works);

        //getting rid of the placed boats
        boats.erase(boats.begin()+positionInInt(boats, boatNum));
        //adding images to list etc, etc
        system("clear");
        boatPictures(boatImages, positions, "$");
        board(shots, boatImages);
    }
}

//computer positions
void computerBoats(vector <string> &positions, vector <int> boats, vector <string> &boatImages, vector <string> shots, vector <boat> &boatStuff){
    int boatNum;
    string position1, position2;
    vector <string> possible2s;
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
            //getting position1
            position1=POSSIBLES[rand()%100];
            while (isInString(positions, position1)){
                position1=POSSIBLES[rand()%100];
            }

            //getting the possibles
            possible2s=possiblePosition2(position1, boatNum, false);
            while (possible2s[0]=="Z20"&&possible2s[1]=="Z20"&&possible2s[2]=="Z20"&&possible2s[3]=="Z20"){
                while (isInString(positions, position1)){
                    position1=POSSIBLES[rand()%100];
                }
                possible2s=possiblePosition2(position1, boatNum, false);
            }
            //second spot
            position2=possible2s[rand()%possible2s.size()];
            int stop=0;
            while ((!spotAllowed(position1, position2, boatNum, possiblePosition2(position1, boatNum,false))||position2=="Z20")&&stop<4){
                position2=possible2s[stop];
                stop++;
            }
            //adding positions to list
            addingPos(positions, position1, position2, boatNum, works, boatStuff[i]);            

        } while (!works);
        boatPictures(boatImages, positions, "$");
        //board(shots, boatImages);
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
    int boatVolume=0;
    pastHits.shipsSank.push_back(shipSize);
    for (int j=0; j<pastHits.shipsSank.size(); j++){
        boatVolume=pastHits.shipsSank[j];
    }

    //checking (but it is currently broken) that the hits are equal to the size of the ship
    if (pastHits.positionsHit.size()==boatVolume){
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
        cout<<" HIT! ";
        pastHits.previous=true;
        pastHits.positionsHit.push_back(shot);
        pastHits.stillShooting=true;
        for (int i=0; i<boatStuff.size(); i++){
            if (isInString(boatStuff[i].positions, shot)){
                boatStuff[i].timesHit+=1;
                if (boatStuff[i].timesHit==boatStuff[i].shape){
                    cout<<" You sunk my "<<i+2<<" long battleship! :(";

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

//getting shots
void shooting(vector <string> positions, vector <string> &shots, string person, vector <string> &placesShot, vector <string> &boats, vector <boat> &boatStuff, int &shipsSunk, hits &pastHits, int &orderNum, vector <string> order){
    string shot;
    bool isUser=false;
    //seeing who it is
    if (person=="user"){
        isUser=true;
    }
    //getting shots
    if (isUser){
        cout<<"Please enter a coordinate to shoot.";
        cin>>shot;
        shot[0]=toupper(shot[0]);
        while (!isInString(POSSIBLES, shot)){
            cout<<"Please enter an actual coordinate to shoot. (format is still A1 or a1): ";
            cin>>shot;
            shot[0]=toupper(shot[0]);
        }
    } else {
        //making the computer be smart
        if (pastHits.stillShooting==true){
            computerSmart(placesShot, pastHits, shot);
            //shot=possibles[rand()%100];
        } else {
            //choosing where to shoot
            shot=order[orderNum];
            orderNum++;
            if (shot==""){
                cout<<"?";
            }
        }
    }
    //making sure they haven't already shot there
    while (isInString(placesShot, shot)){
        if (isUser){
            do {
                cout<<"Please enter a NEW coordinate to shoot.";
                cin>>shot;
                shot[0]=toupper(shot[0]);
            } while (!isInString(POSSIBLES, shot));
        } else {
            shot=order[orderNum];
            orderNum++;
            cout<<shot;
            //more error checking (this error was annoying)
            if (shot==""){
                cout<<"?";
            }
        }
    }
    if (isUser){
        //system("clear");
    }
    placesShot.push_back(shot);
    //checking for errors
    if (shot==""){
        cout<<"?";
    }
    string wasHit=isHit(positions, shots, shot, boatStuff, shipsSunk, pastHits);
    boatPictures(shots, {shot}, wasHit);
    board(shots, boats);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

//testing
void randomUserBoat (vector <string> &positions, vector <int> &boats, vector <string> &boatImages, vector <string> shots, vector <boat> &boatStuff ){
    computerBoats(positions, boats, boatImages, shots, boatStuff);
}


void play(int &myWins, int &compWins, const vector <string> &order, double &avgShots, bool &playAgain){
    //variables
    string location, wantToPlace, wantToReplay;
    int compBattleshipsSunk=0;
    int userBattleshipsSunk=0;
    int orderNum=rand()%50, number=rand()%50;
    //bool isHit;
    vector <string> userBoats, userPositions, computerPositions, userShots, computerShots, userPlacesShot, computerPlacesShot, emptyBoats, computerBoatVisuals;
    vector <int> boats={2,3,4,5};
    vector <boat> userBoatStuff;
    vector <boat> computerBoatStuff;
    hits computerPastHits;
    hits empty;

    //getting ready
    settingUp(userShots, computerShots, userBoats, emptyBoats, computerBoatVisuals);
    board(userShots, userBoats);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //getting the user's boats
    while (wantToPlace!="c"&&wantToPlace!="r"){
        cout<<"\nWould you like to choose your boat placement or have the computer randomly generate them? (c for choosing, r for randomly): ";
        cin>>wantToPlace;
        //wantToPlace="r";
    }
    while (wantToPlace=="r"){
        userPositions.clear();
        userBoatStuff.clear();
        userShots.clear();
        userBoats.clear();
        settingUp(userShots, computerShots, userBoats, emptyBoats, computerBoatVisuals);
        randomUserBoat(userPositions, boats, userBoats, userShots, userBoatStuff);
        cout<<"\nIf you like this position enter y, otherwise enter 'r' to randoize it: ";
        cin>>wantToPlace;
        //wantToPlace="y";
    }
    if (wantToPlace=="c"){
        gettingBoatPlaces(userPositions, boats, userBoats, userShots, userBoatStuff);

    }

    //getting the comuter boats
    computerBoats(computerPositions, boats, computerBoatVisuals, computerShots, computerBoatStuff);
    

    //shooting
    system("clear");
    cout<<"Board to shoot at: ";
    board(userShots, emptyBoats);
    cout<<"\n\nYour board: ";
    board(computerShots, userBoats);
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //actually shooting
    while (compBattleshipsSunk!=4&&userBattleshipsSunk!=4){
        cout<<"\nYour guess: ";
        shooting(computerPositions, userShots, "user", userPlacesShot, emptyBoats, computerBoatStuff, compBattleshipsSunk, empty, number, order);
        cout<<"\n\nComputer's guess: ";
        shooting(userPositions, computerShots, "computer", computerPlacesShot, userBoats, userBoatStuff, userBattleshipsSunk, computerPastHits, orderNum, order);  
    }
    avgShots+=userPlacesShot.size();
    //showing the winner
    if (compBattleshipsSunk==4){
        cout<<"\n\nYou won!";
        myWins++;
    } else {
        cout<<"\n\nYou lost :(";
        compWins++;
        cout<<"\nThe computer's boats were in these positions: ";
        board(userShots, computerBoatVisuals);
    }
    cout<<"Do you want to play again? ";
    cin>>wantToReplay;

    if (tolower(wantToReplay[0])=='y'||wantToReplay=="sure"){
        playAgain=true;
    } else {
        playAgain=false;
    }
}

int main(){
    //time
    auto t1=std::chrono::high_resolution_clock::now();

    //variables
    int myWins=0, compWins=0, numGames=100;
    bool playAgain;
    double avgShots;
    vector <string> order=getOrder();

    do {
        srand(time(0));
        play(myWins, compWins, order, avgShots, playAgain);
        usleep(50000000);
    } while (playAgain);
    //end time
    auto t2=std::chrono::high_resolution_clock::now();
    auto elapsed_time=std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();

    //showing the stats
    cout<<"\n\nIt took "<<elapsed_time/1000000<<" seconds to run "<<numGames<<" games.";
    cout<<"\nThe person who went first won: "<<myWins<<" games";
    cout<<"\nThe person who went second won: "<<compWins<<" games";
    cout<<"\nIt took an average of "<<avgShots/numGames<<" shots";
    return 0;
}
//updated error list: nothing!!!!!!!!!!
//new error at 193, 299, 340, 365, 366, 437, 449, 636, 887
//errors at 15, 41, 107, 166, 187, 190, 226, 254, 265, 278, 286
