
//--------------------------------------------------------------------------------//
//                                                                                //
//                           ASSEMBLER FOR THE FLiP01 CPU                         //
//                                                                                //
//                                   MIT License                                  //
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
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <richedit.h>
#include <tchar.h>
#include <cstdio>
#include <iomanip>
#include <algorithm>

using namespace std;

// Define control IDs for UI elements
#define ID_TEXT_BOX 1
#define ID_LABEL 2
#define ID_LINE_NUMBER 3
#define ID_SAVE_BUTTON 4
#define ID_OPEN_BUTTON 5
#define ID_NEW_BUTTON 6
#define ID_CHECK_BOX 7
#define ID_CONV_BUTTON 8
#define ID_MANUAL_BUTTON 9

// Define debug mode
#define DEBUG 0 //0 = Demo version, 1 = Debug version

// Constants----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Window and application settings
const char* TITLE = " Assembler for the FLiP01 CPU - Demo";             // Main window title
const char* STARTUP_TEXT_EDITOR = "Write your text here";               // Initial text in the text editor
const char* STARTUP_TEXT_CONSOLE = "\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\rAssembler for the FLiP01 CPU\r\nDeveloped by Pescetti Studio ('_)<"; // Initial text in the console
const char* STARTUP_FILE_NAME = "AssemblerAssets/startup.txt";          // Path to the file containing the splash texts to show at the startup
const char* ICON_LOCATION = "AssemblerAssets/AssemblerIcon.ico";        // Path to the application icon
const char* SAVE_BUTTON = "Save The Code";                              // Text for the "save" button
const char* OPEN_BUTTON = "Open A File";                                // Text for the "open file" button
const char* NEW_BUTTON = "New Code";                                    // Text for the "new code" button
const char* WINDOW_CLASS = "FlipAssembler";                             // Window class name

// Instruction sets and their corresponding opcodes

const string Instr0[] = { "addR", "subR", "andR", "orR", "cmpR", "flip", "stop", "testR", "nop", "wait", "mulR" }; 
// Instructions requiring 0 arguments
const string op0[] = { "018", "01E", "021", "01B", "032", "034", "038", "06B", "06F", "070", "63" }; 
// Op-codes for instructions with 0 arguments

const string Instr1[] = { "not", "neg", "inc", "dec", "rnd", "jc", "jv", "jn", "jz", "je", "jg", "jl", "jle", "jge", "jmp", "save", "read", "copy" }; 
// Instructions requiring 1 argument or 1 label
const string op1[] = { "12", "15", "24", "27", "37", "03A", "03C", "03E", "040", "042", "044", "046", "048", "04A", "04C", "5C", "5D", "73" }; 
// Op-codes for instructions with 1 argument or 1 label

const string Instr2[] = { "load", "store", "add", "sub", "and", "or", "shiftl$", "shiftr$", "cmp", "output", "input", "sub$", "add$", "and$", "or$", "load$", "cmp$", "test", "test$", "mul", "mul$" }; 
// Instructions requiring 2 arguments
const string op2[] = { "02", "04", "06", "09", "0C", "0F", "2A", "2D", "30", "5F", "5E", "4D", "50", "53", "56", "59", "5A", "69", "6D", "60", "66" }; 
// Op-codes for instructions with 2 arguments


// Processor memory limits--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const int MAX_ADDRESS = 65535;        // Maximum addressable memory for the processor
const int MAX_DATA = 255;             // Maximum data value for the processor

//Global variables---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Data structures
vector<vector<string>> codeMatrix;    // Matrix to store code instructions and variables
string textBuffer;                    // Buffer for text input/output

// Counters for errors and warnings
int errorCounter = 0;                 // Number of errors encountered
int warningCounter = 0;               // Number of warnings encountered

// Flags for program state
bool startup = true;                  // Indicates if the program is in its startup phase
bool liveAnalysis = true;             // A boolean flag for general use

// Handles and identifiers for Windows GUI components
HINSTANCE hInstance;                  // Instance handle for the application
HWND hTextBox;                        // Handle for the text box control
HWND hLabel;                          // Handle for the label control
HWND hLineNumber;                     // Handle for the line number control
HWND hSaveButton;                     // Handle for the save button
HWND hOpenButton;                     // Handle for the open button
HWND hNewButton;                      // Handle for the new button
HWND hConvButton;                     // Handle for the conversion button
HWND hManualButton;                   // Handle for the manual button
HWND hCheckbox;                       // Handle for the checkbox control
HANDLE hConsole;                      // Handle for the console window

// File and error information
string currentFilePath;               // Path of the current file being processed
string errorText;                     // Text buffer for error messages
string warningText;                   // Text buffer for warning messages
string error;                         // General error text (possibly unused)

// Text and file processing variables
string randomLine;                    // A random line of text from the specified startup file
int countNullLines = 0;               // Counter for the number of empty lines in the code
int countVariables = 0;               // Counter for the number of variable declarations in the code
int countLabels = 0;                  // Counter for the number of labels in the code
int countIODevices = 0;               // Counter for the number of devices declared in the code
int countComments = 0;                // Counter for the number of comments made by the user

// Global variables to hold process information for two programs
PROCESS_INFORMATION processInfo1 = { 0 }; // For the first program
PROCESS_INFORMATION processInfo2 = { 0 }; // For the second program

//Functions declaration---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Functions for window and control management 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void ShowConsole();
void ResetProgram(HWND hTextBox, HWND hLabel);
void UpdateLineNumbers();
 
//Functions for text manipulation  
void AnalyzeText(HWND hLabel);
void UpdatecodeMatrix(const string& text);
void CounterOfThings();
void JumpManagement();
void DeviceManagement();
void InstructionRecognition();
void CheckLength(int dim, int pos);
void AppendRegister(int pos, bool regNotNecessary);
void ImmediateInstructionManagement(int pos);
void VariableManagement();
void CheckVariableDeclaration(int pos);
void ShowResults();

//Functions for error and warning reporting  
void ErrorString (int errorCode, int line);
void WarningString(int warningCode, int line);

//Functions for file management 
bool GetRandomLineFromFile(const string& filePath, string& randomLine); 
void SaveTextToFile();
void OpenFileAndLoadText(HWND hwnd, HWND hTextBox);
void SaveMatrixToFile();

//Functions for Rich Edit controls  
void SetRichEditText(HWND hErrorControl, const string& text, COLORREF color);
void AppendTextToEditControl(HWND hTextBoxControl, const string& textToAppend);

//Other functions and routines
string DecimalToHexString(const string& decimalStr);
bool CheckIfIsNumber(string str);
bool StartsWith(const string& str, const string& prefix);
bool EndsWith(string const &str, string const &suffix);

// Functions to handle external programs ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool IsProcessRunning(PROCESS_INFORMATION& processInfo);
void StartConverter();
void ShowManual();
void TerminateChildProcesses();

// Window and control management ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------  

/*
Entry point for the Windows application

@param hInstance - Handle to the current instance of the application
@param hPrevInstance - Handle to the previous instance of the application (always NULL in Win32)
@param lpCmdLine - Command line arguments (not used in this example)
@param nCmdShow - How the window should be shown (SW_SHOWNORMAL, SW_SHOWMAXIMIZED, etc.)
@return The exit code of the application
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ::hInstance = hInstance;  // Store instance handle for later use
    
    // If debug mode is enabled, display the console
    if (DEBUG) {
        ShowConsole();
        cout << "\n" << randomLine << endl << endl;
    }

    // Get a random line from the startup file
    currentFilePath = "";
    GetRandomLineFromFile(STARTUP_FILE_NAME, randomLine);

    MSG messages;        // Message structure for the message loop
    WNDCLASS wincl;      // Window class structure
    HWND hwnd;           // Window handle

    // Set up and register the window class
    wincl.hInstance = hInstance;
    wincl.lpszClassName = WINDOW_CLASS;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.hIcon = (HICON)LoadImage(NULL, ICON_LOCATION, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = CreateSolidBrush(RGB(20, 20, 20));  // Set the window background color to black

    if (!RegisterClass(&wincl)) return 0;

    // Create and display the main window
    hwnd = CreateWindow(
        WINDOW_CLASS, TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 585, 635,
        HWND_DESKTOP, NULL, hInstance, NULL
    );
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main message loop
    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

	// Return exit code from message loop
    return messages.wParam;  
}

/*
Handles window messages

@param hwnd - Handle to the window receiving the message
@param message - The message identifier
@param wParam - Additional message-specific information
@param lParam - Additional message-specific information
@return The result of the message processing
*/
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	switch (message) {
    case WM_CREATE: {
    	
    	// Load Rich Edit library for rich text controls
    	LoadLibrary(TEXT("Riched20.dll"));

    	// Create the font used by controls
    	HFONT hFont = CreateFontA(
        	18, 0, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
        	OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH | FF_DONTCARE, 
        	TEXT("MS Sans Serif")
        );

        // Create a multiline text editor 
        hTextBox = CreateWindowEx(
            0, "EDIT", STARTUP_TEXT_EDITOR,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            60, 10, 510, 310, hwnd, (HMENU)ID_TEXT_BOX, NULL, NULL
        );
        // Add the initial text and scroll back to the top
        for (int r = 0; r < 19; r++){
        	AppendTextToEditControl(hTextBox, "\r\n");
		}
		AppendTextToEditControl(hTextBox, randomLine);
		SendMessage(hTextBox, WM_VSCROLL, SB_TOP, 0);
		
		// Create a rich edit control for displaying analysis results
    	hLabel = CreateWindowEx(
    		0, RICHEDIT_CLASS, TEXT(""),
    		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
    		10, 410, 560, 185, hwnd, (HMENU)ID_LABEL, NULL, NULL
		);
		SendMessage(hLabel, EM_SETBKGNDCOLOR, 0, RGB(20, 20, 20));         // Set background color to black
		SendMessage(hLabel, WM_SETFONT, (WPARAM)hFont, TRUE);              // Set font for the hLabel control
		SetRichEditText(hLabel, STARTUP_TEXT_CONSOLE, RGB(142, 142, 142)); // Display the initial text message for the hLabel control in gray

		// Create a control to display the line number
		hLineNumber = CreateWindowEx(
			0, "EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_MULTILINE | ES_READONLY | 
            WS_VSCROLL | ES_AUTOVSCROLL, 10, 10, 50, 310, hwnd, (HMENU)ID_LINE_NUMBER, 
			hInstance, NULL
		);
		
		// Hide the vertical scrollbar for the hLineNumber control
        ShowScrollBar(hLineNumber, SB_VERT, FALSE);  
        
        // Create 3 buttons: Save, Open, and New
        hSaveButton = CreateWindow(
            "BUTTON", SAVE_BUTTON, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE,
            10, 330, 180, 30, hwnd, (HMENU)ID_SAVE_BUTTON, NULL, NULL
        );
        
        hOpenButton = CreateWindow(
        	"BUTTON", OPEN_BUTTON, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE,
        	200, 330, 180, 30, hwnd, (HMENU)ID_OPEN_BUTTON, 
        	(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
    	);
        
		hNewButton = CreateWindow(
            "BUTTON", NEW_BUTTON, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE,
            390, 330, 180, 30, hwnd, (HMENU)ID_NEW_BUTTON, NULL, NULL
        );   
		
		// Create a checkbox for enabling/disabling live analysis
		hCheckbox = CreateWindowEx(
    		0, "BUTTON", "[X] Live analysis enabled", WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_PUSHLIKE,
    		200, 370, 180, 30, hwnd, (HMENU)ID_CHECK_BOX, hInstance, NULL 
		);    
		
		// Create a Rich Edit button for the first program
		hConvButton = CreateWindowEx(
        0, "BUTTON", "Conversion Table",
        WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
        10, 370, 180, 30,
        hwnd, (HMENU)ID_CONV_BUTTON, hInstance, NULL
    	);  
		
		// Create a Rich Edit button for the second program
    	hManualButton = CreateWindowEx(
        0, TEXT("BUTTON"), TEXT("View Manual"),
        WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
        390, 370, 180, 30,
        hwnd, (HMENU)ID_MANUAL_BUTTON, hInstance, NULL
    	);   
		
		// Check the hCheckbox checkbox by default
		SendMessage(hCheckbox, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);

        // Apply the same font to all created controls
        SendMessage(hTextBox,    WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hLineNumber, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSaveButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hOpenButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hNewButton,  WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hManualButton,   WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hConvButton,   WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hCheckbox,   WM_SETFONT, (WPARAM)hFont, TRUE);
        
        break;
	}

    case WM_CTLCOLORSTATIC: {
        // Set color for static controls (hLineNumber)
        HDC hdcStatic = (HDC)wParam;	
        SetTextColor(hdcStatic, RGB(142, 142, 142));        // Grey text by default
        if ((HWND)lParam == hLabel && !startup) {
        	SetTextColor(hdcStatic, RGB(255, 255, 255));    // White text after startup
		}
        SetBkColor(hdcStatic, RGB(20, 20, 20));             // Black background for the text
        return (INT_PTR)CreateSolidBrush(RGB(20, 20, 20));  // Black background for the control
    }

    case WM_CTLCOLOREDIT: {
        // Set text and background color for edit controls (hTextBox, hLabel)
        HDC hdcEdit = (HDC)wParam;
        
        if(startup){
        	SetTextColor(hdcEdit, RGB(142, 142, 142));      // Text in gray during startup
		} else {
			SetTextColor(hdcEdit, RGB(255, 255, 255));      // Default text in white
		}
        SetBkColor(hdcEdit, RGB(20, 20, 20));               // Black background for the text
        return (INT_PTR)CreateSolidBrush(RGB(20, 20, 20));  // Black background for the control
    }

	// Handle commands from controls (buttons, checkboxes, etc.)
    case WM_COMMAND: {
    	
    	// If the hTextBox control is used for the first time, exit the startup phase and clear the control
    	if (HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) == ID_TEXT_BOX && startup) {
            SetWindowText(hTextBox, "");
            UpdateLineNumbers();
            startup = false;        
        }

        // Handle text changes in the text box
        if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == ID_TEXT_BOX) {
            char text[MAX_ADDRESS];
            GetWindowText(hTextBox, text, MAX_ADDRESS * 10);
            textBuffer = text;
            
            if (!startup && liveAnalysis) {
            	AnalyzeText(hLabel);
        	}
        }

		// Handling the behavior of the three buttons: Save the code, Open a file, and New code
		if (LOWORD(wParam) == ID_SAVE_BUTTON) {
            SaveTextToFile();
        }
        if (LOWORD(wParam) == ID_OPEN_BUTTON) {
        	if (startup) {
        		SetWindowText(hTextBox, "");
			}
        	startup = true;
            OpenFileAndLoadText(hwnd, hTextBox);
            char text[MAX_ADDRESS];
            GetWindowText(hTextBox, text, MAX_ADDRESS);
            textBuffer = text;
            
            // Handle a failed file opening attempt
            if (textBuffer == "") {
            	SetWindowText(hTextBox, "Oh no.");
				if (DEBUG) {
					cout << "Opening sequence failed" << endl;
				}
			} else {
				startup = false;
				if (liveAnalysis) {
					AnalyzeText(hLabel);
				}
            	UpdateLineNumbers();
			}
        }
        		if (LOWORD(wParam) == ID_NEW_BUTTON) {
            ResetProgram(hTextBox, hLabel);
        }
        
        // Handle events related to the TextBox control scroll
        if (LOWORD(wParam) == ID_TEXT_BOX) {
            switch (HIWORD(wParam)) {
                case EN_VSCROLL:  // Handle vertical scrolling
                case EN_CHANGE:   // Handle text change
                	if(!startup){
                		UpdateLineNumbers();
					}
                    break;
            }
        }

		// Handle checkbox toggle (enable/disable live analysis)
		if (LOWORD(wParam) == ID_CHECK_BOX) {
			if (liveAnalysis) {
				SendMessage(hCheckbox, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
				liveAnalysis = false;
				SetWindowText(hLabel, "");
				SetRichEditText(hLabel, "\n\n\n\n\n\n\n\n\n\nLive analysis disabled by the user", RGB(142, 142, 142));
				SetWindowText(hCheckbox, "[  ] Live analysis disabled");
				if (DEBUG) {
					cout << "Live analysis disabled by the user" << endl;
				}
			} else {
				SendMessage(hCheckbox, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
				liveAnalysis = true;
				SetWindowText(hLabel, "");
				SetRichEditText(hLabel, "\n\n\n\n\n\n\n\n\n\n\nLive analysis enabled by the user", RGB(142, 142, 142));
				SetWindowText(hCheckbox, "[X] Live analysis enabled");
				if (!startup) {
					AnalyzeText(hLabel);
				}
				if (DEBUG) {
					cout << "Live analysis enabled by the user" << endl;
				}
			}			
        }
        
        if (LOWORD(wParam) == ID_CONV_BUTTON) {  // Button ID for the first program
            StartConverter();
        } else if (LOWORD(wParam) == ID_MANUAL_BUTTON) {  // Button ID for the second program
            ShowManual();
        }
        break;  
	}
        
    case WM_VSCROLL: {
        // Synchronize scrolling between the lines in hTextBox and hLineNumber
        if ((HWND)lParam == hLineNumber) {
            int scrollPos = GetScrollPos(hLineNumber, SB_VERT);
            SendMessage(hTextBox, EM_LINESCROLL, 0, scrollPos);
        } else if ((HWND)lParam == hTextBox) {
            UpdateLineNumbers();
        }
        break;
    }

    case WM_MOUSEWHEEL: {
        // Synchronize scrolling with the mouse wheel between the lines in hTextBox and hLineNumber
        if ((HWND)lParam == hLineNumber) {
            SendMessage(hTextBox, WM_MOUSEWHEEL, wParam, lParam);
            SendMessage(hLineNumber, WM_MOUSEWHEEL, wParam, lParam);
            UpdateLineNumbers();
        }
        break;
    }
    
	// Quit the application when the window is destroyed
    case WM_DESTROY:
    	TerminateChildProcesses(); // Terminate child processes before exiting
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

/*
Create and display a console for debugging purposes
*/
void ShowConsole() {
    // Attempt to allocate a new console for the application
    if (AllocConsole()) {
        FILE* pCStream;

        // Redirect standard output, input, and error streams to the new console
        freopen("CONOUT$", "w", stdout);
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stderr);

        
        cout.clear();  // Clear any existing output buffer
        cout.sync_with_stdio(true);  // Ensure stdio functions work in sync with c++ streams

        if (DEBUG){
        	cout << "Console is open for debugging." << endl;
		}
        
    } else {
        // If the console allocation fails, show an error message box
        MessageBox(NULL, "Failed to allocate console", "Error", MB_OK | MB_ICONERROR);
    }
}

/*
Resets the program to its initial state

@param hTextBox - Handle to the control where text is displayed
@param hLabel - Handle to the control where status messages are displayed
*/
void ResetProgram(HWND hTextBox, HWND hLabel) {
    
	// Prompt the user with a confirmation message box
    int response = MessageBox(NULL, "Are you sure you want to reset the program?", "Confirm Reset", MB_YESNO | MB_ICONQUESTION);
	
	// If the user confirms the reset
    if (response == IDYES) {
        codeMatrix.clear();    // Clear the word matrix, removing any parsed data
        textBuffer.clear();    // Clear the text buffer that holds the editor content
        errorCounter = 0;      // Reset the error counter
        startup = true;        // Return to startup mode by updating the corresponding flag
        currentFilePath = "";  // Reset the current file path

        // Set text box to initial placeholder text
        SetWindowText(hTextBox, STARTUP_TEXT_EDITOR);  

        if (DEBUG) {
            system("CLS");
            cout << "Console is open for debugging.\nProgram has been reset to original status." << endl;
        }

        // Select a random phrase from the startup file and display it
        GetRandomLineFromFile(STARTUP_FILE_NAME, randomLine);
        cout << "\n" << randomLine << endl << endl;
        for (int r = 0; r < 19; r++){
        	AppendTextToEditControl(hTextBox, "\r\n");
		}
        AppendTextToEditControl(hTextBox, randomLine);

        // Scroll the text box to the top
        SendMessage(hTextBox, WM_VSCROLL, SB_TOP, 0);

        // Clear the hLabel and hLineNumber UI elements
        SetWindowText(hLabel, "");
        SetWindowText(hLineNumber, "");

        // Reset the rich edit control with default console text
        SetRichEditText(hLabel, STARTUP_TEXT_CONSOLE, RGB(142, 142, 142));
    } else {
    	
        // If the user cancels the reset operation
        if (DEBUG) {
            cout << "\nReset cancelled by user\n" << endl;
        }
    }
}

/*
Synchronize the content of the hLineNumber control with the hTextBox control, updating its cursor position
*/
void UpdateLineNumbers() {
    int startChar;
    RECT rect;

    // Retrieve the size and position of the hTextBox control
    SendMessage(hTextBox, EM_GETRECT, 0, (LPARAM)&rect);

    // Get the character index of the topmost visible position in the hTextBox control
    startChar = SendMessage(hTextBox, EM_CHARFROMPOS, 0, MAKELPARAM(rect.left, rect.top));
    
    // Determine the line number corresponding to the topmost visible character
    int startLine = SendMessage(hTextBox, EM_EXLINEFROMCHAR, 0, startChar);

    // Build a string of line numbers, one for each line in the text box
    stringstream ss;
    int totalLines = SendMessage(hTextBox, EM_GETLINECOUNT, 0, 0);
    for (int i = 0; i < totalLines; i++) {
        ss << i + 1 << " \r\n ";  // Append line number with a newline
    }

    // Set the text of the hLineNumber control to the generated line numbers
    SetWindowText(hLineNumber, ss.str().c_str());

    // Scroll the hLineNumber control to match the scrolling of the hTextBox control
    int firstVisibleLine = SendMessage(hTextBox, EM_GETFIRSTVISIBLELINE, 0, 0);
    SendMessage(hLineNumber, EM_LINESCROLL, 0, firstVisibleLine);
}

// Text manipulation ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
Main function for analyzing the text written in the hTextBox control. 
Calls all necessary analysis functions.

@param hLabel - Handle to the label control where the results will be displayed.
*/
void AnalyzeText(HWND hLabel) {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get the console handle for debug output (if enabled)
    int i;  // Ccounter for iterating through the word matrix

    // Clear the console screen if debugging is enabled
    if (DEBUG) {
        system("CLS");  // Clear the console output to ensure a fresh start for debugging
    }
    
    // Analyze the text in the text buffer
    UpdatecodeMatrix(textBuffer);  
    CounterOfThings();              
    JumpManagement();                
    DeviceManagement();              
    InstructionRecognition();       
    VariableManagement();           

    // Debug output: Print matrix and analysis results to the console if debugging mode is active
    if (DEBUG) {
        SetConsoleTextAttribute(hConsole, 11);  // Set console text color to bright cyan for headers
        cout << "Final matrix:" << endl;        // Output header for the matrix display
        SetConsoleTextAttribute(hConsole, 15);  // Reset console text color to white

        // Iterate through the word matrix and display its contents
        for (i = 0; i < codeMatrix.size(); i++) {
        	
            // Skip entries where the first element is "NULL", ">", "<" or where the second element is "="
            if (codeMatrix[i][1] != "=" && codeMatrix[i][0] != "NULL" && codeMatrix[i][0] != "<" && codeMatrix[i][0] != ">") {
                for (int j = 0; j < 2; j++) {
                	
                	// Output non-NULL values in matrix
                    if (codeMatrix[i][j] != "NULL") {
                        cout << codeMatrix[i][j] << " ";
                    }
                }
                cout << endl;
            }
        }

        //Iterate through the matrix to display, at the end of the data and instruction matrix, the values assigned to any declared variables
        for (i = 0; i < codeMatrix.size(); i++) {
            if (codeMatrix[i][1] == "=") {
                for (int j = 0; j < 3; j++) {
                    cout << codeMatrix[i][j] << " ";
                }
                cout << endl;
            }
        }
        cout << endl; 
    }

    // Update the GUI with the results of the analysis
    ShowResults(); 
}

/*
Update and populate the matrix corresponding to the text entered in the hTextBox control

@param text The input text to be processed and added to the word matrix
*/
void UpdatecodeMatrix(const string& text) {
	
    // Reset error and warning counters, and null lines count
    errorCounter = 0;
    warningCounter = 0;
    countNullLines = 0;
    
    // Clear the existing word matrix
    codeMatrix.clear();
    
    // Create a string stream from the input text
    stringstream ss(text);
    string word;
    
    // Vector to hold words from each line
    vector<string> lineWords;  
    
    // Reset error and warning messages
    errorText = "";
    warningText = "";
    
    // Get the console handle for debug output
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Process each line from the input text
    while (getline(ss, word, '\n')) {   // Read the input text line by line
        stringstream lineStream(word);  // Create a string stream for each line
        lineWords.clear();              // Clear current line's words

        // Read words from the current line and add them to lineWords
        while (lineStream >> word) {
        	
        	// Only allow up to 3 words per line
            if (lineWords.size() < 3) {  
                lineWords.push_back(word);
            } else if (!StartsWith(lineWords[0], "//")) {
                // If more than 3 words and not a comment, increment error counter
                errorCounter++;
                ErrorString(6, codeMatrix.size()); // Error: Too many parameters
                break;  // Exit the word reading loop, discard extra words
            }
        }

        // If fewer than 3 words in the line, pad with "NULL"
        while (lineWords.size() < 3) {
            lineWords.push_back("NULL");
        }

        // Only add lines to the word matrix
        codeMatrix.push_back(lineWords);
    }
    
    // If debugging is enabled, print the matrix content to the console
    if (DEBUG) {
        SetConsoleTextAttribute(hConsole, 11);  // Set console text color to cyan
        cout << "Inserted text - matrix:" << endl;
        SetConsoleTextAttribute(hConsole, 15);  // Reset text color to white
        
        // Print each row of the matrix
        for (int i = 0; i < codeMatrix.size(); i++) {
            for (int j = 0; j < codeMatrix[i].size(); j++) {
                cout << codeMatrix[i][j] << " ";
            }
            cout << endl;
        }
    }
    
    // Saves user input to restore in case of an accidental crash
    SaveMatrixToFile();
}

/* 
Count the empty lines, declared variables, labels, and declared I/O devices
*/
void CounterOfThings() {
	
    // Initialize counters
    countNullLines = 0;
    countVariables = 0;
    countLabels = 0;
    countIODevices = 0;
    countComments = 0;
    
    // Iterate through each entry in the codeMatrix
    for (int i = 0; i < codeMatrix.size(); i++) {
    	
        // Count empty lines
        if (codeMatrix[i][0] == "NULL") {
            countNullLines++;
        }
        // Count variable declarations
        if (codeMatrix[i][1] == "=") {
            countVariables++;
        }
        // Count labels 
        if (EndsWith(codeMatrix[i][0], ":")) {
            countLabels++;
        }
        // Count the declared I/O devices
        if (codeMatrix[i][0] == "<" || codeMatrix[i][0] == ">") {
            countIODevices++;
        }
        // Count the comments
        if (StartsWith(codeMatrix[i][0], "//")){
        	countComments++;
		}
    }
    
    // If DEBUG mode is enabled, output the counts to the console
    if (DEBUG) {
        SetConsoleTextAttribute(hConsole, 10);  // Set text color to green for output
        cout << "\nCode properties:" << endl;
        SetConsoleTextAttribute(hConsole, 15);  // Reset text color to white
        
        cout << "Empty lines: " << countNullLines;
        cout << " Variables: " << countVariables;
        cout << " Labels: " << countLabels;
        cout << " Lines: " << codeMatrix.size();
        cout << " Declared Devices: " << countIODevices;
        cout << " Lines with code: " << codeMatrix.size() - countNullLines << endl << endl;
    }
}

// :)

/*
Handles all operations related to conditional and unconditional jumps  
Checks the declaration of labels
*/
void JumpManagement() {
	
    int jmpDefinitionFound = 0;       // Flag to indicate if a label definition is found
    int countLabelTemp = 0;           // Counter for label declarations
    int countNullTemp = 0;            // Counter for empty lines
    int countVarTemp = 0;             // Counter for variable declarations
    int countDevicesTemp = 0;         // Counter for devices declaration
    int countCommentsTemp = 0;        // Counter for comments made by the user
    int i = 0, j = 0;                 // Iteration variables
    bool labelIsUsed;                 // Flag to check if a label is used in a jump

    // Check if a label has been declared but not referenced in the code
    for (i = 0; i < codeMatrix.size(); i++) {
        if (EndsWith(codeMatrix[i][0], ":")) {
            labelIsUsed = false;
            string labelName = codeMatrix[i][0];
            labelName.pop_back();     // Remove trailing ":" from the label
            
            if(CheckIfIsNumber(labelName)){
        		ErrorString(21, i); // Error: Label name is a number
			}

            for (j = 0; j < codeMatrix.size(); j++) {
                if (codeMatrix[j][1] == labelName) {
                    labelIsUsed = true;
                }
            }

            if (!labelIsUsed) {
                WarningString(0, i);  // Warning: Label declared but not used
            }
        }
    }

    // Check if a label referenced by a jump is defined
    // Replace the label reference with the correct hexadecimal value if defined
    for (i = 0; i < codeMatrix.size(); i++) {
    	
        jmpDefinitionFound = 0;
        countLabelTemp = 0;
        countNullTemp = 0;
        countVarTemp = 0;
        countDevicesTemp = 0;
        countCommentsTemp = 0;

        if (StartsWith(codeMatrix[i][0], "j")) {
        	
        	if(CheckIfIsNumber(codeMatrix[i][1])){
        		ErrorString(21, i); // Error: Label name is a number
			}
            string labelName = codeMatrix[i][1] + ":";

            for (j = 0; j < codeMatrix.size(); j++) {
            	
            	// Count empty lines before the label declaration
                if (codeMatrix[j][0] == "NULL") {
                    countNullTemp++;
                }
                
                // Count label declarations before the label
                if (EndsWith(codeMatrix[j][0], ":")) {
                    countLabelTemp++;
                }
                
                // Count variable declarations before the label
                if (codeMatrix[j][1] == "=") {
                    countVarTemp++; 
                }
                
                // Count I/O devices before the label declaration
                if (codeMatrix[j][0] == "<" || codeMatrix[j][0] == ">") {
                    countDevicesTemp++; 
                }
                
                // Count the comments
        		if (StartsWith(codeMatrix[j][0], "//") || codeMatrix[j][0] == "//"){
        			countCommentsTemp++;
				}
                
                // Replace label reference with the hexadecimal value of its position
                if (codeMatrix[j][0] == labelName) {
                    jmpDefinitionFound++;
                    codeMatrix[i][1] = DecimalToHexString(to_string(j + 1 - countLabelTemp - countNullTemp - countVarTemp - countDevicesTemp- countCommentsTemp));
                }
            }

            if (jmpDefinitionFound == 0) {
                ErrorString(13, i); // Error: Label called but not defined
            } else if (jmpDefinitionFound > 1) {
                ErrorString(14, i); // Error: Label redefined within the code
            }
        }
    }
}

/*
Handles all operations related to input and output devices
Checks the declaration of I/O devices
*/
void DeviceManagement(){
	
	// Variables for tracking device usage and state
	bool input;
	bool output;
	int countDevicesI = 0;            // Counter for the number of input devices
	int countDevicesO = 0;            // Counter for the number of output devices
	int deviceIsUsed;                 // To track if a device is used correctly
	int i, j;                         // Loop counters
	bool deviceIsFound;               // Flag to indicate if a device is found
	
	// Check if a device used in the code has been correctly declared
	for(i = 0; i < codeMatrix.size(); i++){
		if(codeMatrix[i][0] == "input" || codeMatrix[i][0] == "output"){
			deviceIsFound = false;
			for(j = 0; j < codeMatrix.size(); j++){
				if((codeMatrix[j][1] == codeMatrix[i][2]) && (codeMatrix[j][0] == "<" || codeMatrix[j][0] == ">")){
					deviceIsFound = true;
					break;
				}
			}
			if(!deviceIsFound){
				ErrorString(20,i); // Error: Device used but not initialized
			}
		}
	}
	
	// Check if the device name and the assigned port number are correct
	for(i = 0; i < codeMatrix.size(); i++){
		if (codeMatrix[i][0] == ">" || codeMatrix[i][0] == "<"){
			for(j = 0; j < codeMatrix.size(); j++){
				if (codeMatrix[i][0] == codeMatrix[j][0] && codeMatrix[j][1] == codeMatrix[i][1] && codeMatrix[i][0] != "NULL" && j != i){
					ErrorString(24, i); // Error: Device name already in use
				}
				if (codeMatrix[i][0] == codeMatrix[j][0] && codeMatrix[j][2] == codeMatrix[i][2] && codeMatrix[i][2] != "NULL" && j != i){
					ErrorString(25, i); //Error: Port number already in use
				}
			}
			if(!CheckIfIsNumber(codeMatrix[i][2])){
				ErrorString(26, i); // Error: Port number is not an integer
			} else if (stoi(codeMatrix[i][2]) > 31 || stoi(codeMatrix[i][2]) < 0) {
				ErrorString (27, i); // Error: Port number is not between 0 and 31
			}
			
		}
	}
	
	// Check if a device declared in a specific mode is used in the same mode (input - input, output - output)
	for(i = 0; i < codeMatrix.size(); i++){
		input = false;
		output = false;
		deviceIsUsed = 0;
		
		// Determine if the current line represents an input or output operation
		if(codeMatrix[i][0] == ">"){
			input = true;
			countDevicesI++;
			deviceIsUsed++;
		}
		if(codeMatrix[i][0] == "<"){
			output = true;
			countDevicesO++;
			deviceIsUsed++;
		}
		if(input || output){
			if(CheckIfIsNumber(codeMatrix[i][1])){
				ErrorString(22, i); // Error: Device name is a number
			}
			if(codeMatrix[i][1] == "NULL"){
				ErrorString(23, i); // Error: No device assigned to the I/O operation
			}
			
			// Assign a unique memory cell to each I/O device
			for(j = 0; j < codeMatrix.size(); j++){
				if(codeMatrix[j][0] == "input" && codeMatrix[j][2] == codeMatrix[i][1] && input){
					deviceIsUsed++;
					codeMatrix[j][2] = DecimalToHexString(codeMatrix[i][2]);
				}
				if(codeMatrix[j][0] == "output" && codeMatrix[j][2] == codeMatrix[i][1] && output){
					deviceIsUsed++;
					codeMatrix[j][2] = DecimalToHexString(codeMatrix[i][2]);
				}
				if(codeMatrix[j][0] == "input" && codeMatrix[j][2] == codeMatrix[i][1] && output){
					deviceIsUsed++;
					ErrorString(19, i); // Error: Device declared as output but used as input
				}
				if(codeMatrix[j][0] == "output" && codeMatrix[j][2] == codeMatrix[i][1] && input){
					deviceIsUsed++;
					ErrorString(18, i); // Error: Device declared as input but used as output
				}
			}
		}
		// Warn if a device is declared but not used
		if(deviceIsUsed == 1){
			WarningString(2, i);
		}
	}
	
	// Check if the number of devices exceeds the maximum allowed limit
	if(countDevicesI > 32){
		ErrorString(15, 0);	// Error: Too many devices declared
	}
	if(countDevicesO > 32){
		ErrorString(16, 0);	// Error: Too many devices declared
	}
}

/*
Handles all operations, evaluations, and substitutions related to the syntax of instructions
*/
void InstructionRecognition() {
    int j;                                          // Loop variable 
    bool instructionFound;                          // Indicates whether the current instruction is recognized
    bool regNotNecessary = true;                    // Indicates whether a general-purpose register is required
    bool immediateInstruction = false;              // Indicates whether the instruction involves an immediate value

    // Iterate through each line of instructions in the codeMatrix
    for (int i = 0; i < codeMatrix.size(); i++) {
        instructionFound = false;                   // Reset instructionFound flag
        regNotNecessary = false;                    // Reset regNotNecessary flag
        immediateInstruction = false;               // Reset immediateInstruction flag

        // Check for instructions with 0 parameters
        for (j = 0; j < (sizeof(Instr0) / sizeof(Instr0[0])); j++) {
            if (codeMatrix[i][0] == Instr0[j]) {
                codeMatrix[i][0] = op0[j];          // Replace the instruction with the corresponding opcode
                codeMatrix[i][1] = "00";            // Default value for second parameter
                CheckLength(0, i);                  // Ensure that the number of parameters is correct
                instructionFound = true;            // Mark instruction as found
                break;  
            }        
        }

        // Check for instructions with 1 parameter
        for (j = 0; j < (sizeof(Instr1) / sizeof(Instr1[0])); j++) {
            if (codeMatrix[i][0] == Instr1[j]) {
                if (StartsWith(codeMatrix[i][0], "j")) {
                    regNotNecessary = true;         // Set regNotNecessary flag if the instruction is a jump
                }
                codeMatrix[i][0] = op1[j];          // Replace the instruction with the corresponding opcode
                CheckLength(1, i);                  // Ensure that the number of parameters is correct
                AppendRegister(i, regNotNecessary); // Validate and append the correct register (ax or bx)
                if (!regNotNecessary) {
                    codeMatrix[i][1] = "00";        // Default value for the second parameter
                }
                instructionFound = true;            // Mark instruction as found
                break;
            }        
        }

        // Check for instructions with 2 parameters
        for (j = 0; j < (sizeof(Instr2) / sizeof(Instr2[0])); j++) {
            if (codeMatrix[i][0] == Instr2[j]) {
                CheckLength(2, i);                  // Ensure that the number of parameters is correct
                
                // Handle immediate instructions
                if (EndsWith(codeMatrix[i][0], "$")) {
                    immediateInstruction = true; 
					ImmediateInstructionManagement(i);
                } else {
                    if (CheckIfIsNumber(codeMatrix[i][2]) && codeMatrix[i][0] != "input" && codeMatrix[i][0] != "output") {
                        ErrorString(3, i); // Error: Incorrect second parameter
                    } else {
                    	if(codeMatrix[i][0] != "input" && codeMatrix[i][0] != "output"){
                    		CheckVariableDeclaration(i); // Check if the variable used in the instruction is declared
						}
                    }
                }
                codeMatrix[i][0] = op2[j];               // Replace the instruction with the corresponding opcode
                AppendRegister(i, regNotNecessary);      // Validate and append the correct register (ax or bx)
                
                if(codeMatrix[i][0] == "05E" || codeMatrix[i][0] == "05F" || codeMatrix[i][0] == "15E" || codeMatrix[i][0] == "15F" || immediateInstruction){
                	codeMatrix[i][1] = codeMatrix[i][2];
				}
                
                instructionFound = true;
                break;
            }        
        }

        if (!instructionFound && codeMatrix[i][0] != "NULL" && codeMatrix[i][1] != "=" && !EndsWith(codeMatrix[i][0], ":") && codeMatrix[i][0] != ">" && codeMatrix[i][0] != "<" && !StartsWith(codeMatrix[i][0], "//")) {
            ErrorString(2, i); // Error: Incorrect instruction
        }
    }
}

/*
Check the number of parameters for an instruction

@param dim - The expected number of parameters for the instruction
@param pos - The line number containing the analyzed instruction
*/
void CheckLength(int dim, int pos) {
	
    // Expected number of parameters: 2
    if (dim == 2) {
    	
        // Too few parameters
        if ((codeMatrix[pos][1] == "NULL") || (codeMatrix[pos][2] == "NULL")) {
            ErrorString(4, pos); // Error: Too few parameters
        }
    }

    // Expected number of parameters: 1
    if (dim == 1) {
    	
        // Too few parameters
        if ((codeMatrix[pos][1] == "NULL")) {
            ErrorString(4, pos); // Error: Too few parameters
        }
        
        // Too many parameters
        if ((codeMatrix[pos][2] != "NULL")) {
            ErrorString(6, pos); // Error: Too many parameters
        }
    }

    // Expected number of parameters: 0
    if (dim == 0) {
    	
        // Too many parameters
        if ((codeMatrix[pos][2] != "NULL") || (codeMatrix[pos][1] != "00")) {
            ErrorString(6, pos); // Error: Too many parameters
        }
    }
}

/*
Add 1 as a prefix to the instruction if it refers to the AX register, or 0 if it refers to the BX register

@param pos - The line number where the considered instruction is present
@param regNotNecessary - If true, the instruction does not require a general-purpose register as a parameter
*/
void AppendRegister(int pos, bool regNotNecessary) {
    bool correctReg = false; // Flag to check if the correct register has been declared

    // Check for "ax" (general-purpose register 1)
    if ((codeMatrix[pos][1] == "ax") || (codeMatrix[pos][1] == "Ax") || 
        (codeMatrix[pos][1] == "aX") || (codeMatrix[pos][1] == "AX")) {
        codeMatrix[pos][0] = "1" + codeMatrix[pos][0]; // Prefix instruction with "1" for "ax"
        correctReg = true;
    }

    // Check for "bx" (general-purpose register 0)
    if ((codeMatrix[pos][1] == "bx") || (codeMatrix[pos][1] == "Bx") || 
        (codeMatrix[pos][1] == "bX") || (codeMatrix[pos][1] == "BX")) {
        codeMatrix[pos][0] = "0" + codeMatrix[pos][0]; // Prefix instruction with "0" for "bx"
        correctReg = true;
    }

    // If the instruction is a jump (regNotNecessary is true), a register is not required
    if (regNotNecessary) {
        correctReg = true;
    }

	// A valid register was not found
    if (!correctReg) {
        ErrorString(5, pos); // Error: Neither AX nor BX is used as the first parameter
	}
}

/*
Handles the second parameter of direct instructions

@param pos - The line number containing the analyzed instruction
*/
void ImmediateInstructionManagement(int pos) {
	
    // Check if the second parameter of the direct instruction is a number
    if (CheckIfIsNumber(codeMatrix[pos][2])) {
    	
        // Check if the second parameter of the direct instruction is a number supported by the Flip01 CPU
        if (stoi(codeMatrix[pos][2]) > 255) {
            ErrorString(12, pos); // Error: Value associated with the variable is too large
        } else {
            // Convert the valid number to an hexadecimal string representation
            codeMatrix[pos][2] = DecimalToHexString(codeMatrix[pos][2]);
        }
    } else {
        ErrorString(8, pos); // Error: Immediate instruction value is not a number
    }
}

/*
Handles all operations, evaluations, and substitutions related to variable declarations
*/
void VariableManagement() {
    bool variableIsUsed;                // Indicates whether the variable is used in the code
    int variablePosition;               // The calculated position of the variable in memory
    int count = 0;                      // Counter for variable declarations

    for (int i = 0; i < codeMatrix.size(); i++) {
        
		// Check for variable declarations
        if (codeMatrix[i][1] == "=") {
            
			// Validate variable names
            if (CheckIfIsNumber(codeMatrix[i][0])) {
                ErrorString(17, i); // Error: Variable name is a number
            }

            count++; // Increment the variable declaration count
            variableIsUsed = false;     // Reset variableIsUsed flag
            
            // Calculate the memory position for the variable
            variablePosition = codeMatrix.size() - countLabels - countVariables - 1 + count - countNullLines - countIODevices - countComments;

            // Validate and convert the variable value to hexadecimal
            if (CheckIfIsNumber(codeMatrix[i][2])) {
                if (stoi(codeMatrix[i][2]) > 255) {
                    ErrorString(12, i); // Error: Value associated with the variable is too large
                }
                // Convert the value assigned to the variable into hexadecimal
                codeMatrix[i][2] = DecimalToHexString(codeMatrix[i][2]);
            } else {
                ErrorString(10, i); // Error: Value associated with the variable is not a number
            }

            for (int j = 0; j < codeMatrix.size(); j++) {
            	
                // Check for references to the variable
                if (codeMatrix[j][2] == codeMatrix[i][0]) {
                    variableIsUsed = true; // Variable is used
                    codeMatrix[j][1] = DecimalToHexString(to_string(variablePosition)); // Update reference with the variable's memory position
                }
                
                // Check for duplicate variable declarations
                if (codeMatrix[j][1] == "=" && codeMatrix[j][0] == codeMatrix[i][0] && j != i) {
                    ErrorString(11, j); // Error: Variable redefined within the code
                }
            }
            // Warn if the variable has been declared but not used in the code.
            if (!variableIsUsed) {
                WarningString(1, i); // Warning for unused variable
            }
        }
    }
}

/*
Check if a variable used in the code has been declared

@param pos - The line number containing the instruction that uses the variable
*/
void CheckVariableDeclaration(int pos) {
    bool found = false;

    // Iterate through the codeMatrix to check if the variable has been declared
    for (int j = 0; j < codeMatrix.size(); j++) {
        if (codeMatrix[j][0] == codeMatrix[pos][2]) {
            found = true;
            break;
        }
    }

    if (!found && codeMatrix[pos][2] != "NULL") {
        ErrorString(9, pos); // Error: Variable used but not declared
    }
}

/*
Display the analysis results: content of memory 1 (reserved for data), content of memory 2 (reserved for instructions), any errors, and any warnings
*/
void ShowResults() {
	
	// Get the console handle for debugging output
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  
    
    // Prepare error and warning summary strings
    string zeroErrors = "Errors: " + to_string(errorCounter) + "\n";       // Summary of errors
    string zeroWarns = "Warnings: " + to_string(warningCounter) + "\n\n";  // Summary of warnings
    
    // Prepare headers for instructions and data
    string instructions = "MEM2 - Instructions\n";
    string dataText = "MEM1 - Data\n";
    

    for (int i = 0; i < codeMatrix.size(); i++) {
    	
        // Skip labels, variable assignments, device declarations and function calls
        if (codeMatrix[i][0] != "NULL" && !EndsWith(codeMatrix[i][0], ":") && codeMatrix[i][1] != "=" && codeMatrix[i][0] != "<" && codeMatrix[i][0] != ">" && !StartsWith(codeMatrix[i][0], "//")) {
            
            // If the hexadecimal code corresponding to the instruction is not 3 digits, apply padding
            while (codeMatrix[i][1].size() < 4) {
                codeMatrix[i][1] = "0" + codeMatrix[i][1];
            }
            
            // Append instruction and memory address to their respective strings
            instructions.append(codeMatrix[i][0]).append("  ");
            dataText.append(codeMatrix[i][1]).append("  ");
        }
    }
    
    // Process and append variable values to the dataText
    for (int j = 0; j < codeMatrix.size(); j++) {
        if (codeMatrix[j][1] == "=") {
        	
            // Ensure variable values are 4 characters long
            while (codeMatrix[j][2].size() < 4) {
                codeMatrix[j][2] = "0" + codeMatrix[j][2];
            }
            dataText.append(codeMatrix[j][2]).append("  ");
        }
    }

    // Finalize instruction and data output
    instructions.append("\n\n");
    dataText.append("\n\n").append(instructions);
    
    // Clear the label text before updating
    SetWindowText(hLabel, "");

    // Display results in the label with color coding based on error presence
    if (errorCounter == 0) {
    	
        // No errors: show data and instructions, then errors and warnings
        SetRichEditText(hLabel, dataText, RGB(204, 229, 255));   // Display data and instructions in light blue
        SetRichEditText(hLabel, zeroErrors, RGB(255, 0, 0));     // Display errors in red
        SetRichEditText(hLabel, zeroWarns, RGB(255, 255, 0));    // Display warnings in yellow
		SetRichEditText(hLabel, warningText, RGB(255, 255, 0));  // Show warning details in yellow    
	} else {
		
        // Errors exist: only show errors and warnings
        SetRichEditText(hLabel, errorText, RGB(255, 0, 0));      // Show error details in red
        SetRichEditText(hLabel, warningText, RGB(255, 255, 0));  // Show warning details in yellow
    }
    
    // If debugging is enabled, output error and warning text to the console
    if (DEBUG) {
        SetConsoleTextAttribute(hConsole, 12);                   // Set console text color to red for errors
        cout << errorText << endl;                               // Output error text
        SetConsoleTextAttribute(hConsole, 14);                   // Set console text color to yellow for warnings
        cout << warningText << endl;                             // Output warning text
        SetConsoleTextAttribute(hConsole, 15);                   // Reset console text color to white
    }
}

// Error and warning reporting ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------  

/*
Create the string containing the errors from the user's written code

@param errorCode - Represent the error code corresponding to a specific message
@param line - Indicate the line number where the error was detected
*/
void ErrorString(int errorCode, int line) {
    
	// Increment the error counter
    errorCounter++;
    
    string errorCodeString = to_string(errorCode);
    while(errorCodeString.size() < 3){
    	errorCodeString = "0" + errorCodeString;
	}
    
    errorText.append("Line: ").append(to_string(line + 1)).append(" | Error E-").append(errorCodeString).append("\n");
    
    switch (errorCode) {
        case 0:
            errorText.append("Unable to open the file. The specified file might be corrupted or inaccessible.\n");
            break;
        case 1:
            errorText.append("The size of the code in the file exceeds the memory capacity of the Flip01 processor (64 kB).\n");
            break;
        case 2:
            errorText.append("The instruction \"").append(codeMatrix[line][0]).append("\" is either written incorrectly or not supported.\n");
            break;
        case 3:
            errorText.append("This instruction contains an invalid argument. A variable is expected as the second parameter.\n");
            break;
        case 4:
            errorText.append("This instruction has fewer arguments than expected.\n");
            break;
        case 5:
            errorText.append("This instruction does not have a general-purpose register (AX or BX) as the first argument, even though it requires one.\n");
            break;
        case 6:
            errorText.append("This instruction has more arguments than expected.\n");
            break;
        case 7:
            errorText.append("This value associated with this immediate instruction exceeds the calculation capacity of the Flip01 processor (n = [0,255]).\n");
            break;
        case 8:
            errorText.append("This value associated with this immediate instruction is not an integer.\n");
            break;
        case 9:
            errorText.append("This variable has not been declared or defined at the beginning of the code.\n");
            break;
        case 10:
            errorText.append("This variable has not been defined correctly: The associated value is not a number.\n");
            break;
        case 11:
            errorText.append("This variable is defined multiple times in the code.\n");
            break;
        case 12:
            errorText.append("The value associated with this variable exceeds the calculation capacity of the Flip01 processor (n = [0,255]).\n");
            break;
        case 13:
            errorText.append("This label is referenced but has not been defined.\n");
            break;
        case 14:
            errorText.append("This label is defined multiple times in the code.\n");
            break;
        case 15:
            errorText.append("Too many input devices declared. Flip01 supports a maximum of 32 input devices.\n");
            break;
        case 16:
            errorText.append("Too many output devices declared. Flip01 supports a maximum of 32 output devices.\n");
            break;
        case 17:
        	errorText.append("The name assigned to this variable is a number. Variable names must contain at least one letter.\n");
            break;
        case 18:
            errorText.append("This device was declared as input but is being used as output.\n");
            break;
        case 19:
            errorText.append("This device was declared as output but is being used as input.\n");
            break;
        case 20:
            errorText.append("This device is being used but wasn't declared.\n");
            break;
        case 21:
            errorText.append("The name assigned to this label is a number. Label names must contain at least one letter.\n");
            break;
        case 22:
            errorText.append("The name assigned to this device is a number. Device names must contain at least one letter.\n");
            break;
        case 23:
            errorText.append("No name has been assigned to this I/O operation.\n");
            break;
        case 24:
            errorText.append("The name assigned to this device is already in use by another device.\n");
            break;
        case 25:
            errorText.append("The port number assigned to this I/O device has already been assigned to another device.\n");
            break;
        case 26:
            errorText.append("The field for the port number of this device does not contain an integer.\n");
            break;
        case 27:
            errorText.append("The port number assigned to this I/O device is too high. Flip01 supports up to 32 input devices (0 - 31) and up to 32 output devices (0 - 31).\n");
            break;
        default:
            break;
    }
    
    errorText.append("\n");
}


/*
Create the string containing the warnings from the user's written code

Parameters:
@param warningCode - Represent the warning code corresponding to a specific message
@param line - Indicate the line number where the warning was found
*/
void WarningString(int warningCode, int line) {
	
    // Increment the warning counter
    warningCounter++;
    
    warningText.append("W").append(to_string(warningCode)).append(" Warning | Line: ").append(to_string(line + 1)).append("\n");
    
    switch (warningCode) {
        case 0:
            warningText.append("This label is not referenced anywhere in the code\n");
            break;
        case 1:
            warningText.append("This variable is not used in the code\n");
            break;
        case 2:
            warningText.append("This device was declared but isn't used in the code.\n");
            break;
        default:
            break;
    }
    
    warningText.append("\n");
}

// File management ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
Select a random phrase from the specified file
 
@param filePath - The path of the text file to read
@param randomLine - Reference to the string that will contain the selected phrase
@return true if a line was successfully retrieved, false otherwise
 */
bool GetRandomLineFromFile(const string& filePath, string& randomLine) {
    
	// Open the file for reading
	ifstream file(filePath); 

    // Check if the file was opened successfully
    if (!file) {  
        if (DEBUG) {
            cout << "Failed to open file: " << filePath << endl;
        }
        return false;
    }

	// Array of strings to store the lines read from the file
    vector<string> lines;  
    string line;

    // Read each line from the file and add it to the vector
    while (getline(file, line)) {
        lines.push_back(line);  // Store each line in the vector
    }

    // Check if the lines array is empty (due to an empty file or all lines from the file having already been read)
    if (lines.empty()) {
        if (DEBUG) {
            cout << "File is empty or unable to read lines: " << filePath << endl;
        }
        return false;  // Return false to indicate failure
    }

    // Seed the random number generator with the current time
    srand(static_cast<unsigned>(time(0)));  
    
    // Select a random line from the vector of lines
    randomLine = lines[rand() % lines.size()];  

    return true;  // Return true to indicate success
}

// Ciao

/*
Save the current code to the text file specified by the user 
If a save has already been performed, subsequent saves will occur in the same file
*/
void SaveTextToFile() {
    OPENFILENAME ofn;                    // Common dialog box structure for file saving
    CHAR szFile[260] = {0};              // Buffer to hold the file name
    string currentFilePath;              // String to store the selected file path

    // Initialize the OPENFILENAME structure
    ZeroMemory(&ofn, sizeof(ofn));                            // Clear the memory for the dialog structure
    ofn.lStructSize = sizeof(ofn);                            // Set the size of the structure
    ofn.hwndOwner = NULL;                                     // No specific window owner for the dialog
    ofn.lpstrFile = szFile;                                   // Buffer to receive the file name
    ofn.nMaxFile = sizeof(szFile);                            // Maximum length of the file name
    ofn.lpstrFilter = "Text Files\0*.TXT\0All Files\0*.*\0";  // Filter to show text files and all files
    ofn.nFilterIndex = 1;                                     // Default filter index for "Text Files"
    ofn.lpstrFileTitle = NULL;                                // No specific file title
    ofn.nMaxFileTitle = 0;                                    // Maximum length for the file title
    ofn.lpstrInitialDir = NULL;                               // Default initial directory
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;      // File path must exist and prompt before overwriting

    // If currentFilePath is empty, show the Save File dialog
    if (currentFilePath.empty()) {
        if (GetSaveFileName(&ofn)) {          // Display the Save File dialog
            currentFilePath = ofn.lpstrFile;  // Store the file path chosen by the user
        } else {
            return;                           // User canceled the dialog, exit the function
        }
    }

    // Ensure the file has a ".txt" extension
    if (currentFilePath.size() < 4 || currentFilePath.substr(currentFilePath.size() - 4) != ".txt") {
        currentFilePath += ".txt";            // Append ".txt" extension if not present
    }

    // Open the file for writing
    ofstream outFile(currentFilePath);        // Open the file for writing
    
    // Check if the file was opened successfully
    if (outFile.is_open()) {  
        outFile << textBuffer;                                                   // Write the contents of textBuffer to the file
        outFile.close();                                                         // Close the file
        MessageBox(NULL, "File Saved Correctly", "Saved!", MB_OK);               // Notify user of successful save
    } else {
        MessageBox(NULL, "Failed to save file", "Oh no", MB_OK | MB_ICONERROR);  // Notify user of failure
    }
}

/*
Open a text file, read its content, and display it in the hTextBox control
 
@param hwnd - Handle to the parent window, used to specify the owner of the dialog box
@param hTextBox - Handle to the text box where the file content will be loaded
*/
void OpenFileAndLoadText(HWND hwnd, HWND hTextBox) {
    OPENFILENAME ofn;               // Common dialog box structure
    char szFile[260];               // Buffer for file name
    HANDLE hFile;                   // File handle

    // Initialize OPENFILENAME structure
    ZeroMemory(&ofn, sizeof(ofn));                            // Clear the memory for the dialog structure
    ofn.lStructSize = sizeof(ofn);                            // Set the size of the structure
    ofn.hwndOwner = hwnd;                                     // Owner window handle for the dialog
    ofn.lpstrFile = szFile;                                   // Buffer to receive the file name
    ofn.lpstrFile[0] = '\0';                                  // Initialize the buffer to an empty string
    ofn.nMaxFile = sizeof(szFile);                            // Maximum file name length
    ofn.lpstrFilter = "Text files\0*.TXT\0All files\0*.*\0";  // Filter for file types
    ofn.nFilterIndex = 1;                                     // Default filter is "Text files"
    ofn.lpstrFileTitle = NULL;                                // No specific file title
    ofn.nMaxFileTitle = 0;                                    // Maximum file title length
    ofn.lpstrInitialDir = NULL;                               // No specific initial directory
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;        // File must exist

    // Display the Open File dialog box
    
    // If the user selects a file and confirms
    if (GetOpenFileName(&ofn) == TRUE) {  
    
        // Open the selected file for reading
        hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        // Check if the file was opened successfully
        if (hFile != INVALID_HANDLE_VALUE) {
        	
            // Get the size of the file
            DWORD fileSize = GetFileSize(hFile, NULL);
            
            // Ensure the file size is valid
            if (fileSize != INVALID_FILE_SIZE) {  
            
                // Allocate a buffer to hold the file content
                char* buffer = new char[fileSize + 1];
                DWORD bytesRead;

                // Read the file content into the buffer
                if (ReadFile(hFile, buffer, fileSize, &bytesRead, NULL)) {
                    buffer[bytesRead] = '\0';  // Null-terminate the buffer
                    SetWindowText(hTextBox, buffer);  // Set the text in the text box
                }

                delete[] buffer;    // Clean up the allocated buffer
            }

            CloseHandle(hFile);     // Close the file handle
            currentFilePath = "";   // Clear the current file path
        }
        else {
        	
            // Handle file open error
            errorText = "";
            ErrorString(0, 0);                                   // Error: Unable to open the file
            SetWindowText(hLabel, "");                           // Clear the label
            SetRichEditText(hLabel, errorText, RGB(255, 0, 0));  // Display the error message in red
        }
    }
}

/*
Function to save the matrix to a temporary file
*/
void SaveMatrixToFile() {
    // Retrieve the path to the directory containing the executable
    char exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    // Convert the path to a string and extract the directory path
    string directoryPath(exePath);
    size_t lastBackslashPos = directoryPath.find_last_of("\\/");
    directoryPath = directoryPath.substr(0, lastBackslashPos);

    // Create the full path to log.txt in the same directory as the executable
    string logFilePath = directoryPath + "\\log.txt";

    // Open 'log.txt' in write mode (ios::trunc to clear existing contents if any)
    ofstream file(logFilePath, ios::trunc);

    // Check if file opened successfully
    if (DEBUG){
    	if (!file.is_open()) {
        	cout << "Error: Unable to open or create log.txt" << endl;
        	return;
    	}
	}
    // Write the contents of the matrix to log.txt
    for (int i = 0; i < codeMatrix.size(); i++) {
        for (int j = 0; j < codeMatrix[i].size(); j++) {
        	
        	if (codeMatrix[i][j] != "NULL" && !StartsWith(codeMatrix[i][0], "//"))
            	file << codeMatrix[i][j] << " ";
        }
        file << endl;
    }

	// Close the file
    file.close();
    
    if(DEBUG){
    	cout << "Matrix saved to log.txt" << endl;
	}
}

// Rich Edit controls ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
Sets the text and its color in a Rich Edit control
 
@param hErrorControl - Handle to the Rich Edit control
@param text - The text to be inserted into the Rich Edit control
@param color - The color to be applied to the text (using RGB color values)
 */
void SetRichEditText(HWND hErrorControl, const string& text, COLORREF color) {
    
	// CHARRANGE structure to specify the range of text to format
    CHARRANGE cr;
    cr.cpMin = -1;  // Position the cursor at the end of the text
    cr.cpMax = -1;
    
    // Send message to select the range (end of text)
    SendMessage(hErrorControl, EM_EXSETSEL, 0, (LPARAM)&cr);

    // CHARFORMAT structure to define text formatting
    CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);        // Set the size of the CHARFORMAT structure
    cf.dwMask = CFM_COLOR;                 // Specify which attributes to apply (text color in this case)
    cf.crTextColor = color;                // Set the text color
    cf.dwEffects = 0;                      // Reset all text effects

    // Send message to apply character formatting to the selection
    SendMessage(hErrorControl, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    
    // Send message to replace the selected text (in this case, appending text) 
    SendMessage(hErrorControl, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
}

/*
Append text to an EDIT control

@param hTextBoxControl - Handle to the EDIT control
@param textToAppend - The text to be appended to the control
*/
void AppendTextToEditControl(HWND hTextBoxControl, const string& textToAppend) {
    
	// Get the length of the current text in the edit control
    int textLength = GetWindowTextLength(hTextBoxControl);
    
    // Set the selection range to the end of the existing text
    SendMessage(hTextBoxControl, EM_SETSEL, (WPARAM)textLength, (LPARAM)textLength);
    
    // Add the new text specified as a parameter
    SendMessage(hTextBoxControl, EM_REPLACESEL, FALSE, (LPARAM)textToAppend.c_str());
}

// Other functions -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
Convert a string with numeric content to its corresponding hexadecimal value

@param decimalStr - Contains the number to be converted
@return A string containing the number converted to hexadecimal
*/
string DecimalToHexString(const string& decimalStr) {
	
	// Convert the string to an integer
    int decimal = stoi(decimalStr);
    
    // Create a string stream for formatting
    stringstream ss;
    
	// Format the integer as a two-digit uppercase hexadecimal string
    ss << hex << uppercase << setw(2) << setfill('0') << decimal; 
    
	// Return the formatted string
	return ss.str(); 
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

/*
Check if a string starts with a specific prefix

@param str - The string to be checked
@param prefix - The prefix to consider
@return true if the specified string (str) starts with the specified prefix (prefix), false otherwise
*/
bool StartsWith(const string& str, const string& prefix) {
    
	// If the prefix is longer than the string, it cannot be a prefix
	if (prefix.length() > str.length()) {
        return false;
    }
    
    // Check if the beginning of the string matches the prefix
    return str.compare(0, prefix.length(), prefix) == 0;
}

/*
Check if a string ends with a specific suffix

@param str - The string to be checked
@param suffix - The suffix to consider
@return true if the specified string (str) ends with the specified suffix (suffix), false otherwise
*/
bool EndsWith(string const &str, string const &suffix) {
	
	// If the suffix is longer than the string, it cannot be a suffix
    if (str.length() < suffix.length()) {
        return false; 
    }
    
    // Check if the end of the string matches the suffix
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

// Handling of external programs ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
Function to check if a process is running

@param processInfo - A reference to a PROCESS_INFORMATION structure containing the process handle (hProcess) and thread handle (hThread).
@return true if the process is still running, false otherwise. Cleans up handles if the process has terminated.
*/
bool IsProcessRunning(PROCESS_INFORMATION& processInfo) {
    // If the process handle is NULL, the process is not running
    if (processInfo.hProcess == NULL) return false;

    DWORD exitCode;  // Variable to store the process exit code
    // Retrieve the exit code of the specified process
    GetExitCodeProcess(processInfo.hProcess, &exitCode);

    // If the process is still active, return true
    if (exitCode == STILL_ACTIVE) {
        return true;
    } else {
        // If the process is not active (terminated), close the handles
        CloseHandle(processInfo.hProcess);  // Close the process handle
        CloseHandle(processInfo.hThread);   // Close the thread handle

        // Reset the process and thread handles to NULL to indicate no active process
        processInfo.hProcess = NULL;
        processInfo.hThread = NULL;

        return false;  // Return false indicating the process has ended
    }
}

/*
Function to start the conversion program

@return none
*/
void StartConverter() {
    // Check if the first program is not already running
    if (!IsProcessRunning(processInfo1)) {
        // Set the path to the first external program (Converter.exe)
        TCHAR programPath[MAX_PATH] = TEXT("Converter.exe");

        // Set up the STARTUPINFO structure for the new process
        STARTUPINFO si = { sizeof(si) };
        ZeroMemory(&si, sizeof(si));  // Clear the structure
        si.cb = sizeof(si);  // Set the size of the STARTUPINFO structure

        // Try to create the new process (start the program)
        if (CreateProcess(NULL, programPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo1)) {
            // Process started successfully (no action required)
        } else {
            // If the process fails to start, show an error message
            MessageBox(NULL, TEXT("Failed to start the conversion table program."), TEXT("Error"), MB_OK | MB_ICONERROR);
        }
    } else {
        // If the program is already running, show an informational message
        MessageBox(NULL, TEXT("The conversion table program is already running."), TEXT("Information"), MB_OK | MB_ICONINFORMATION);
    }
}

/*
Function to show the manual (open a URL in the default web browser)

@return none
*/
void ShowManual() {
    // Message to display in the confirmation dialog box
    int result = MessageBox(NULL, 
    	TEXT("This will open a tab in your browser to show the free manual. Is this ok?"), 
        TEXT("Open Manual"), 
        MB_YESNO | MB_ICONQUESTION
	);

    // Check the result of the message box
    if (result == IDYES) {
        // URL to open in the web browser
        std::string url = "https://medium.com/@biasolo.riccardo/flip01-a-simple-yet-versatile-8-bit-cpu-fc01c36d5922";

        // Open the URL in the default web browser using system command
        system(("start " + url).c_str());
    }
    // If the user clicked "No", the function does nothing and just exits.
}

/*
Function to terminate child processes

@return none
*/
void TerminateChildProcesses() {
    // Check if the first program is running
    if (IsProcessRunning(processInfo1)) {
        // Terminate the process if it is running
        TerminateProcess(processInfo1.hProcess, 0);  // Terminate with exit code 0

        // Close the handles for the process and its thread
        CloseHandle(processInfo1.hProcess);
        CloseHandle(processInfo1.hThread);

        // Reset the process and thread handles to NULL to indicate no active process
        processInfo1.hProcess = NULL;
        processInfo1.hThread = NULL;
    }
}


// - Ric :)

