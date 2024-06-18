#include <cctype>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using std::cout, std::endl, std::cerr, std::ofstream, std::ifstream,
    std::vector, std::string;

const std::string BASE_FILE_NAME = "word_bank.txt";

// check if a file with specified file name exists
bool fileExists(std::string file);
// a valid response is a one character 'Y' or 'N'
bool validResponse(std::string word);
// checks to see if a string is at least three characters long and does not
// contain numbers
int checkValidWord(std::string string);
// get the word bank from the specified file.
void getWordBank(const std::string &file);
// word error map
std::unordered_map<int, std::string> wordErrorMap = {
    {2, "Word is less than three characters long, please make sure it's a real "
        "word"},
    {3, "Word contains integers, please make sure it's a real word"},
    {4, "Word contains unknown characters, please make sure it's a real word"}};

std::unordered_map<std::string, int> wordFrequencyMap;

// empty file functionality
bool emptyFileFunc();
// interactive prompt. will produce string value of choice
int getUserChoice();
// run different options
int runUserChoice(int option);
// option 1: add a word to the databank
int addWordToDataBank(void);
// option 2: list whole databank
int listWholeDataBank(void);
// option 3: search for a word
int searchDataBank(void);
// commit any changes to the map that may have been made
void commitChanges(const std::string &file);

int main(int argc, char *argv[]) {
  // if file exists prompt user to add word or check contents (sorted desc)
  // if file doesn't exist prompt user to create file
  // starter func
  if (!emptyFileFunc()) {
    return 1;
  }

  getWordBank(BASE_FILE_NAME);
  int userCode;
  int choice;
  do {
    choice = getUserChoice();
    if (choice == 4) {
      break;
    }

  } while ((userCode = runUserChoice(choice)) == 400 || userCode == 100);

  cout << endl << "Thank you and see you next time!" << endl;
}

bool emptyFileFunc() {
  if (!fileExists(BASE_FILE_NAME)) {
    std::string response;

    cout << "Create file called " << BASE_FILE_NAME
         << " to store and respective frequencies? (Y or N): ";
    std::cin >> response;

    while (!validResponse(response)) {
      cerr << "Please enter a valid response (Y or N): ";
      std::cin >> response;
    }

    if (toupper(response[0]) == 'Y') {
      ofstream create;
      create.open(BASE_FILE_NAME);
      cout << "Created file " << BASE_FILE_NAME << " succesfully.";
      create.close();
      return true;
    } else {
      std::cout << "Didn't create file " << BASE_FILE_NAME
                << ", unable to continue.";
      return false;
    }
  }
  return true;
}

// interactive prompt. will produce string value of choice
int getUserChoice() {
  std::string command;
  do {
    cout << "Please choose one of the following options." << endl;
    cout << "1) Add a word / word frequency to the databank" << endl;
    cout << "2) List the databank" << endl;
    cout << "3) Search for a words frequency" << endl;
    cout << "4) Exit" << endl;
    cout << "Please enter your choice: ";

    std::cin >> command;
    int command_val;

    try {
      command_val = stoi(command);
    } catch (std::invalid_argument e) {
      std::cerr << "Please enter a valid choice" << endl << endl;
      continue;
    }

    if (command_val <= 4 && command_val >= 1) {
      return command_val;
    } else {
      cout << "Please enter a valid choice" << endl << endl;
      continue;
    }

    cout << endl;

  } while ("4" != command);
  return 4;
}

int runUserChoice(int choice) {
  if (choice == 1) {
    return addWordToDataBank();
  } else if (choice == 2) {
    return listWholeDataBank();
  } else if (choice == 3) {
    return searchDataBank();
  } else {
    throw std::invalid_argument(
        "Error: choice was not 1 - 3 but reached 'runUserChoice' function.");
  }
}

// option 1: add a word to the databank
int addWordToDataBank(void) {
  std::string addendumWord;
  cout << "What word would you like to add to the database? ";
  std::cin >> addendumWord;
  int error = checkValidWord(addendumWord);

  // verifies the word the user inputted is valid
  while (error != 1) {
    cout << wordErrorMap[error] << endl << endl;
    cout << "What word would you like to add to the database? ";
    std::cin >> addendumWord;
    error = checkValidWord(addendumWord);
  }

  // verify word choice
  std::string response;
  cout << "Are you sure you would like to add \'" << addendumWord
       << "\' to the databank? (Y or N): ";
  std::cin >> response;

  while (!validResponse(response)) {
    cerr << "Please enter a valid response (Y or N): ";
    std::cin >> response;
  }

  if (toupper(response[0]) == 'N') {
    return 400;
  }

  cout << endl
       << "Adding one frequency for " << addendumWord << " to the databank."
       << endl;
  wordFrequencyMap[addendumWord] = wordFrequencyMap[addendumWord] + 1;
  cout << "Total frequency for " << addendumWord << " is now "
       << wordFrequencyMap[addendumWord] << endl
       << endl;

  // TODO: WHEN EVERYTHING IS CONFIRMED WORKING, CHANGE TO ORIGINAL FILE
  commitChanges("test.txt");

  return 100;
}

void commitChanges(const string &file) {
  ofstream fileStream(file);
  string key;
  string frequency;

  for (auto it = wordFrequencyMap.begin(); it != wordFrequencyMap.end(); ++it) {
    key = it->first;
    fileStream.write(key.c_str(), key.size());
    fileStream.put(',');
    frequency = std::to_string(it->second);
    fileStream.write(frequency.c_str(), frequency.size());
    fileStream.put('\n');
  }
}

// option 2: list whole databank ordered descending by frequency
//
// TODO: insanely inefficient but I'll try and refine this after i get off the
// plane
int listWholeDataBank(void) {
  vector<string> words;
  vector<int> indexingVector;

  // begin to iterate over the frequency map
  //
  // the basic idea is to create two vectors that mirror each other
  // (just like a map) and then order one of them by frequency DESC
  // and keep the insert order on the other vector (to mirror the map)
  //
  // this is inefficient and a better way would be to somehow order the map
  // based off the value before, or just not create an extra two vectors,
  // but that's a post 9 hour plane ride problem
  // hello vro <3 - jrodan
  // much love <3 - paolo
  for (auto it = wordFrequencyMap.begin(); it != wordFrequencyMap.end(); ++it) {
    string key = it->first;
    int value = it->second;
    int index = 0;
    for (; index < indexingVector.size(); ++index) {
      if (indexingVector[index] < value) {
        auto numsIt = indexingVector.begin();
        auto wordsIt = words.begin();
        for (int i = 0; i < index; ++i) {
          ++numsIt;
          ++wordsIt;
        }

        indexingVector.insert(numsIt, value);
        words.insert(wordsIt, key);
        break;
      }
    }
    if (index == indexingVector.size()) {
      words.push_back(key);
      indexingVector.push_back(value);
    }
  }

  cout << endl;

  // output the decomposed databank ordered on descending frequency
  for (int i = 0; i < words.size(); ++i) {
    cout << "The word \'" << words[i] << "\' has frequency "
         << indexingVector[i] << endl;
  }

  cout << endl;

  return 100;
}
// option 3: search for a word
int searchDataBank(void) {
  string searchWord;
  cout << "What word would you like to search for? ";
  std::cin >> searchWord;
  int error = checkValidWord(searchWord);

  // verifies the word the user inputted is valid
  while (error != 1) {
    cout << wordErrorMap[error] << endl << endl;
    cout << "What word would you like to search for? ";
    std::cin >> searchWord;
    error = checkValidWord(searchWord);
  }
  int frequency = wordFrequencyMap[searchWord];

  cout << endl
       << "Word \'" << searchWord << "\' has frequency " << frequency << endl
       << endl;

  if (frequency == 0) {
    wordFrequencyMap.erase(searchWord);
  }

  return 100;
}

int checkValidWord(std::string string) {
  // 2 signifies the size of the word is less than 3
  if (string.size() < 3) {
    return 2;
  }
  for (int i = 0; i < string.size(); ++i) {
    // 3 signifies there are numbers in the "word"
    if (isdigit(string[i])) {
      return 3;
    }
    // 4 signifies there is an unknown char in the "word"
    if (!isalpha(string[i])) {
      return 4;
    }
  }
  return 1;
}

bool fileExists(std::string file) {
  std::ifstream infile(file);
  return infile.good();
}

bool validResponse(std::string word) {
  if (word.size() > 1) {
    return false;
  }
  if (toupper(word[0]) == 'Y' || toupper(word[0]) == 'N') {
    return true;
  }
  return false;
}

void getWordBank(const std::string &fileName) {
  ifstream readFile(fileName);

  std::string line;
  std::string currentWord;
  std::string currentNumber;

  while (getline(readFile, line, '\n')) {
    currentWord.clear();
    currentNumber.clear();
    bool delimFlag = false;

    for (int i = 0; i < line.size(); ++i) {
      // indicates that we've passed the delim, aka "reached the int"
      if (delimFlag) {
        currentNumber.append(1, line[i]);
      } else {
        if (line[i] == ',') {
          delimFlag = true;
          continue;
        }
        currentWord.append(1, line[i]);
      }
    }

    wordFrequencyMap[currentWord] = stoi(currentNumber);
  }

  readFile.close();
}
