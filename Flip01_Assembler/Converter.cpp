
//--------------------------------------------------------------------------------//
//                                                                                //
//                            BIN-HEX-DEC | CONVERTER                             //
//        This software is part of the assembler designed for the Flip01 CPU      //
//                                                                                //
//                                  MIT License                                   //
//                                                                                //
//     Copyright (c) 2024 Pescetti Studio (Biasolo Riccardo & Croci Lorenzo)      //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//                                                                                //
//                                                                                //
//      Author(s): Pescetti Studio (Biasolo Riccardo & Croci Lorenzo) - 2024      //
//                                                                                //
//                        Do you think this code sucks?                           //
//                            You’re probably right!                              //
//              Hit us up at pescettistudio@gmail.com and let us know             //
//                            how to make it better.                              //
//                                                                                //
//--------------------------------------------------------------------------------//

#include <windows.h>
#include <string>
#include <sstream>
#include <bitset>
#include <commctrl.h>
#include <richedit.h>
#include <stdexcept>

using namespace std;

// Constants----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Window and application settings
const char* ICON_LOCATION = "AssemblerAssets/AssemblerIcon.ico";                              // Path to the application icon
const char* BIN_STARTUP_SENTENCE = "      Converted Bin Value";                               // Standard message for the binary conversion section
const char* DEC_STARTUP_SENTENCE = "      Converted Dec Value";                               // Standard message for the decimal conversion section
const char* HEX_STARTUP_SENTENCE = "      Converted Hex Value";                               // Standard message for the hexadecimal conversion section
const char* ERR_STARTUP_SENTENCE = "\r\n                   Any error will show up here";      // Initial message for the error display box

//Global variables---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Error information
string errorString = ""; // String to store any error messages
bool errorCheck = false; // True if errors were detected; false otherwise

// Conversion values
int inputFormat = 10;    // Default to Decimal input

// Global variables for controls
HWND hInput;             // Handle for the input box control
HWND hBinOutput;         // Handle for the binary conversion box control
HWND hDecOutput;         // Handle for the decimal conversion box control
HWND hHexOutput;         // Handle for the hexadecimal conversion box control
HWND hBinButton;         // Handle for the button to select binary conversion
HWND hDecButton;         // Handle for the button to select decimal conversion
HWND hHexButton;         // Handle for the button to select hexadecimal conversion
HWND hErrorControl;      // Handle for the error viewer box control

//Functions declaration---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Functions for window and control management 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ApplyDarkMode(HWND hwnd);

//Functions for conversion and error handling
void ConvertNumber();
void ErrorMsg(int errorCode);
bool CheckIfIsNumber(string str);
string toHex(int value);
string toBinary(int value);

// Window and control management ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------  

/*
Entry point for the Windows application

@param hInstance     - Handle to the current instance of the application
@param hPrevInstance - Handle to the previous instance of the application (always NULL for Win32 apps)
@param lpCmdLine     - Command line arguments passed to the application
@param nCmdShow      - Flag that specifies how the window should be shown
@return int          - Exit code of the application (usually 0 for success)
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
	// Define the name of the window class
    const char CLASS_NAME[] = "NumberConverter";
    WNDCLASS wc = { };
    wc.style = CS_DBLCLKS;
    wc.hIcon = (HICON)LoadImage(NULL, ICON_LOCATION, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszMenuName = NULL;
    wc.cbWndExtra = 0;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    // Create the main window with the specified class, text, and style
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        "Number Converter",             // Window title
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // Window style
        CW_USEDEFAULT, CW_USEDEFAULT,   // Default window position
        325, 370,                       // Window size (width, height)
        NULL,                           // Parent window
        NULL,                           // Menu
        hInstance,                      // Application instance
        NULL                            // No additional data
    );

    // Check if the window creation failed
    if (hwnd == NULL) {
        return 0;
    }

    // Show the window
    ShowWindow(hwnd, nCmdShow);

    // Message loop to handle events and dispatch messages to the window procedure
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);   // Translate keyboard messages
        DispatchMessage(&msg);    // Dispatch the message to the window procedure
    }

    // Return 0 to indicate successful execution of the application
    return 0;
}

// :)

/*
Event handler for the window procedure

@param hwnd     - Handle to the window for which the event is being processed
@param uMsg     - Message identifier that specifies the event
@param wParam   - Additional message information (depends on the message)
@param lParam   - Additional message information (depends on the message)
@return LRESULT - Result of processing the message
*/
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Create the font used by controls
        HFONT hFont = CreateFontA(
            18, 0, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH | FF_DONTCARE, 
            TEXT("MS Sans Serif")
        );
        
        // Create the input field for the number
        hInput = CreateWindow("EDIT", "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
            20, 45, 150, 20, hwnd, (HMENU)101, NULL, NULL
        );

        // Create Binary, Decimal, and Hexadecimal buttons
        hBinButton = CreateWindow("BUTTON", "Binary",
            WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
            180, 10, 80, 30, hwnd, (HMENU)1, NULL, NULL
        );

        hDecButton = CreateWindow("BUTTON", "Decimal",
            WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
            180, 35, 80, 30, hwnd, (HMENU)2, NULL, NULL
        );

        hHexButton = CreateWindow("BUTTON", "Hexadecimal",
            WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,
            180, 60, 120, 30, hwnd, (HMENU)3, NULL, NULL
        );

        // Set Decimal as the default selection
        SendMessage(hDecButton, BM_SETCHECK, BST_CHECKED, 0);

        // Create static text labels for output fields (Binary, Decimal, Hexadecimal)
        CreateWindow("STATIC", "BINARY",
            WS_VISIBLE | WS_CHILD,
            198, 122, 100, 20, hwnd, NULL, NULL, NULL
        );

        hBinOutput = CreateWindow(
            "EDIT", BIN_STARTUP_SENTENCE,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_READONLY,
            20, 120, 170, 20, hwnd, NULL, NULL, NULL
        );

        CreateWindow("STATIC", "DECIMAL",
            WS_VISIBLE | WS_CHILD,
            198, 162, 100, 20, hwnd, NULL, NULL, NULL
        );

        hDecOutput = CreateWindow(
            "EDIT", DEC_STARTUP_SENTENCE,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_READONLY,
            20, 160, 170, 20, hwnd, NULL, NULL, NULL
        );

        CreateWindow("STATIC", "HEXADECIMAL",
            WS_VISIBLE | WS_CHILD,
            198, 202, 100, 20, hwnd, NULL, NULL, NULL
        );

        hHexOutput = CreateWindow(
            "EDIT", HEX_STARTUP_SENTENCE,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_READONLY,
            20, 200, 170, 20, hwnd, NULL, NULL, NULL
        );

        // Create a control for displaying error messages
        hErrorControl = CreateWindowEx(
            0, "EDIT", ERR_STARTUP_SENTENCE,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            20, 240, 280, 60, hwnd, NULL, NULL, NULL
        );
        
        // Set the font for the error control and other output fields
        SendMessage(hErrorControl, WM_SETFONT, (WPARAM)hFont, TRUE);  // Set font for the error control
        SendMessage(hBinOutput, WM_SETFONT, (WPARAM)hFont, TRUE);     // Set font for the binary output field
        SendMessage(hDecOutput, WM_SETFONT, (WPARAM)hFont, TRUE);     // Set font for the decimal output field
        SendMessage(hHexOutput, WM_SETFONT, (WPARAM)hFont, TRUE);     // Set font for the hexadecimal output field
        
        // Apply dark mode to the window and its controls
        ApplyDarkMode(hwnd);
        break;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT: {
    	
        // Set the background and text color for controls to match dark mode
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(220, 220, 220));  // Light gray text
        SetBkColor(hdcStatic, RGB(20, 20, 20));       // Dark background
        return (LRESULT)CreateSolidBrush(RGB(20, 20, 20));
    }
    case WM_COMMAND: {
        if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == 101) {
            // If input field text has changed, trigger number conversion
            ConvertNumber();
        } else if (LOWORD(wParam) == 1) {
            // Binary button selected, set it checked and the others unchecked
            SendMessage(hBinButton, BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(hDecButton, BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(hHexButton, BM_SETCHECK, BST_UNCHECKED, 0);
            inputFormat = 2;  // Set input format to binary
            ConvertNumber();
        } else if (LOWORD(wParam) == 2) {
            // Decimal button selected, set it checked and the others unchecked
            SendMessage(hBinButton, BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(hDecButton, BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(hHexButton, BM_SETCHECK, BST_UNCHECKED, 0);
            inputFormat = 10; // Set input format to decimal
            ConvertNumber();
        } else if (LOWORD(wParam) == 3) {
            // Hexadecimal button selected, set it checked and the others unchecked
            SendMessage(hBinButton, BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(hDecButton, BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(hHexButton, BM_SETCHECK, BST_CHECKED, 0);
            inputFormat = 16; // Set input format to hexadecimal
            ConvertNumber();
        }
        break;
    }
    case WM_PAINT: {
        // Handle the window painting event
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Create the font used by controls
        HFONT hFont = CreateFontA(
            18, 0, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH | FF_DONTCARE, 
            TEXT("MS Sans Serif")
        );

        // Create a pen to draw a line
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(142, 142, 142));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        // Draw a line from (20, 100) to (300, 100)
        MoveToEx(hdc, 20, 100, NULL);
        LineTo(hdc, 300, 100);

        // Set the text color and background mode for drawing text
        SetTextColor(hdc, RGB(142, 142, 142));  // Set text color to light gray
        SetBkMode(hdc, TRANSPARENT);            // Transparent background

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // Draw a message on the window
        const char* text1 = "Write your number here";
        TextOut(hdc, 27, 25, text1, strlen(text1));

        // Draw a footer text
        const char* text2 = "Developed by Pescetti Studio ('_)<";
        TextOut(hdc, 50, 310, text2, strlen(text2));

        // Clean up resources
        SelectObject(hdc, hOldFont);  // Restore previous font
        DeleteObject(hFont);          // Delete created font
        SelectObject(hdc, hOldPen);   // Restore previous pen
        DeleteObject(hPen);           // Delete created pen
        EndPaint(hwnd, &ps);          // End the painting process
        break;
    }
    case WM_DESTROY: {
        // Post quit message to exit the application
        PostQuitMessage(0);
        break;
    }
    default:
        // Default handling for unprocessed messages
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

/*
Function to apply dark mode to the window and its child controls

@param hwnd - The handle of the window to which dark mode will be applied
*/
void ApplyDarkMode(HWND hwnd) {
    // Set window background color to a dark color
    HBRUSH darkBrush = CreateSolidBrush(RGB(20, 20, 20));
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)darkBrush);

    // Get the handle to the first child window/control of the specified parent window
    HWND hChild = GetWindow(hwnd, GW_CHILD);

    // Loop through all child windows and apply text color changes
    while (hChild) {

        // Redraw the error control (likely to refresh its appearance after applying dark mode)
        InvalidateRect(hErrorControl, NULL, TRUE);

        // Get the next child window/control
        hChild = GetNextWindow(hChild, GW_HWNDNEXT);
    }
}

// Conversion and error handling ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

/*
Function to convert the input string to an integer and display the result in binary, decimal, and hexadecimal formats.
Handles various input formats (binary, decimal, hexadecimal) and checks for errors.
*/
void ConvertNumber() {
	
    // Retrieve the text from the input control and store it in the buffer
    char buffer[256]; 
    GetWindowText(hInput, buffer, 256);
    string inputStr = buffer;
    
    // Clear the error string before starting the conversion process
    errorString = "";

    // Initialize the integer value to store the converted number
    int value = 0;

    // Flag to indicate if an error occurred during conversion
    errorCheck = false;

    // If the input string is not empty, process the input
    if (inputStr != "") {
        try {
            // Convert the input string based on the selected format
            if (inputFormat == 2) {
                // If binary input is selected convert the string to an integer
                value = stoi(inputStr, nullptr, 2);
                if (!CheckIfIsNumber(inputStr)) {
                    ErrorMsg(0);  // Display error message if the string contains invalid characters for binary
                }
            } else if (inputFormat == 10) {
                // If decimal input is selected convert the string to an integer
                value = stoi(inputStr, nullptr, 10);
                if (!CheckIfIsNumber(inputStr)) {
                    ErrorMsg(0);  // Display error message if the string contains non-numeric characters
                }
            } else if (inputFormat == 16) {
                // If hexadecimal input is selected convert the string to an integer
                value = stoi(inputStr, nullptr, 16);
                
                // Check if the string contains only valid hexadecimal digits
                for (int i = 0; i < inputStr.size(); i++) {
                    if (!isxdigit(inputStr[i])) {
                        ErrorMsg(0);  // Display error message if the string contains invalid characters for hexadecimal
                    }
                }
            }
        } catch (const invalid_argument& e) {
        	
            // Handle invalid argument exceptions (non-numeric input)
            ErrorMsg(0);
        } catch (const out_of_range& e) {
        	
            // Handle out of range exceptions (value is too large)
            ErrorMsg(1);
        }
    } else {
        // If the input string is empty, display an error message
        ErrorMsg(2);
    }

    // If there were no errors, proceed with the conversion and display the results
    if (!errorCheck) {
    	
        // Ensure that the integer value fits within the 8-bit range (0 to 255)
        if (value < 0 || value > 255) {
            ErrorMsg(3);
        }
    }

    string binaryStr = toBinary(value);                 // Convert the value to binary
    string decimalStr = to_string(value);               // Convert the value to decimal
    string hexStr = toHex(value);                       // Convert the value to hexadecimal
    
    // If no errors occurred, display the results
    if (!errorCheck) {
        SetWindowText(hErrorControl, "");               // Clear the error message
        SetWindowText(hBinOutput, binaryStr.c_str());   // Display the binary string
        SetWindowText(hDecOutput, decimalStr.c_str());  // Display the decimal string
        SetWindowText(hHexOutput, hexStr.c_str());      // Display the hexadecimal string
    } else {
        // If an error occurred, display "Error" in the output controls
        SetWindowText(hBinOutput, "Error");
        SetWindowText(hDecOutput, "Error");
        SetWindowText(hHexOutput, "Error");

        // Display the error message in the error control
        SetWindowText(hErrorControl, errorString.c_str());
    }
}

/*
Create the string containing the errors from the user's written code

@param errorCode - Represent the error code corresponding to a specific message
*/
void ErrorMsg(int errorCode) {
    
	// Increment the error counter
    errorCheck = true;
    
    errorString = "";
    errorString.append("Error E-").append(to_string(errorCode)).append(":\r\n");
    
    switch (errorCode) {
        case 0:
            errorString.append("Invalid input format");
            break;
        case 1:
            errorString.append("Input number is out of range");
            break;
        case 2:
            errorString.append("Empty input");
            break;
        case 3:
            errorString.append("The input number exceeds the 8-bit limit");
            break;
        case 4:
            errorString.append("Invalid input format");
            break;
        case 5:
            errorString.append("Invalid input format");
            break;
        default:
            break;
    }
}

/*
Check if a string represents a valid integer

@param str - The string to be checked
@return true if the string contains only numeric digits, false otherwise
*/
bool CheckIfIsNumber(string str) {
	
	// Check if each character of the str sting is a digit
    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i])) { 
        
        	// Return false if a non-digit character is found
            return false;
        }
    }
    
    // Return true if all characters are digits
    return true; 
}

// Ciao

/*
Function to convert an integer to its hexadecimal string representation

@param value - The integer to be converted to hexadecimal
@return A string containing the hexadecimal representation of the input integer
*/
string toHex(int value) {
    // Create a stringstream object to hold the hexadecimal output
    stringstream ss;

    // Set the stringstream to output in hexadecimal format
    ss << hex << value;

    // Convert the stringstream contents to a string and return it
    return ss.str();
}

/*
Function to convert an integer to its binary string representation

@param value - The integer to be converted to binary
@return A string containing the binary representation of the input integer
*/
string toBinary(int value) {
    // Convert the integer to a binary string, limited to 8 bits
    return bitset<8>(value).to_string();
}


// :) 

