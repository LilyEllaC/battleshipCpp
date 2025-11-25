//importing
#include <iostream>
#include <cctype>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <string>
#include <set> 
#include <fstream>

using namespace std;

#define DEBUG_MODE // Define this macro for debug builds

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


//functions
//getting the possible locations
vector <string> getPositions(){
    vector <string> goodWords;       
    vector <string> newWords;
    ifstream file("positions.txt");    
    string word;                   
    
    // Read each word from file and add to set
    while (file >> word) {
        goodWords.push_back(word);             
    }

    //capitalizing
    for (const std::string& word : goodWords) {
        std::string upperWord = word; 
        for (char& c : upperWord) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        newWords.push_back(upperWord);
    }
    return newWords;  
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
bool spotAllowed (string position1, string position2, int boatNum, vector <string> possibles, vector <string> possible2s){
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
vector <string> possiblePosition2(vector <string> possibles, string position1, int boatNum){
    vector <string> possible2s;
    string position2;
    char letterCoord1, letterCoord2;
    int numberCoord1, numberCoord2;

    //setting the positions
    settingCoords(position1, letterCoord1, numberCoord1);

    for (int i=0; i<100; i++){
        //if (possibles[i]!="Z20"){
            position2=possibles[i];
            settingCoords(position2, letterCoord2, numberCoord2);
        
            //setting my vairables to see where in the list of possibles posible 1 and 2 are
            int coordinate1= positionInString(possibles, position1);
            int coordinate2= positionInString(possibles, position2);
            // here we use our clever math to know that the distance up/down/left/right must be specfic values
            if (coordinate1-coordinate2==boatNum-1||coordinate1-coordinate2==-boatNum+1||coordinate1-coordinate2==boatNum*10-10||coordinate1-coordinate2==-boatNum*10+10){
                //stopping it from being a column over (ex A10-B1)
                if (letterCoord1==letterCoord2||numberCoord1==numberCoord2){
                    //cout<<"\nCoords: "<<position1<<"  "<<position2<<endl;
                    possible2s.push_back(position2);
                } else{
                    possible2s.push_back("Z20");
                }
            }
        //}
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
                //cout<<possibles[positionInString(possibles, position1)-(i+1)];
            } else {
                newPositions.push_back(possibles[positionInString(possibles, position1)+(i+1)]);
                //cout<<possibles[positionInString(possibles, position1)+(i+1)];
            }
        } else {
            //horizontal
            //left or right
            if (letterCoord1>letterCoord2){
                newPositions.push_back(possibles[positionInString(possibles, position1)-(i+1)*10]);
                //cout<<possibles[positionInString(possibles, position1)-(i+1)*10];
            } else {
                newPositions.push_back(possibles[positionInString(possibles, position1)+(i+1)*10]);
                //cout<<possibles[positionInString(possibles, position1)+(i+1)*10];
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
            cout<<"FAILED";
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
void boatPictures(vector <string> &boatImages, vector <string> positions, vector <string> possibles, string shape){
    char letterCoord;
    int numberCoord;

    for (int i=0; i<positions.size(); i++){
        //setting the positions
        if (positions[i]!="NB"&&positions[i]!="Z20"){
            settingCoords(positions[i], letterCoord, numberCoord);
            boatImages[(numberCoord)*10-10+letterCoord-'A']=shape;
        }
        //cout<<letterCoord<<"   "<<numberCoord<<"\n";
    }
}

//getting the positions
void gettingBoatPlaces(vector <string> &positions, vector <string> possibles, vector <int> boats, vector <string> &boatImages, vector <string> &shots, vector <boat> &boatStuff){
    int boatNum;
    string position1, position2="0";
    bool works=true;
    for (int i=0; i<4; i++){
        boatStuff.push_back(boat());
        cout<<"Before while";
        do{
            cout<<"\nWhich boat do you want to place? (";
            // print out boat options
            for (int j=0; j<boats.size(); j++){
                cout<<boats[j];
                
                if (j!=boats.size()-1){
                    cout<<", ";
                }
            }
            cout<<")";
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
                while (!isInString(possibles, position1)||isInString(positions, position1)){
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
            while (!isInString(possibles, position1)||isInString(positions, position2)){
                cout<<"That isn't a possible answer. \nPlease use format 'A1': ";
                cin>>position2;
                position2[0]=toupper(position2[0]);
            }
            while (!spotAllowed(position1, position2, boatNum, possibles, possiblePosition2(possibles, position1, boatNum))){
                cout<<"Failed: ";
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
        boatPictures(boatImages, positions, possibles, "$");
        board(shots, boatImages);
    }
}

//computer positions
void computerBoats(vector <string> &positions, vector <string> possibles, vector <int> boats, vector <string> &boatImages, vector <string> shots, vector <boat> &boatStuff){
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
            position1=possibles[rand()%100];
            while (isInString(positions, position1)){
                position1=possibles[rand()%100];
            }

            //getting the possibles
            possible2s=possiblePosition2(possibles, position1, boatNum);
            while (possible2s.size()==0){
                while (isInString(positions, position1)){
                    position1=possibles[rand()%100];
                }
                possible2s=possiblePosition2(possibles, position1, boatNum);
                cout<<"Error\n";
            }
            //second spot
            position2=possible2s[rand()%possible2s.size()];

            while (!spotAllowed(position1, position2, boatNum, possibles, possiblePosition2(possibles, position1, boatNum))||position2=="Z20"){
                cout<<"Bad: "<<position2<<endl;
                position2=possible2s[rand()%possible2s.size()];
            }
            cout<<" Positions: "<<position1<<" "<<position2;

            cout<<" yay";
            //adding positions to list
            //system("clear");
            addingPos(positions, position1, position2, boatNum, works, boatStuff[i]);            

        } while (!works);
        boatPictures(boatImages, positions, possibles,"$");
        board(shots, boatImages);
    }
}

//making the computer be smart
void computerSmart(vector <string> possibles, vector <string> placesShot, hits &pastHits, string &shot){
    vector <string> possibleInitialBoatSpots=possiblePosition2(possibles, pastHits.positionsHit[0],2);
    vector <string> possibleNextBoatSpots=possiblePosition2(possibles, pastHits.positionsHit[pastHits.positionsHit.size()-1],2);
    bool swapped=false, continuing=false;
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
    {
        do {
            pastHits.direction;
            swapped=false;
            //swapping direction if they miss
            if (isInString(placesShot, possibleNextBoatSpots[pastHits.direction])&&!swapped){
                shot=possibleNextBoatSpots[pastHits.direction];
                swapped=true;
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
            } else if (!swapped&&possibleNextBoatSpots[pastHits.direction]!="Z20"){
                shot=possibleNextBoatSpots[pastHits.direction];
            } else {
                shot=possibleInitialBoatSpots[pastHits.direction];
                continuing=true;
            }
            //continuing if it is a miss
        } while ((isInString(placesShot, possibleNextBoatSpots[pastHits.direction])&&swapped)||continuing);
        cout<<"    "<<shot;
        cout<<"Both were hits";
    }
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
            //cout<<"New boat: ";
            for (int j=0; j<boatStuff[i].positions.size(); j++){
                //cout<<boatStuff[i].positions[j]<<", ";
            }
            if (isInString(boatStuff[i].positions, shot)){
                boatStuff[i].timesHit+=1;
                if (boatStuff[i].timesHit==boatStuff[i].shape){
                    cout<<" You sunk my battleship! :(";
                    //checking if all the hits were on the sunken 
                    int boatVolume=0;
                    pastHits.shipsSank.push_back(i+2);
                    for (int j=0; j<pastHits.shipsSank.size(); j++){
                        boatVolume=pastHits.shipsSank[j];
                    }
                    if (pastHits.positionsHit.size()==boatVolume){
                        cout<<"IMPORTANT: ALL SHOT SHIPS HAVE BEEN SUNK";
                        pastHits.positionsHit.clear();
                        pastHits.stillShooting=false;
                    }
                    boatStuff.erase(boatStuff.begin()+i);
                    shipsSunk+=1;
                }
            }
        }

        //returning the right thing
        return "\033[0;91mX";
    } else {
        cout<<" Miss";pastHits.previous=false;
        return "\033[0;93mO";
    }
   
}

//getting shots
void shooting(vector <string> positions, vector <string> possibles, vector <string> &shots, string person, vector <string> &placesShot, vector <string> &boats, vector <boat> &boatStuff, int &shipsSunk, hits &pastHits){
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
        while (!isInString(possibles, shot)){
            cout<<"Please enter an actual coordinate to shoot.";
            cin>>shot;
            shot[0]=toupper(shot[0]);
        }
    } else {
        //making the computer be smart
        if (pastHits.stillShooting==true){
            computerSmart(possibles, placesShot, pastHits, shot);
            //shot=possibles[rand()%100];
        } else {
            //choosing where to shoot
            shot=possibles[rand()%100];
        }
    }

    //making sure they haven't already shot there
    while (isInString(placesShot, shot)){
        if (isUser){
            do {
                cout<<"Please enter a NEW coordinate to shoot.";
                cin>>shot;
                shot[0]=toupper(shot[0]);
            } while (!isInString(possibles, shot));
        } else {
            shot=possibles[rand()%100];
            cout<<shot;
        }
    }
    if (isUser){
        //system("clear");
    }
    placesShot.push_back(shot);
    boatPictures(shots, {shot}, possibles, isHit(positions, shots, shot, boatStuff, shipsSunk, pastHits));
    for (int i=0; i<shots.size(); i++){
        if (shots[i]!="\033[1;32m"){
            //boats[i]="";
        }
    }
    board(shots, boats);
}

//testing
void test (vector <string> &positions, vector <string> possibles, vector <int> &boats, vector <string> &boatImages, vector <string> shots, vector <boat> &boatStuff ){
    computerBoats(positions, possibles, boats, boatImages, shots, boatStuff);
}

int main(){
    //variables
    string location;
    int compBattleshipsSunk=0;
    int userBattleshipsSunk=0;
    //bool isHit;
    vector <string> userBoats, userPositions, computerPositions, userShots, computerShots, userPlacesShot, computerPlacesShot, emptyBoats, computerBoatVisuals;
    vector <string> possibleSpots=getPositions();
    vector <int> boats={2,3,4,5};
    vector <boat> userBoatStuff;
    vector <boat> computerBoatStuff;
    hits computerPastHits;
    hits empty;

    //random
    srand(time(0));


    //getting ready
    settingUp(userShots, computerShots, userBoats, emptyBoats, computerBoatVisuals);
    board(userShots, userBoats);
    //gettingBoatPlaces(userPositions, possibleSpots, boats, userBoats, userShots, userBoatStuff);
    test(userPositions, possibleSpots, boats, userBoats, userShots, userBoatStuff);
    computerBoats(computerPositions, possibleSpots, boats, computerBoatVisuals, computerShots, computerBoatStuff);
    

    //shooting
    system("clear");
    cout<<"Board to shoot at: ";
    board(userShots, emptyBoats);
    cout<<"\n\nYour board: ";
    board(computerShots, userBoats);

    //actually shooting
    for (int i=0; i<100; i++){
        if (compBattleshipsSunk!=4){
            cout<<"\nYour guess: ";
            shooting(computerPositions, possibleSpots, userShots, "user", userPlacesShot, emptyBoats, computerBoatStuff, compBattleshipsSunk, empty);
            cout<<"\n\nComputer's guess: ";
            shooting(userPositions, possibleSpots, computerShots, "computer", computerPlacesShot, userBoats, userBoatStuff, userBattleshipsSunk, computerPastHits);
            cout<<"Was a hit: "<<computerPastHits.previous;//
            if (computerPastHits.positionsHit.size()>0){
                cout<<" Positions: "<<computerPastHits.positionsHit[0];//<<" Ships Sunk: "<<computerPastHits.shipsSank[0];
            }
        } else {
            cout<<"You won!";
        }
    }

    //ending
    return 0;
}