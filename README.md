# print-rj
Program for printing timetables from the Train Driver 2 game.
The program supports dot matrix printers with Epson FX and IBM PPR emulation.

# QUICK_SETUP.TXT

1. Requirements
2. Compiled versions delivered to the end user
3. Setting up the printer
	3.1. Setting up the printer printer with printing on perforation
		3.1.1. Using PERFORATION_CALIBRATION.CMD to calibrate the printer
	3.2. Setting up the printer with printing outside perforation / Setting up the printer with printing on A4 sheet
4. Using PRINT-RJ.EXE to print the timetable
5. A short description of the source code
6. LICENSE

---------------------------------------------------------------------------------------------------------------------
1. Requirements

	The PRINT-RJ.EXE requires:
		- Dot matrix printer:
			> with CP852 / LATIN II / ISO 8859-2 code page support
			> and with character pitch support: >=12 CPI
			> With support for codes from IBM Proprinter OR EPSON FX
		- Operating System: Microsoft Windows (32 OR 64 bit)
		- Internet connection.
	
---------------------------------------------------------------------------------------------------------------------
2. Compiled versions delivered to the end user

	The program should be compiled using static runtime libraries, not the DLL version.
	If not, because the program requires "Visual C++ Redistributable Package", please contact me:
		Anonim17PL
		E-Mail: anonim17pl@gmail.com
	
	Depending on the needs, three versions of the program can be delivered:
		PRINT-RJ_PPR.ZIP - Program with IBM Proprinter codes emulation.
		PRINT-RJ_FX.ZIP - Program with EPSON FX codes emulation.
		PRINT-RJ_FX_DH.ZIP - Program with EPSON FX codes emulation and with Double Height function.
		
---------------------------------------------------------------------------------------------------------------------
3. Setting up the printer

	Make sure the printer has the following features turned off:
		- "Auto Form Feed"
		- "Auto Carriage Return"
		- "Auto Line Feed"
		
	Make sure the printer has the following features turned on:
		- "Code Page: East Europe Latin II-852" or compatible
		
	3.1. Setting up the printer printer with printing on perforation

		Step 1. Make sure that printing on the perforation of the paper is enabled:
			OKI Printers:
				Make sure the printer has the following features turned off:
					> "Skip Over Perforation"
			Printers from other manufacturers:
				See the manual for your printer or contact the printer manufacturer.
		Step 2. Prepare the printer for printing and let it set the paper position automatically.
		Step 3. Run PERFORATION_CALIBRATION.CMD (See section 3.1.1. of this manual) and calibrate the printer.
		
	3.1.1. Using PERFORATION_CALIBRATION.CMD to calibrate the printer
	
		Make sure the printer has the following disabled while it is calibrating:
			- "Auto Tear"
		
		A printer calibration program is supplied with the program.
		It allows you to calibrate the position of the paper in the printer,
		thanks to which it is possible to tear off continuous paper 
		with perforation while maintaining the legibility of the printout.
	
		Step 1. Follow the instructions in section 3.1. of this manual.
		Step 2. Run PERFORATION_CALIBRATION.CMD. The program will print a sequence of characters intended for calibration.
		Step 3. Make sure a solid line is printed on the perforation of the paper.
			STEP 3.1 If not, calibrate the paper position in the printer
					(see the printer's instruction manual to know how to do this).
		Step 4. Use FORM FEED command
		Step 5. Use TEAR command
		Step 6. Tear off the paper at the perforation.
		Step 7. If you have calibrated the paper position in the printer, check again. Go to step 2.
		
	3.2. Setting up the printer with printing outside perforation / Setting up the printer with printing on A4 sheet

		1. Make sure that printing on the perforation of the paper is disabled:
			OKI Printers:
				Make sure the printer has the following features turned on:
					> "Skip Over Perforation"
			Printers from other manufacturers:
				See the manual for your printer or contact the printer manufacturer.
		2. Prepare the printer for printing.

---------------------------------------------------------------------------------------------------------------------
4. Using PRINT-RJ.EXE to print the timetable

	Step 1. Connect to the Internet
	Step 2. Run PRINT-RJ.EXE
	Step 3. The program will ask you for the train number. Enter a valid number without category.
	Step 4. The program will download the Internet database and analyze it.
			After a while, program should ask about the first (LOK1), second (LOK2), third (LOK3) series of locomotive in the train.
			If there is no second or third locomotive, enter the x (lower case letter)
	Step 5. The program will ask about the permissible maximum speed of the entire train set. Enter an integer in units of kilometers per hour
			
			EXAMPLE:
				TRAIN NUMBER: 22002
				LOK1: EU07
				LOK2: x
				LOK3: x
				Vmax=125
				
			EXAMPLE:
				TRAIN NUMBER: 340122
				LOK1: ET41
				LOK2: x
				LOK3: x
				Vmax=100
				
			EXAMPLE:
				TRAIN NUMBER: 2137
				LOK1: EP08
				LOK2: EP08
				LOK3: x
				Vmax=140
				
	Step 6. The program will create a string and pass it to the first parallel port (LPT1).
			After passing the string, the program will close and the operating system will take care of controlling the parallel port
			
	If the program ends after entering the train number, make sure that:
		> Internet connection is available.
		> The train number is correct.
		> The train number exists in the JSON file located on the API server. (https://rj.td2.info.pl/live)
		> LibCurl certificate file named CACERT.PEM is exsist
		
	If you have made sure that all the above-mentioned conditions are met and the program still does not work, please contact me:
		Anonim17PL
		E-Mail: anonim17pl@gmail.com
		
---------------------------------------------------------------------------------------------------------------------
5. A short description of the source code

Source code has the ability to easily set up predefined printer codes and additional options (available depending on the emulation mode selected).
You can easily create new "code tables" with the help of #define.

6. LICENSE

MIT License

Copyright (c) 2021 Anonim17PL

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


