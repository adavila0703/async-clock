#include <iostream>
#include <stdio.h>
#include <ctime>
#include <math.h>
#include <unistd.h>
#include <string>
#include <future>
#include <windows.h>

using namespace std;

bool MENU_ON;

enum DisplayType
{
  TYPE_24,
  TYPE_12,
  TYPE_BOTH,
};

// clock object.
class clockObject
{
private:
  int second = 0;
  int minute = 0;
  int hour = 0;

  // returns a 24 hour formatted string
  string formatTime24()
  {
    string hour = this->twoDigitString(this->hour);
    string minute = this->twoDigitString(this->minute);
    string second = this->twoDigitString(this->second);
    return hour + ':' + minute + ':' + second;
  }

  // returns a 12 hour formatted string
  string formatTime12()
  {
    string hour = this->twoDigitString(this->hour == 0 ? 12 : this->hour);
    string period = this->hour > 11 ? "PM" : "AM";

    if (this->hour > 12)
    {
      hour = this->twoDigitString(this->hour - 13);
    }

    string minute = this->twoDigitString(this->minute);
    string second = this->twoDigitString(this->second);
    return hour + ':' + minute + ':' + second + " " + period;
  }

  // takes in an integer and returns the string value
  string twoDigitString(unsigned int number)
  {
    string numStr = to_string(number);

    if (numStr.length() < 2)
    {
      numStr = "0" + numStr;
    }
    return numStr;
  }

public:
  clockObject()
  {
  }
  ~clockObject()
  {
  }

  void addTime()
  {
    this->second++;

    if (this->second > 59)
    {
      this->minute++;
      this->second = 0;
      if (this->minute > 59)
      {
        this->hour++;
        this->minute = 0;
        if (this->hour > 24)
        {
          this->hour = 0;
        }
      }
    }
  }

  // displays the time in the given format
  void displayTime(string exitText, DisplayType type)
  {
    string time;

    switch (type)
    {
    case TYPE_24:
      time = this->formatTime24();
      break;
    case TYPE_12:
      time = this->formatTime12();
      break;
    case TYPE_BOTH:
      time += this->formatTime24();
      time += "\n";
      time += this->formatTime12();
      break;

    default:
      break;
    }

    system("cls");
    cout << exitText << endl;
    cout << time << endl;
    sleep(1);
  }

  void addOneHour()
  {
    this->hour++;
    if (this->hour > 25)
    {
      this->hour = 0;
    }
  }

  void addOneMinute()
  {
    this->minute++;
    if (this->minute > 59)
    {
      this->minute = 0;
    }
  }

  void addOneSecond()
  {
    this->second++;
    if (this->second > 59)
    {
      this->second = 0;
    }
  }

  // this function is meant to allow the users to set a custom time
  void setTime()
  {
    system("cls");
    int input;

    while (1)
    {

      cout << "Set hour" << endl;
      cin >> input;
      if (input <= 24)
      {
        this->hour = input;
        break;
      }
      cout << "Make sure your input 24 or less" << endl;
    }

    while (1)
    {
      cout << "Set minute" << endl;
      cin >> input;
      if (input <= 59)
      {
        this->minute = input;
        break;
      }
      cout << "Make sure your input 59 or less" << endl;
    }

    while (1)
    {
      cout << "Set second" << endl;
      cin >> input;
      if (input <= 59)
      {
        this->second = input;
        break;
      }
      cout << "Make sure your input 59 or less" << endl;
    }
  }
};

// clock which updates our clock object every second
void clockTick(clockObject *clock)
{
  int sec_prev = 0;
  while (1)
  {
    int newSeconds, prevSeconds;

    // storing total seconds
    time_t total_seconds = time(0);

    // getting values of seconds, minutes and hours
    struct tm *localTime = localtime(&total_seconds);

    newSeconds = localTime->tm_sec;

    if (newSeconds != prevSeconds)
    {
      clock->addTime();
      prevSeconds = newSeconds;
    }

    if (MENU_ON == false)
    {
      break;
    }
  }
}

// returns a string of a given character multiplied by a size
string nCharString(size_t size, char character)
{
  string output;

  for (size_t i = 0; i < size; i++)
  {
    output += character;
  }

  return output;
}

// handles the time display
void handleDisplayTime(clockObject *clock, DisplayType type)
{
  while (1)
  {
    clock->displayTime("Hold backspace to exit.", type);
    SHORT keyState = GetKeyState(VK_BACK);
    bool isDown = keyState & 0x8000;

    if (isDown == true)
    {
      break;
    }
  }
}

// display the user menu
void displayMenu()
{
  system("cls");
  cout << nCharString(5, ' ') << "Main Menu" << endl;
  cout << nCharString(20, '*') << endl;
  cout << "1. Display 24 and 12 hour clock" << endl;
  cout << "2. Display 24 hour clock" << endl;
  cout << "3. Display 12 hour clock" << endl;
  cout << "4. Add one hour" << endl;
  cout << "5. Add one minute" << endl;
  cout << "6. Add one second" << endl;
  cout << "7. Set time" << endl;
  cout << "0. Exit Program" << endl;
}

// handles user inputs
void handleMenuInputs(char input, clockObject *clock)
{
  switch (input)
  {
  case '1':
    handleDisplayTime(clock, DisplayType::TYPE_BOTH);
    break;
  case '2':
    handleDisplayTime(clock, DisplayType::TYPE_24);
    break;
  case '3':
    handleDisplayTime(clock, DisplayType::TYPE_12);
    break;
  case '4':
    clock->addOneHour();
    break;
  case '5':
    clock->addOneMinute();
    break;
  case '6':
    clock->addOneSecond();
    break;
  case '7':
    clock->setTime();
    break;

  default:
    break;
  }
}

// main menu loop
void mainMenuLoop(clockObject *clock)
{
  char input;
  MENU_ON = true;
  while (1)
  {
    displayMenu();
    cin >> input;
    handleMenuInputs(input, clock);
    if (input == '0')
    {
      break;
    }
  }
  MENU_ON = false;
  cout << "Exiting..." << endl;
}

// main
int main()
{
  // create clock object
  clockObject clock = clockObject();

  // start an asynchronous clock which updates every second
  auto clockFuture = async(clockTick, &clock);

  // main menu
  mainMenuLoop(&clock);

  // clear promise
  clockFuture.wait();

  return 0;
}