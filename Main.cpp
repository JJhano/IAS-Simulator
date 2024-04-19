//Adrian Barberis

//COSC 2150

//IASSimulator

//Nov 13 2016
//Main program

//Note 1: Complete except for strange hang when running 10Test1.txt needs looking into

//Edit 1 - Dec 2 2016:  Fixed some specifications that I had overlooked or read wrongly, cleaned up the code 

//Note 2:  Hang seems to have fixed itself which seems to indicate a problem with my home version of Visual Studio
//				Since when I ran this on school computer No hang but home computer there was a hang.  Will double check to be sure.

#include<fstream>
#include<iostream>
#include<regex>
#include"Memory.h"

//+---------------------------------+

//|		 Function Declarations		|

//+---------------------------------+

//decodes the instructions and sends them to execute

void decode(std::string instruction, std::match_results<std::string::const_iterator> results);
//Based on information passed to it by decode() executes the appropriate action
void execute(int casenum, int memory_location);
//Reads in a file and stores the contents in the Memory class:
//		when it finds 'begin' it sets the PC to that index location
//		stores instructions in memory using line number as index
//		Returns the last line number for use by printMemory() function
int read(std::ifstream& file, Memory* memory, std::match_results<std::string::const_iterator> results);
//prints contents of Memory class array
void printMem(Memory& memory, int line_number);
//print pre execution
void printExec1();
//print post execution
void printExec2();
void printMessage(std::string message);

//print code
void printCode(std::string filename);
//helper function: converts string to int 
//(this was just here to make code look cleaner 
//not sure why anymore and i wont rewrite 30+ lines of code
//Just to change one already working thing)
int stringToInt(std::string string);

//+-------------------------+
//|		Global Variables    |
//+-------------------------+
//These are global on purpose
//You could make them private but this would mean that methods like execute() 
// would take more than the 2 alloted parameters

int program_counter = 0;
std::string instruction_register;
std::string memory_buffer_register;
int memory_address_register;
std::string accumulator_register = "0";
std::string multiplier_quotient_register = "0";
Memory memory;
std::match_results<std::string::const_iterator> results;
//+-------------+
//|		main    |
//+-------------+
int main(int argc, char* argv[]){
	if(argc != 2){
		printf("Usage: %s <filename>\n", argv[0]);
		return EXIT_FAILURE;
	}
	std::string filename = argv[1];
	printf("FILENAME: %s\n", filename.c_str());
	//-----------------------------------------
	printMessage("Code");
	printCode(filename);
	std::ifstream infile(filename);
	printf("Aun no lee..\n");
	int line_number = read(infile, &memory, results);
	printf("Leyo..\n");
	//----------------------------------------
	printMessage("Execution");
	//+--------------------------------------+

	//|				FETCH/EXECUTE            |

	//+--------------------------------------+
	while (program_counter < memory.size()){
		if (memory.get(program_counter) == "halt"){
			break;
		}
		memory_address_register = program_counter;
		memory_buffer_register = memory.get(program_counter);
		instruction_register = memory_buffer_register;
		program_counter++;
		printExec1();
		decode(memory.get(memory_address_register), results);
		printExec2();
	}
	//----------------------------------------
	printMessage("Memory");
	//----------------------------------------
	//print memory after execution
	printMem(memory, line_number);
	//----------------------------------------
	std::cout << std::endl;
	return 0;
}
//+------------------------------------+

//|		  Function Definitions         |

//+------------------------------------+
int read(std::ifstream& file, Memory* memory, std::match_results<std::string::const_iterator> results){
	//If file failed to open just exit
	if (!file.is_open()){
		std::cout << "Error: File is NOT OPEN;  Exiting..." << std::endl;
		exit(0);
	}
	int line_num = 0;
	while (true){
		//if end of file break
		if (file.eof())		{
			break;
		}
		std::regex fullLine("(\\d+)(.*)");
		std::regex comment("(.*)(\\.)(.*)");
		std::regex isSingleLetter("\\s?([a-z]{1})");
		std::string instruction;
		std::string line;
		//get whole line
		std::getline(file, line);
		if(line == ""){
			continue;
		}
		//check for regex
		std::regex_match(line, results, fullLine);
		//set line  equal to everything but the number
		instruction = results[2];
		//convert the line number from string to int
		std::cout << "ASD: " << results[1] << std::endl;
		// printf("ASD: %s",results[1]);
		line_num = std::stoi(results[1]);
		//remove all whitespace from non comment instruction
		if (!std::regex_match(line, results, comment)){
			instruction.erase(std::remove_if(instruction.begin(), instruction.end(), isspace), instruction.end());
		}
		//if instruction is 'begin' set PC to that line number
		//This is in keeping with directions for hmwrk 9 Q&A suggestions #3
		if (instruction == "begin")		{
			program_counter = line_num;
		}
		//if instruction is single letter set that variable's starting value to zero
		//variables without a value should always be initialized to 0
		if (std::regex_match(instruction, results, isSingleLetter))		{
			instruction = "0" + instruction;
		}
		//if the instruction is -2b or 2b for example  remove the 'b' and keep everything else
		//This is in keeping with the layout for the IAS simulator used throught the year for the various Hmwrks
		//While you could write comments after variables in that simulator the eventual memory print ommited those
		//comments entirely. The following code snippet emulates that process as well as making eventual execution far less complex
		std::regex variable("(-)?(\\d+)(.*)");
		if (std::regex_match(instruction, results, variable))		{
			std::string x = results[1];
			std::string y = results[2];
			std::string temp = x + y;
			instruction = temp;
		}
		//push to memory
		memory->set(instruction, line_num);
	}
	file.close();
	//return the last line number read + 1 (useful for print method)
	//This way I don't have to find the first 'halt' all over again instead I simply 
	//loop the print method using this line number, i.e. the final line number, as the upper limit
	return line_num + 1;
}
void decode(std::string instruction, std::match_results<std::string::const_iterator> results){
	//Regular expression definition for each command
	std::regex loadMQ("load\\s?MQ");
	std::regex loadMQMX("load\\s?MQ,M\\((\\d+)\\)");
	std::regex storMX("stor\\s?M\\((\\d+)\\)");
	std::regex loadMX("load\\s?M\\((\\d+)\\)");
	std::regex loadNegMX("load\\s?-M\\((\\d+)\\)");
	std::regex loadAbsMX("load\\s?\\|M\\((\\d+)\\)\\|");
	std::regex loadNegAbsMX("load\\s?-\\|M\\((\\d+)\\)\\|");
	std::regex comment("(.*)(\\.)(.*)");
	std::regex jump("jump\\s?M\\((\\d+)\\)");
	std::regex jumpPlus("jump\\+\\s?M\\((\\d+)\\)");
	std::regex addMX("add\\s?M\\((\\d+)\\)");
	std::regex addAbsMX("add\\s?\\|M\\((\\d+)\\)\\|");
	std::regex subMX("sub\\s?M\\((\\d+)\\)");
	std::regex subAbsMX("sub\\s?\\|M\\((\\d+)\\)\\|");
	std::regex mulMX("mul\\s?M\\((\\d+)\\)");
	std::regex divMX("div\\s?M\\((\\d+)\\)");
	std::regex lsh("lsh");
	std::regex rsh("rsh");

	if (std::regex_match(instruction, results, loadMQMX)){
		int loc = stringToInt(results[1]);
		execute(1, loc);

	}
	else if (std::regex_match(instruction, results, storMX)){
		int loc = stringToInt(results[1]);
		execute(2, loc);
	}
	else if (std::regex_match(instruction, results, loadMX)){
		int loc = stringToInt(results[1]);
		execute(3, loc);
	}
	else if (std::regex_match(instruction, results, loadNegMX)){
		int loc = stringToInt(results[1]);
		execute(4,loc);
	}
	else if (std::regex_match(instruction, results, loadAbsMX)){
		int loc = stringToInt(results[1]);
		execute(5, loc);
	}
	else if (std::regex_match(instruction, results, loadNegAbsMX)){
		int loc = stringToInt(results[1]);
		execute(6, loc);
	}
	else if (std::regex_match(instruction, results, loadMQ)){
		execute(7,999);
	}
	else if (std::regex_match(instruction, results, jump)){
		int loc = stringToInt(results[1]);
		execute(8, loc);
	}
	else if (std::regex_match(instruction, results, jumpPlus)){
		int loc = stringToInt(results[1]);
		execute(9, loc);
		return;
	}
	else if (std::regex_match(instruction, results, addMX)){
		int loc = stringToInt(results[1]);
		execute(10, loc);
	}
	else if (std::regex_match(instruction, results, addAbsMX)){
		int loc = stringToInt(results[1]);
		execute(11, loc);
	}
	else if (std::regex_match(instruction, results, subMX)){
		int loc = stringToInt(results[1]);
		execute(12, loc);
	}
	else if (std::regex_match(instruction, results, subAbsMX))
	{
		int loc = stringToInt(results[1]);
		execute(13, loc);
	}
	else if (std::regex_match(instruction, results, mulMX))
	{
		int loc = stringToInt(results[1]);
		execute(14, loc);
	}
	else if (std::regex_match(instruction, results, divMX))
	{
		int loc = stringToInt(results[1]);
		execute(15, loc);
	}
	else if (std::regex_match(instruction, results, lsh))
	{
		execute(16, program_counter);
	}

	else if (std::regex_match(instruction, results, rsh))
	{
		execute(17, program_counter);
	}
	else if (std::regex_match(instruction, results, comment))
	{
		return;
	}
	else if (instruction == "begin" || instruction == "nop")
	{
		return;
	}
	else
	{
		std::cout << "Error: Instruction CANNOT be decoded! " << std::endl;
		std::cout << "Exiting..." << std::endl;
		exit(0);
	}
	return;
}

void execute(int casenum, int memory_location){
	//case 1:  load MQ,M(x)
	//case 2:  stor M(x)
	//case 3:  load M(x)
	//case 4:  load -M(x)
	//case 5:  load |M(x)|
	//case 6:  load -|M(x)|
	//case 7:  load MQ
	//case 8:  jump M(x)
	//case 9:  jump+ M(x)
	//case 10: add M(x)
	//case 11: add |M(x)|
	//case 12: sub M(x)
	//case 13: sub |M(x)|
	//case 14: mul M(x)
	//case 15: div M(x)
	//case 16: lsh
	//case 17: rsh
	switch (casenum){
		//multiplier quotient  = the contents of memory at x
		case(1): {
			multiplier_quotient_register = memory.get(memory_location);
			return;
		}
		//store the accumulator register value at memory location x
		case(2): {
			memory.set(accumulator_register, memory_location);
			return;
		}
		//load value of memory_location 'x' into accumulator register
		case(3): {
			accumulator_register = memory.get(memory_location);
			return;
		}
		//load negative value of memory_location 'x' into accumulator register
		case(4): {
			int num = std::stoi(memory.get(memory_location));
			num = -num;
			accumulator_register = std::to_string(num);
			return;
		}
		//load absolute value of memory_location 'x' into accumulator register
		case(5): {
			int num = std::stoi(memory.get(memory_location));
			num = abs(num);
			accumulator_register = std::to_string(num);
			return;
		}
		//load negative of absolute value of memory_location 'x' into accumulator register
		case(6): {
			int num = std::stoi(memory.get(memory_location));
			num = abs(num);
			num = -num;
			accumulator_register = std::to_string(num);
			return;
		}
		//accumulator register  = multiplier quotient register
		case(7): {
			accumulator_register = multiplier_quotient_register;
			return;
		}
		//jump to line i.e. set program counter to memory location x
		case(8):{
			program_counter = memory_location;
			return;
		}
		//jump to line only if accumulator register is nonnegative
		case(9):{
			if (stringToInt(accumulator_register) >= 0){
				program_counter = memory_location;
			}
			return;
		}
		//add value at memory location x to accumulator register
		case(10):{
			int ac = stringToInt(accumulator_register);
			int num = stringToInt(memory.get(memory_location));
			ac += num;
			accumulator_register = std::to_string(ac);
			return;
		}
		//add absolute value of 'value at memory location x' to accumulator register
		case(11):{
			int ac = stringToInt(accumulator_register);
			int num = stringToInt(memory.get(memory_location));
			num = abs(num);
			ac += num;
			accumulator_register = std::to_string(ac);
			return;
		}
		//subtract value at memory location x from accumulator register
		case(12):{
			int ac = stringToInt(accumulator_register);
			int num = stringToInt(memory.get(memory_location));
			ac -= num;
			accumulator_register = std::to_string(ac);
			return;
		}
		//subtract absolute value of 'value at memory location x' from accumulator register
		case(13):{
			int ac = stringToInt(accumulator_register);
			int num = stringToInt(memory.get(memory_location));
			num = abs(num);
			ac -= num;
			accumulator_register = std::to_string(ac);
			return;
		}
		//multiply value at memory location x by the value in the multiplier quotient register 
		//and then store into accumulator register

		case(14):{
			int mq = stringToInt(multiplier_quotient_register);
			int num = stringToInt(memory.get(memory_location));
			mq *= num;
			accumulator_register = std::to_string(mq);
			return;
		}
		//divide accumulator register bu the value at memory location x
		//store answer in multiplier quotient register
		//store remainder in accumulator register
		case(15):{
			int ac = stringToInt(accumulator_register);
			int temp = ac;
			int num = stringToInt(memory.get(memory_location));
			ac /= num;
			multiplier_quotient_register = std::to_string(ac);
			temp %= num;
			accumulator_register = std::to_string(temp);
			return;
		}

		//multiply accumulator register by 2 (shift bit left by 1)

		case(16):{
			int ac = stringToInt(accumulator_register);
			ac *= 2;
			accumulator_register = std::to_string(ac);
			return;
		}
		//divide accumulator register by 2 (shift bit right by 1)

		case(17):{
			int ac = stringToInt(accumulator_register);
			ac /= 2;
			accumulator_register = std::to_string(ac);
			return;
		}
		default: {
			return;
		}
	}
	return;
}

//+--------------------------+

//|		Helper functions     |

//+--------------------------+
int stringToInt(std::string string){
	return stoi(string);
}

void printMem(Memory& memory, int line_number){
	for (int i = 0; i <line_number; i++){
		std::cout << i << " " << memory.get(i) << std::endl;
	}
}

void printMessage(std::string message){
	std::cout << std::endl << std::endl;
	printf("|      %s      |\n", message.c_str());
	std::cout << std::endl;
}
void printExec1(){
	std::cout << "_______________________________" << std::endl << std::endl;
	std::cout << "PC: " << program_counter << "  ";
	std::cout << "IR: " << instruction_register << std::endl;
}

void printExec2(){
	std::cout << "_______________________________" << std::endl << std::endl;
	std::cout << "PC: " << program_counter << "  ";
	std::cout << "AC: " << accumulator_register << "  ";
	std::cout << "MQ: " << multiplier_quotient_register << std::endl;

}

void printCode(std::string filename){
	std::ifstream in(filename);
	if (in.is_open()){
		while (true){
			if (in.eof()){
				break;
			}
			std::string temp;
			std::getline(in, temp);
			std::cout << temp << std::endl;
		}
	}
	in.close();
	return;
}