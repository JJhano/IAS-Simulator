//Adrian Barberis

//COSC 2030

//Nov 13 2016

//Memory Simulation Class

#ifndef Memory_h

#define Memory_h

#include<string>
class Memory {
	public:

		Memory();

		const auto get(int index);

		const auto get();

		const auto size(); //you may not like this but I think its useful

		template<typename T> void set(T data, int index);

	private:

		std::string _memory[1000];

};

Memory::Memory()
{
	for (int i = 0; i < (sizeof(_memory)/sizeof(*_memory)); i++)
	{
		_memory[i] = "nop";
	}
}

const auto Memory::get(int index){
	return _memory[index];
}
const auto Memory::get(){
	return _memory;
}

const auto Memory::size()
{
	return sizeof(_memory) / sizeof(*_memory);
}

template<typename T> void Memory::set(T data, int index)
{

	_memory[index] = data;

}





























































#endif /* Memory_h */